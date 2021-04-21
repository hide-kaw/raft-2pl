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


#ifndef RPC_H
#define RPC_H

#include <vector>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>      // ostringstream
#include <stdio.h>
#include <string.h> // for memcpy

#include "debug.h"
#include "header.h"
#include "entry.h"
#include "status.h"
#include "log.h"
#include "constant.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::min;
using std::vector;
using std::string;
using std::stringstream;
using std::vector;
using std::string;

class Raft;


// append_entries_rpc
typedef struct _append_entries_rpc {
	RPCKind rpcKind;
	int term;         // leader's term
	int leaderId;
	int prevLogIndex;
	int prevLogTerm;
	int leaderCommit; // leader's commitIndex
	int lsn;
	trans_req log[MAX_GROUP_ENTRY];
	int szLog;

  int workerId;
  int rNodeId;
	/*
		entry groupEntry[MAX_GROUP_ENTRY];
		int szGroup; // used to redo at follower
	*/
} append_entries_rpc;

// commit_message
typedef struct _commit_message {
	RPCKind rpcKind;
	int commandId;
} commit_message;

// response_append_entries
typedef struct _response_append_entries {
	RPCKind rpcKind;
	int term;
	bool success;
	uint lsn;
  int workerId;
  int rNodeId;
} response_append_entries;

// request_vote_rpc
typedef struct _request_vote_rpc {
	RPCKind rpcKind;
	int term;         // candidate's term
	int candidateId;
	int lastLogIndex;
	int lastLogTerm;
} request_vote_rpc;

typedef struct _response_request_vote {
	RPCKind rpcKind;
	int term;
	bool success;
} response_request_vote;

// request_location
typedef struct _request_location {
	RPCKind rpcKind;
} request_location;

// response_request_location
typedef struct _response_request_location {
	RPCKind rpcKind;
	char hostname[LEN_HOST];
	int port;
} response_request_location;

append_entries_rpc arpcByFields(int term,	int leaderId,	int prevLogIndex,	int prevLogTerm, int leaderCommit, trans_req log[MAX_GROUP_ENTRY], int szLog,int lsn, int workerId, int rNodeId);
//append_entries_rpc arpcByFields(int term,	int leaderId,	int prevLogIndex,	int prevLogTerm, int leaderCommit, entry groupEntry[MAX_GROUP_ENTRY], int group_size);
request_vote_rpc rrpcByFields(Raft* raft);
response_append_entries raeByFields(int term, bool success, uint lsn, int workerId, int rNodeId);
response_request_vote rrvByFields(int term, bool success);
request_location rlByFields();
response_request_location rrlByFields(const char hostname[LEN_HOST], int port);
vector<string> StringSplit(const string &str, char sep);
commit_message cmByFields(int commandId);

#endif
