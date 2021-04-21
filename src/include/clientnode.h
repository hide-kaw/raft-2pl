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



#ifndef CLIENT_NODE_H
#define CLIENT_NODE_H


using std::mutex;
using std::vector;

class ClientNode : public Node {
private:
	int lastLogIndex;
	int commitIndex;

	mutex _mtx;

	int lastCommandId;
	int committedCommandId; // 不要？

	bool needReadRequest;
	int readRPCID;
	vector<bool> readGrants;

  int recvSock;
  int sendSock;


public:
	ClientNode(std::string* hostname, int port);

	int getLastLogIndex();
	void setLastLogIndex(int lastIndex);

	int getCommitIndex();
	void setCommitIndex(int commitIndex);

	int getLastCommandId();
	void setLastCommandId(int lastCommandId);

	int getCommittedCommandId();  
	void setCommittedCommandId(int committedCommandId);

	void setReadRPCID(int rpcid);
	int getReadRPCID();
	void setNeedReadRequest(bool b);
	bool getNeedReadRequest();
	int getReadGrantsNum(int size);
	void grant(int raftNodeId);
	void resetReadGrants(int size);

	int getRecvSock();
	int getSendSock();
	void setRecvSock(int sock);
	void setSendSock(int sock);
	void closeSendSock();
	void closeRecvSock();

};
#endif
