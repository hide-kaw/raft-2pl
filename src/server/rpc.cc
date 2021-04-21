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



#include "rpc.h"

RPCKind discernRPC(char* str) {
	char c = str[0];
	if (c >= '0' && c <= '9') {
		return (RPCKind)(c - '0');
	}
	return (RPCKind)-1;
}

append_entries_rpc arpcByFields(int term,	int leaderId,	int prevLogIndex,	int prevLogTerm, int leaderCommit, trans_req log[MAX_GROUP_ENTRY], int szLog, int lsn, int workerId, int rNodeId)
{
	append_entries_rpc arpc;
	arpc.rpcKind      = APPEND_ENTRIES;
	arpc.term         = term;
	arpc.leaderId     = leaderId;
	arpc.prevLogIndex = prevLogIndex;
	arpc.prevLogTerm  = prevLogTerm;
	arpc.leaderCommit = leaderCommit;
	memcpy(arpc.log, log, sizeof(trans_req) * szLog);
	arpc.szLog = szLog;
  arpc.lsn = lsn;
  arpc.workerId = workerId;
  arpc.rNodeId = rNodeId;
	return arpc;
}

/*
append_entries_rpc arpcByFields(int term,	int leaderId,	int prevLogIndex,	int prevLogTerm, int leaderCommit, entry groupEntry[MAX_GROUP_ENTRY],	int szGroup)
{
	append_entries_rpc arpc;
	arpc.rpcKind      = APPEND_ENTRIES;
	arpc.term         = term;
	arpc.leaderId     = leaderId;
	arpc.prevLogIndex = prevLogIndex;
	arpc.prevLogTerm  = prevLogTerm;
	arpc.leaderCommit = leaderCommit;
	//memcpy(arpc.groupEntry, groupEntry, sizeof(entry) * szGroup);
	//arpc.szGroup      = szGroup;
	return arpc;
}
*/

request_vote_rpc rrpcByFields(Raft* raft)
{
	request_vote_rpc rrpc;

	rrpc.rpcKind      = REQUEST_VOTE;
	//rrpc.term         = raft->getStatus()->getCurrentTerm();
	//rrpc.candidateId  = raft->getMe();
	//rrpc.lastLogIndex = raft->getStatus()->getLog()->lastLogIndex();
	//rrpc.lastLogTerm  = raft->getStatus()->getLog()->lastLogTerm();

	/*
	cout << " term: " << rrpc.term 
			 << " candidateid: " << rrpc.candidateId 
			 << " lastlogindex: " << rrpc.lastLogIndex 
			 << " lastlogterm: " << rrpc.lastLogTerm 
			 << endl;
	*/
	return rrpc;
}

response_append_entries raeByFields(int term, bool success, uint lsn, int workerId, int rNodeId)
{
	response_append_entries rae;
	rae.rpcKind       = RESPONSE_APPEND_ENTRIES;
	rae.term          = term;
	rae.success       = success;
	rae.lsn 		  = lsn;
  rae.workerId = workerId;
  rae.rNodeId = rNodeId;
	return rae;
}

response_request_vote rrvByFields(int term,	bool success) {
	response_request_vote rrv;
	rrv.rpcKind = RESPONSE_REQUEST_VOTE;
	rrv.term    = term;
	rrv.success = success;
	return rrv;
}

response_request_location
rrlByFields(const char hostname[LEN_HOST], int port) {
	response_request_location rrl;
	rrl.rpcKind = RESPONSE_REQUEST_LOCATION;
	strncpy(rrl.hostname, hostname, LEN_HOST);
	rrl.port = port;
	return rrl;
}


vector<string>
StringSplit(const string &str, char sep)
{
	vector<string> v;
	stringstream ss(str);
	string buffer;
	while (getline(ss, buffer, sep)) {
		v.push_back(buffer);
	}
	return v;
}

commit_message
cmByFields(int commandId)
{
	commit_message cm;
	cm.rpcKind = COMMIT_MESSAGE;
	cm.commandId = commandId;
	return cm;
}
