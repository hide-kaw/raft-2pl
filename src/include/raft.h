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


#ifndef RAFT_H
#define RAFT_H

#include "constant.h"
#include "header.h"

#include <vector>
#include <queue>
#include <map>
#include <chrono>
#include <mutex>
#include <thread>
#include <stdio.h>
#include <string.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> 

#include <sys/time.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <assert.h>
#include <algorithm>
#include <ctime>
#include <unistd.h>

#include "config.h"
#include "debug.h"
#include "kvs.h"
#include "status.h"
#include "node.h"
#include "raftnode.h"
#include "clientnode.h"
#include "worker.h"
#include "entry.h"
#include "log.h"
#include "rpc.h"
#include "functions.h"

using std::vector;
using std::map;
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::mutex;
using std::thread;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::sort;
using std::min;
using std::vector;
using std::queue;
using std::string;
using std::getline;
using std::random_device;
using std::stringstream;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::system_clock;

//int group_size_w[MAX_CLUSTER_NUM][MAX_GROUP_ENTRY] = {0};
//vector<int> GroupSize;

class Config;
class Status;
class KVS;
class Raft;
class RaftNode;
class ClientNode;
class WorkerInfo;

/*
high_resolution_clock::time_point first_log_time;
high_resolution_clock::time_point end_time;
system_clock::time_point start;
system_clock::time_point end;
*/

typedef struct _worker_args {
	Raft* raft;
	RaftNode* rNode;
	ClientNode* cNode;
	bool isClient;
	HEADER header;
} worker_args;


class Raft {
private:
	mutex _mtx;
	mutex _mtx_next_index;
	
	Config* config;
	Status* status;
	KVS* kvs;
	vector<RaftNode*>* raftNodes;
	vector<ClientNode*>* clientNodes;
  vector<WorkerInfo*>* workerInfos;
	int me;
	high_resolution_clock::time_point startTime;

	int leaderTerm;
	int vote;
	int _lsn = 0; // log sequence number
	
	int commitCount;
  //int notifierSock;

	map<int, int> readRPCIds;

	mutex _mtx_ackQ;
	mutex _mtx_lsn; // log sequence number
	queue<response_append_entries> ackQ;
	
	/* === private functions === */
	void setRaftNodesByConfig();
	//void candidacy();
	//void outputMeasureResults();
	//void sendCommitMessage();

	void setupLeader();
	void setupFollower(int sockfd, HEADER header);
  void setupNotifier();
	void notifier(int sockfd, int rNodeId, HEADER header);
	void worker(WorkerInfo* workerInfo);
	void committer();	
	void producer(int sockfd, HEADER h, ClientNode *cNode, int workerId);	
	void logReceiver(int sockfd, HEADER header);
	
	void closeClient(ClientNode* cNode, int sockfd);


  void sendRPC(int sockfd, HEADER h, char *payload);
  //void sendRPC(RaftNode* rNode, HEADER h, char *payload);
  void sendRPC(ClientNode* me,  HEADER h, char *payload);
  //void sendRPC(Raft* raft, RaftNode* rNode, HEADER h, append_entries_rpc* payload, entry groupEntry[], const uint szGroup);
  int connect2raftnode(RaftNode* rNode);
  //void startWorkerThread(Raft* raft, RaftNode* rNode, ClientNode* cNode, bool isClient, HEADER header);
  //void requestLocationReceived(Raft* raft, ClientNode* cNode, uint size, uint sockfd);
  //void unlock_and_setIndex(Raft *raft, const int nIndex);
  //void responseAppendEntriesReceived(Raft* raft, RaftNode* rNode, uint size, uint sockfd);
	int getLogSequenceNumber();
	
	response_append_entries ackDequeue();
	void ackEnqueue(response_append_entries ack);

public:
	Raft(char* configFileName);
  
  void lock() { _mtx.lock(); }
  void unlock() { _mtx.unlock(); }
  void lock_next_index() { _mtx_next_index.lock(); }
  void unlock_next_index() { _mtx_next_index.unlock(); }

	int incrementCommitCount();

	void receiver();
	void transmitter();
	void resetTimeoutTime();
	void cli();

	bool isTimeout();
	microseconds getDuration();
	void resetStartTime();

	Config* getConfig();
	Status* getStatus();
	KVS* getKVS();

	vector<RaftNode*>* getRaftNodes();
	RaftNode* getRaftNodeById(int id);
	RaftNode* getLeader();

  vector<WorkerInfo*>* getWorkerInfo();
  WorkerInfo* getWorkerInfoById(int id);

	int getLeaderId();

	vector<ClientNode*>* getClientNodes();
	void addClientNode(ClientNode* cNode);
	void delClientNode(ClientNode* cNode);

	void setMe(int me);
	int getMe();

	int getLeaderTerm();
	void setLeaderTerm(int leaderTerm);

	int getVoteElect();
	void setVoteElect(int vote);

	void apply(int index);

	void sendAppendEntriesRPC(RaftNode* rNode, bool isHeartBeat);
	void sendHeartBeat();
	void sendLogEntry();

	void enqueue(); // revise input argument
	void dequeue(); // revise return value
  


};
#endif
