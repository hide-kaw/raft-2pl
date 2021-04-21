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


#include "client.h"

typedef struct _HOST {
	char hostname[LEN_HOST];
	uint port;
} HOST;

request_location rlByFields()
{
	request_location rl;
	rl.rpcKind = REQUEST_LOCATION;
	return rl;
}

HOST getHost(char *config_file)
{
	char buf[BUFSIZ];
	char hostname_port[LEN_HOST]; // hostname:port
	ifstream ifs_config(config_file); if (!ifs_config) ERR;
	ifs_config.getline(buf, BUFSIZ);
	strcpy(hostname_port, buf);
	ifs_config.close();

	vector<std::string> v = split(hostname_port, ':');
	HOST h;
	strcpy(h.hostname, v[0].c_str());
	h.port = stoi(v[1].c_str());

	return h;
}

vector<client_command> getCC(char *command_file)
{
	ifstream ifs(command_file); if (!ifs) ERR;
	vector <client_command> vec_cc;
	client_command cc;
	char buf[BUFSIZ];

	cc.rpcKind = CLIENT_COMMAND;
	int counter = 0;
	while (ifs.getline(buf, BUFSIZ)) {
		sscanf(buf, "%d%d%d", &cc.req[counter].from, &cc.req[counter].to, &cc.req[counter].diff);
		if (++counter == MAX_CLI_REQ) {
			vec_cc.push_back(cc);
			counter = 0;
		}
	} ifs.close();
	
	return vec_cc;
}

void sendRPC(int sockfd, HEADER h, char *payload)
{
	int sz = sizeof(h) + h.size;
	char *p = (char *)malloc(sz);
	memcpy(p, &h, sizeof(h));
	memcpy(p + sizeof(h), payload, h.size);
	if (S_SEND(sockfd, p, sz, 0) < 0) ERR;
	free(p);
}

int conn(char* hostname, int port) {
	char port_str[LEN_PORT];
	sprintf(port_str, "%d", port);

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

	int err = S_GETADDRINFO(hostname, port_str, &hints, &ai);	if (err) ERR;
	if ((fd = S_SOCKET(ai->ai_family, SOCK_STREAM, 0)) == -1) ERR;

	// set internal buffer size
	int val = (1 << 21);
	if (S_SETSOCKOPT(fd, SOL_SOCKET, SO_SNDBUF, &val, sizeof(val)) == -1) ERR;
	if (S_SETSOCKOPT(fd, SOL_SOCKET, SO_RCVBUF, &val, sizeof(val)) == -1) ERR;

#ifdef ENABLE_RSOCKET
	val = 0; // optimization for better bandwidth
	if (S_SETSOCKOPT(fd, SOL_RDMA, RDMA_INLINE, &val, sizeof(val)) == -1) ERR;
#endif // ENABLE_RSOCKET

	// connect to the server
	if (S_CONNECT(fd, S_DST_ADDR(ai), S_DST_ADDRLEN(ai)) == -1) ERR;
	S_FREEADDRINFO(ai);

	return fd;
}

void print_vec_cc(vector<client_command> vec_cc)
{
	for (auto itr = vec_cc.begin(); itr != vec_cc.end(); itr++) {
		//printf("%d %d\n", itr->rpcKind, itr->commandId);
		for (int i = 0; i < MAX_CLI_REQ; i++) {
			printf("%d %d %d\n", itr->req[i].from, itr->req[i].to, itr->req[i].diff);
		}
	}
}

int
main(int argc, char* argv[])
{
	if (argc < 2) {
		cout << "Please specify an input file name and a config filename." << endl; exit(0);
	}
	signal(SIGPIPE, SIG_IGN);

	vector<client_command> vec_cc = getCC(argv[1]);
	//print_vec_cc(vec_cc);
	HOST host = getHost(argv[2]);
	int sockfd;
	response_request_location rrl;
  int txCount = atoi(argv[3]);

  int committimes[10];
  memset(committimes, 0, sizeof(committimes));

	// connect to a raft server
	while (true) {
		HEADER h; h.kind = REQUEST_LOCATION; h.size = sizeof(request_location);
		request_location rl = rlByFields();
		cout << "Connecting to " << host.hostname << ":" << host.port << endl;
		sockfd = conn(host.hostname, host.port);
		assert(h.kind == REQUEST_LOCATION);
		sendRPC(sockfd, h, (char *)&rl);
		if (S_RECV(sockfd, &h, sizeof(HEADER), MSG_WAITALL) < 0) ERR;
		if (S_RECV(sockfd, &rrl, sizeof(response_request_location), MSG_WAITALL) < 0) ERR;
		S(host.hostname);
		S(rrl.hostname);
		if (strcmp(host.hostname, rrl.hostname) == 0) break; // ok. connected.
		else {strcpy(host.hostname, rrl.hostname); close(sockfd);}
	}
		
	// start measurement

	std::chrono::system_clock::time_point start, end, transaction_start, request_end;
	start = std::chrono::system_clock::now();
	int commandId = 0;
	HEADER h; 
	for (auto itr = vec_cc.begin(); itr != vec_cc.end(); itr++) {
		h.size = sizeof(client_command); h.kind = CLIENT_COMMAND;
		client_command cc = *itr;

    transaction_start = std::chrono::system_clock::now();

		cc.commandId = commandId;
		sendRPC(sockfd, h, (char *)&cc);

    int commitCount = 0;
    double sum_commit_time = 0;

		while (true) {
			commit_message cm;
			if (S_RECV(sockfd, &h, sizeof(HEADER), MSG_WAITALL) < 0) ERR;
			if (S_RECV(sockfd, &cm, sizeof(cm), MSG_WAITALL) < 0) ERR;
			if (cm.commandId == commandId) {
        commitCount++;
        request_end = std::chrono::system_clock::now();
        double commit_time = std::chrono::duration_cast<std::chrono::microseconds>(request_end-transaction_start).count();
        sum_commit_time += commit_time;
      }
      if (commitCount == MAX_CLI_REQ) break;
		}
    double elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(request_end-start).count();
    double average_commit_time = sum_commit_time / commitCount;
    cout << "commandId," << commandId << ",txId," << commitCount << "," << average_commit_time/1000 << ",elapsed," << elapsed_time/1000 << endl;

		commandId++;
    if (itr==(vec_cc.end()-1)) itr = vec_cc.begin();
    if(commandId == txCount) break;
	}

	end = std::chrono::system_clock::now();
	double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();

	cout << "time: " << elapsed / 1000 << " seconds" << endl;

	// close connection
	h.kind = CLIENT_EXIT;
	h.size = sizeof(HEADER);

	sendRPC(sockfd, h, (char *)&h); // header and payload are same :-)
	if (S_RECV(sockfd, &h, sizeof(h) + sizeof(h), MSG_WAITALL) < 0) ERR;
	int ret = S_CLOSE(sockfd); if (ret < 0) ERR;
	return 0;
}
