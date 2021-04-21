/* 
 * Copyright (c) 2021 Keio University and Mitsubishi UFJ Information Technology, Ltd.
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
*/


#include "raft.h"
#include "worker.h"
#include <atomic>
#include <unistd.h>

int group_size[MAX_GROUP_ENTRY+1] = {0};

Raft::Raft(char* configFileName) {
	this->config = new Config(configFileName);
	this->raftNodes = new vector<RaftNode*>;
	this->clientNodes = new vector<ClientNode*>;
  this->workerInfos = new vector<WorkerInfo*>;
	this->setRaftNodesByConfig();
	cout << this->getConfig()->getStorageDirectoryName() << endl;

	this->status = new Status(this->getConfig()->getStorageDirectoryName());
	this->status->setState(FOLLOWER);
	this->resetTimeoutTime();
	this->kvs = new KVS();
	// others
	this->leaderTerm = this->status->getCurrentTerm();
	this->vote = 0;
	this->commitCount = 0;
  //this->notifierSock = -1;

	memset(group_size, 0, sizeof(group_size));
}

//
// Log receiver at follower
//
void Raft::receiver()
{
N;
	int fd;
	struct S_ADDRINFO hints;
	struct S_ADDRINFO *ai;

	memset(&hints, 0, sizeof(hints));

#ifdef ENABLE_RSOCKET
	hints.ai_flags = RAI_PASSIVE | RAI_FAMILY;
	hints.ai_port_space = RDMA_PS_TCP;
#else
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
#endif

	char port[LEN_PORT] = {0};
	sprintf(port, "%d", this->getRaftNodeById(this->getMe())->getListenPort());
	int err = S_GETADDRINFO(nullptr, port, &hints, &ai); if (err) ERR;
	if ((fd = S_SOCKET(ai->ai_family, SOCK_STREAM, 0)) == -1) ERR;

#ifdef ENABLE_RSOCKET
	val = 0; // optimization for better bandwidth
	if (S_SETSOCKOPT(fd, SOL_RDMA, RDMA_INLINE, &val, sizeof(val)) == -1) ERR;
#endif // ENABLE_RSOCKET

	int val = 1;
	if (S_SETSOCKOPT(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) ERR;
	if (S_BIND(fd, S_SRC_ADDR(ai), S_SRC_ADDRLEN(ai)) == -1) ERR;
	if (S_LISTEN(fd, 1024) == -1) ERR;

	// Service loop
	struct sockaddr_in c_addr;
	socklen_t len = sizeof(c_addr);
	HEADER header;

  int workerId = 0;

	while (true) {

		int accept_fd = S_ACCEPT(fd, (sockaddr*)&c_addr, &len); if (accept_fd == -1) ERR;
		
		if (S_RECV(accept_fd, &header, sizeof(HEADER), MSG_WAITALL) < 0) ERR;
	  //printRPC(__LINE__, header.kind);
    // Leader
    if (header.kind == REQUEST_LOCATION || header.kind == CLIENT_COMMAND) {
			assert(header.kind == REQUEST_LOCATION);
			string hostname(inet_ntoa(c_addr.sin_addr));
			ClientNode *cNode = new ClientNode(&hostname, (int)ntohs(c_addr.sin_port));
			cNode->setRecvSock(accept_fd);
			cNode->setSendSock(accept_fd);
			cNode->setID(this->getClientNodes()->size());
			this->addClientNode(cNode);
			//cout << __LINE__ << " " << cNode->getHostname() << " connected(Client Node). (sock=" << accept_fd << ")\n";


			thread producerThread(&Raft::producer, this, accept_fd, header, cNode, workerId);
		  producerThread.detach();
	
      // round robin
      if(workerId == this->workerInfos->size()-1){
        workerId = 0;
      } else {
        workerId++;
      }

    // Follower
    } else if (header.kind == REQUEST_VOTE){
		  for (RaftNode* rNode : *this->getRaftNodes()) {
			  if (!rNode->isMe() && inet_ntoa(c_addr.sin_addr) == rNode->getHostname()) {
				  cout << rNode->getHostname() << " connected(Notifier thread). (sock=" << accept_fd << ")\n";
       		S_RECV(accept_fd, NULL, header.size, MSG_WAITALL);
          this->status->setVotedFor(rNode->getID());
        }
      }
    } else if (header.kind == RESPONSE_APPEND_ENTRIES) {

		  for (RaftNode* rNode : *this->getRaftNodes()) {
			  if (!rNode->isMe() && inet_ntoa(c_addr.sin_addr) == rNode->getHostname()) {
				  cout << rNode->getHostname() << " connected(Notifier thread). (sock=" << accept_fd << ")\n";
			    thread notifierThread(&Raft::notifier, this, accept_fd, rNode->getID(), header);
			    notifierThread.detach();
        }
      }

    // Follower
    } else if (header.kind == APPEND_ENTRIES){
			for (RaftNode* rNode : *this->getRaftNodes()) {
	      if (!rNode->isMe() && inet_ntoa(c_addr.sin_addr) == rNode->getHostname()) {
					cout << rNode->getHostname() << " connected(aorker thread). (sock=" << accept_fd << ")\n";
					// create log receiver thread
					thread logReceiverThread(&Raft::logReceiver, this, accept_fd, header);  //accept_argsのような構造体が必要？
					logReceiverThread.detach();
				}
			}
    }
	}
}


void Raft::notifier(int sockfd, int rNodeId, HEADER header){
  N;
 /*
  * Followerからのakcを受け取る
  * ackをackQueueにつめる
  * receiverスレッドとは異なるPortで待ち受ける？ 
  */
  uint logIndex;
  char buf[sizeof(response_append_entries)];
  response_append_entries ack;
  
  KVS* kvs = this->getKVS();
	int clusterSize = this->getRaftNodes()->size();


  // 最初のackのheaderはすでにreceiverで読まれているのでここでは読まない
	if(header.kind == RESPONSE_APPEND_ENTRIES){
    if (S_RECV(sockfd, buf, header.size, MSG_WAITALL) < 0) ERR;
    memcpy(&ack, buf, sizeof(response_append_entries));
  }


  while(true){
	  if(header.kind == RESPONSE_APPEND_ENTRIES){
      WorkerInfo* workerInfo = this->getWorkerInfoById(ack.workerId);
      follower_info* fi = workerInfo->getFollowerInfoById(ack.rNodeId);
      int nextIndex = fi->nextIndex + 1;
      if (ack.success) {
				if (status->addVoteCount(ack.lsn) > (clusterSize/2)) {
          task t = workerInfo->taskLoad(ack.lsn);

					if (t.lsn != -1) { // 既にタスクがロード済であれば-1
              for(int i=0; i < t.nlog; i++){
    				    trans_req xact;
			          memcpy(&xact, &t.xactSet[i], sizeof(trans_req));

                int fromKey = xact.req.from;
                int toKey = xact.req.to;
                int diffVal = xact.req.diff;

		            int fromVal = kvs->index_read(fromKey);	
		            int toVal = kvs->index_read(toKey);	
                fromVal -= diffVal;
                toVal += diffVal;

		            kvs->index_update(fromKey, fromVal);
		            kvs->index_update(toKey, toVal);

                //unlock
                kvs->unlock(fromKey);
                kvs->unlock(toKey);
          
                workerInfo->setCommitIndex(nextIndex);
   	            for (ClientNode* cNode : *this->getClientNodes()) {
	   	            if (cNode->getID() == xact.clientId) {
		  	            HEADER h; h.size = sizeof(commit_message); h.kind = COMMIT_MESSAGE;
		  	            commit_message cm = cmByFields(xact.commandId);
		  	            this->sendRPC(cNode, h, (char *)&cm);
       	            break;
                  }
                }
            }
          }
        }
      }
  	}
    // 次のackを待ち受け
    if (S_RECV(sockfd, &header, sizeof(HEADER), MSG_WAITALL) < 0) ERR;
    if (S_RECV(sockfd, buf, header.size, MSG_WAITALL) < 0) ERR;
    memcpy(&ack, buf, sizeof(response_append_entries));
  }
}

int Raft::getLogSequenceNumber()
{
	int lsn;
	static std::atomic<int> _lsn(0); 
	lsn = atomic_fetch_add(&_lsn, 1); 
	return lsn;
}


struct timeval
setLogSend(void)
{
	static struct timeval t;
	gettimeofday(&t, NULL);

	return t;
}

bool
timeOut(struct timeval prev)
{
	struct timeval cur;
	gettimeofday(&cur, NULL);

	long diff = (cur.tv_sec - prev.tv_sec) * 1000 * 1000 + (cur.tv_usec - prev.tv_usec);
	if (diff >= LOG_TIMEOUT) {
		return true;
	}

	return false;
}


void Raft::worker(WorkerInfo* workerInfo) {
  N;

	int buffid;
	trans_req logBuffer[MAX_GROUP_ENTRY];
	int workerId = workerInfo->getWorkerId();
	Log *log = workerInfo->getLog();
	struct timeval preLogSend;
	log = new Log(this->getConfig()->getStorageDirectoryName(), workerId);
	
	//int matchIndex; // found in paper

	gettimeofday(&preLogSend, NULL);
	buffid = 0;
	bool canDoXactDeq = true;
	trans_req xact;

	while (true) {

		// TxQueueからTxを取り出す
		if (canDoXactDeq == true) {
			xact = workerInfo->xactDequeue();
	  } 
		if (xact.clientId != -1) { // Xact exists!

      assert(xact.req.from != xact.req.to);

      // sort
      int tmp;
      if(xact.req.from > xact.req.to){
        tmp = xact.req.from;
        xact.req.from = xact.req.to;
        xact.req.to = tmp;
      }

		  if (kvs->lock(xact.req.from) == true) {
				if (kvs->lock(xact.req.to) == true) {
					// Insert to local log buffer
          memcpy(&logBuffer[buffid], &xact, sizeof(trans_req));
 					canDoXactDeq = true;
          buffid++;
				} else { // [to] lock failure
					kvs->unlock(xact.req.from);
					canDoXactDeq = false;
				}
			} else {
				canDoXactDeq = false;
			}
		}			

		if ((timeOut(preLogSend) && buffid > 0) || (buffid == MAX_GROUP_ENTRY)) {
      assert(buffid >0 );


			entry e;
			int lsn = getLogSequenceNumber(); // lsn == global-log-index
      // uint
      assert(lsn>=0);

			int nlog = buffid;
			buffid = 0;

			e.lsn = lsn;
			e.term = status->getCurrentTerm();
			e.nlog = nlog;
			memcpy(e.xactSet, logBuffer, sizeof(trans_req) * nlog);
			gettimeofday(&e.time, NULL);
			// Write log to storage
			log->addEntry(e);

			
			// Send to follower
			for (follower_info* fi : *workerInfo->getFollowers()) {
				HEADER h;
				h.kind = APPEND_ENTRIES;
				h.size = sizeof(append_entries_rpc);
				append_entries_rpc packet;
				Status* status = this->getStatus();
				int prevLogIndex = fi->nextIndex - 1;
				int curTerm = status->getCurrentTerm();
				int prevTerm = curTerm - 1;
				int commitIndex = workerInfo->getCommitIndex();
				packet = arpcByFields(curTerm, this->getMe(), prevLogIndex, prevTerm, commitIndex, logBuffer, nlog, lsn, workerId, fi->followerNodeId);
				
				if (fi->sockfd == -1) {
					for (RaftNode* rNode : *this->getRaftNodes()) {
						if (!rNode->isMe() && rNode->getID() == fi->followerNodeId) {
							fi->sockfd =  this->connect2raftnode(rNode);
						}
					}
				}
				sendRPC(fi->sockfd, h, (char*)&packet);
				preLogSend = setLogSend();
			}

			// Store Task (setLockMemo)
			task t;
			memcpy(t.xactSet, logBuffer, sizeof(trans_req) * nlog);
			t.lsn = lsn;
			t.nlog = nlog;
			t.workerId = workerId;
			workerInfo->taskStore(t);


		}
	}
}




//
// log receiver at follower node
//
void Raft::logReceiver(int sockfd, HEADER header){
  /*
   * LeaderのWorkerからLogGrpを受け取る
   * 自分のログにLogGrpを書き込む
   * クライアントに応答を返す
   */
N;
  append_entries_rpc arpc;
  Status* status = this->getStatus();
  int curTerm = status->getCurrentTerm();
	HEADER h; h.kind =	RESPONSE_APPEND_ENTRIES; h.size =	sizeof(response_append_entries);
  RaftNode* leader = this->getLeader();
	int notifierSock =  this->connect2raftnode(leader);
  char buf[sizeof(append_entries_rpc)];  // + sizeof(trans_req) * MAX_CLI_REQ?
  bool grant = true;
	  
  // 最初のAppendEntriesはReceiverでheaderを読んでいるのでここでは読まない
  if (S_RECV(sockfd, buf, header.size, MSG_WAITALL) < 0) ERR;
	memcpy(&arpc, buf, sizeof(append_entries_rpc));
 	Log *log = new Log(this->getConfig()->getStorageDirectoryName(), arpc.workerId);

  while(true){

    grant = true;
    entry e;
		int lsn = arpc.lsn; // lsn == global-log-index
		int nlog = arpc.szLog;
		e.lsn = lsn;
		e.term = arpc.term;
		e.nlog = nlog;
    assert(lsn>=0);
    assert(nlog>0);
		memcpy(e.xactSet, arpc.log, sizeof(trans_req) * nlog);
		gettimeofday(&e.time, NULL);
	  // Write log to storage
		log->addEntry(e);

    // set follower commit Index
    // commitIndexをファイルに永続化する
	  response_append_entries rae = raeByFields(curTerm, grant, arpc.lsn, arpc.workerId, arpc.rNodeId);

	  usleep(DISTANCE_DELAY);
	  this->sendRPC(notifierSock, h, (char*)&rae);	

    // 次のAppendEntriesRPCのheaderを待ち受け
	  if (S_RECV(sockfd, &header, sizeof(HEADER), MSG_WAITALL) < 0) ERR;
    if (S_RECV(sockfd, buf, header.size, MSG_WAITALL) < 0) ERR;
	  memcpy(&arpc, buf, sizeof(append_entries_rpc));

  }
}


void Raft::ackEnqueue(response_append_entries ack)
{
	_mtx_ackQ.lock();
	this->ackQ.push(ack);
	_mtx_ackQ.unlock();
}

response_append_entries Raft::ackDequeue()
{
	response_append_entries ack;

	_mtx_ackQ.lock();
  if (this->ackQ.empty() == true){
   	   ack.rNodeId = -1;
	  _mtx_ackQ.unlock();
  }else {
    ack = this->ackQ.front();
    this->ackQ.pop();	
	  _mtx_ackQ.unlock();
  }
	return ack;
}


void Raft::producer(int sockfd, HEADER header, ClientNode *cNode, int workerId){
N;
  /* 
   * clientからTxGrpを受け取る
   * TxGrpをTxごとに分割
   * clientId、commandId、TxId、TxをQueueに詰めるk
   */

  WorkerInfo* workerInfo = this->getWorkerInfoById(workerId);

  // response request location
  RaftNode* leader = this->getLeader();
  request_location rl;
  S_RECV(sockfd, &rl, header.size, MSG_WAITALL);
  response_request_location rrl = rrlByFields(leader->getHostname().c_str(), leader->getListenPort());
  HEADER h; h.kind = RESPONSE_REQUEST_LOCATION;	h.size = sizeof(response_request_location);

  this->sendRPC(cNode, h, (char *)&rrl);

  client_command cc;
  trans_req tx;

  while(true) {
	  if (S_RECV(sockfd, &header, sizeof(HEADER), MSG_WAITALL) < 0) ERR;

    if (header.kind == CLIENT_COMMAND){
	  if (S_RECV(sockfd, &cc, header.size, MSG_WAITALL) < 0) ERR;


      for (int i = 0; i < MAX_CLI_REQ; i++) {
			  memcpy(&tx.req, &cc.req[i], sizeof(client_request));
			  tx.commandId = cc.commandId;
			  tx.clientId = cNode->getID(); 
			  workerInfo->xactEnqueue(tx);
      }
    } else {
      // close client connection 
      this->closeClient(cNode, sockfd);
      break;
    }
  }
}


void Raft::setupLeader()
{

  int sockfd;
  // dummy vote
  this->getStatus()->setVotedFor(this->getMe());
 	for (RaftNode* rNode : *this->getRaftNodes()) {
	  if (!rNode->isMe()) {
			sockfd = this->connect2raftnode(rNode);
			HEADER h; h.kind = REQUEST_VOTE; h.size = sizeof(request_vote_rpc);
			request_vote_rpc rrpc = rrpcByFields(this);
			this->sendRPC(sockfd, h, (char*)&rrpc);
		}
	}


	this->status->becomeLeader();	

	// create worker thread
	for(int i=0; i<WORKER_NUM; i++){
    	vector<follower_info*> *followers = new vector<follower_info*>;
   	for (RaftNode* rNode : *this->getRaftNodes()) {
			if (!rNode->isMe()) {
        follower_info* follower = (follower_info*)malloc(sizeof(follower_info));
        follower->sockfd = -1;
        follower->followerNodeId = rNode->getID();
				
        followers->push_back(follower);
			}
		}   
		WorkerInfo* workerInfo = new WorkerInfo(i, followers);
		this->workerInfos->push_back(workerInfo);
		thread workerThread(&Raft::worker, this, workerInfo);
		workerThread.detach();
	}


}


void Raft::transmitter() // or timer
{
	if (CAN_BE_CANDIDACY) {
		this->setupLeader();
	} 
	while(true){
		// transmitter
	}
}


void Raft::resetTimeoutTime() {
	int tt = myrand(MIN_TIMEOUTTIME_MICROSECONDS, MAX_TIMEOUTTIME_MICROSECONDS);
	this->status->setTimeoutTime(tt);
}

microseconds Raft::getDuration() {
	return duration_cast<microseconds>(high_resolution_clock::now() - this->startTime);
}
void Raft::resetStartTime() {
	this->startTime = high_resolution_clock::now();
}

Config* Raft::getConfig() {
	return this->config;
}
Status* Raft::getStatus() {
	return this->status;
}
KVS* Raft::getKVS() {
	return this->kvs;
}

void Raft::setRaftNodesByConfig() {
	int cnt = 0;
	for (node_conf* nconf : this->config->getNodes()) {
		RaftNode* rNode = new RaftNode(nconf->hostname, nconf->port);
		rNode->setID(cnt);
		this->raftNodes->push_back(rNode);
		cnt++;
	}
	
	struct ifaddrs* ifa_list;
	struct ifaddrs* ifa;

	int n;
	char addrstr[256];

	n = getifaddrs(&ifa_list);
	if (n != 0) {
		exit(1);
	}

	for (ifa = ifa_list; ifa != NULL; ifa = ifa->ifa_next) {
		memset(addrstr, 0, sizeof(addrstr));

		if (ifa->ifa_addr->sa_family == AF_INET) {
			inet_ntop(AF_INET,
				&((struct sockaddr_in*)ifa->ifa_addr)->sin_addr,
				addrstr, sizeof(addrstr));
			for (RaftNode* rNode : *this->getRaftNodes()) {
				if (strcmp(rNode->getHostname().c_str(), addrstr) == 0) {
					rNode->setIsMe(true);
					this->setMe(rNode->getID());
				}
			}
		}
	}

	// Debug 
	for (RaftNode* rNode : *this->getRaftNodes()) {
		if (rNode->isMe()) {
			cout << "I am \"" << rNode->getHostname() << ":\t" << rNode->getListenPort() << "\"." << endl;
		}
	}

	freeifaddrs(ifa_list);
}

vector<RaftNode*>* Raft::getRaftNodes() {
	return this->raftNodes;
}
RaftNode* Raft::getRaftNodeById(int id) {
	return (*this->raftNodes)[id];
}

WorkerInfo* Raft::getWorkerInfoById(int id){
	WorkerInfo* ret;
  for(WorkerInfo* wi : *this->workerInfos){
    if (wi->getWorkerId() == id) {
			ret = wi;
		}
  }

	return ret;
}

RaftNode* Raft::getLeader() {
	return (*this->raftNodes)[this->status->getVotedFor()];
}

int Raft::getLeaderId() {
	return this->status->getVotedFor();
}

vector<ClientNode*>* Raft::getClientNodes() {
	return this->clientNodes;
}

void Raft::addClientNode(ClientNode* cNode) {
	this->clientNodes->push_back(cNode);
}

void Raft::delClientNode(ClientNode* cNode) {
	for (auto itr = this->clientNodes->begin(); itr != this->clientNodes->end(); itr++) {
		if (cNode == *itr) {
			this->clientNodes->erase(itr);
			free(cNode);
			break;
		}
	}
}

void Raft::setMe(int me) {
	this->me = me;
}

int Raft::getMe() {
	return this->me;
}

int Raft::getVoteElect() {
	return this->vote;
}

void Raft::setVoteElect(int vote) {
	_mtx.lock();
	this->vote = vote;
	_mtx.unlock();
}

int Raft::getLeaderTerm() {
	return this->leaderTerm;
}
void Raft::setLeaderTerm(int leaderTerm) {
	this->leaderTerm = leaderTerm;
}

void Raft::sendRPC(int fd, HEADER h, char *payload) {
    int sz = sizeof(HEADER) + h.size;
	  char *p = (char *)malloc(sz);
	  memcpy(p, &h, sizeof(HEADER));
	  memcpy(p+sizeof(HEADER), payload, h.size);
 	  S_SEND(fd, p, sz, 0);
	  free(p);
}

void Raft::sendRPC(ClientNode* me, HEADER h, char *payload)
{
	int sz = sizeof(HEADER) + h.size;
	char *p = (char *)malloc(sz);
	memcpy(p, &h, sizeof(HEADER));
	memcpy(p+sizeof(HEADER), payload, h.size);

	bool ok = true;
	if (S_SEND(me->getSendSock(), p, sz, 0) < 0) {
		perror("write");
		std::cout << "   node is " << me->getHostname() << std::endl;
		ok = false;
	}
	free(p);

	if (!ok) {
		vector<ClientNode *>* cn = this->getClientNodes();
		for (auto itr = cn->begin(); itr != cn->end(); itr++) {
			if (*itr == me) {	cn->erase(itr); break; }
		}
	}
}

int Raft::connect2raftnode(RaftNode* rNode) {
	// return if I have already connected to rNode
	//if (rNode->getSendSock() > 0) return -1;

	// here....
	int fd;
	struct S_ADDRINFO hints;
	struct S_ADDRINFO *ai;

	memset(&hints, 0, sizeof(hints));
#ifdef ENABLE_RSOCKET
	hints.ai_flags = RAI_FAMILY;
	hints.ai_port_space = RDMA_PS_TCP;
#else
	hints.ai_flags = 0;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
#endif // RNETLIB_ENABLE_VERBS

	char hostname[LEN_HOST];
	strcpy(hostname, rNode->getHostname().c_str());
	char port[LEN_PORT];
	sprintf(port, "%d", rNode->getListenPort());

	int err = S_GETADDRINFO(hostname, port, &hints, &ai); if (err) ERR;	
	if ((fd = S_SOCKET(ai->ai_family, SOCK_STREAM, 0)) == -1) ERR;

	// set internal buffer size
#ifdef ENABLE_RSOCKET
	int val = 0; // optimization for better bandwidth
	if (S_SETSOCKOPT(fd, SOL_RDMA, RDMA_INLINE, &val, sizeof(val)) == -1) {
		perror("setsockopt (RDMA_INLINE)");
		S_CLOSE(fd);
		S_FREEADDRINFO(ai);
		exit(1);
	}
#endif // ENABLE_RSOCKET

	// connect to the server
	if (S_CONNECT(fd, S_DST_ADDR(ai), S_DST_ADDRLEN(ai)) == -1) ERR;
	//else rNode->setSendSock(fd);
	S_FREEADDRINFO(ai);
  return fd;
}


void Raft::closeClient(ClientNode* cNode, int sockfd)
{
	HEADER h; h.size = sizeof(HEADER); h.kind = BYE_FROM_RAFT;
	sendRPC(cNode, h, (char *)&h); // header and payload are same :-)

  // 一時的にコメントアウト
	/* vector<ClientNode *>* cn = this->getClientNodes(); */
	/* // Delete me */
	/* for (auto itr = cn->begin(); itr != cn->end(); itr++) { */
	/* 	if (*itr == cNode) { */	
	/* 		cn->erase(itr); */
	/* 		break; */
	/* 	} */
	/* } */
	cNode->closeRecvSock();
	free(cNode);
}


