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


#include <stdio.h>
#include <iostream>

#include "../src/raft/rpc.cc"
#include "../src/functions.cc"
#include "../src/kvs.h"

using std::cout;
using std::endl;

int main() {
	// ======= RPC TEST ========== //
	// append_entries_rpc
	append_entries_rpc* arpc = (append_entries_rpc*)malloc(sizeof(append_entries_rpc));
	char sarpc[MESSAGE_SIZE] = "0 1 2 3 4 5 put,x,2,put,y,3";
	//str2arpc(sarpc, arpc);
	arpcByFields(arpc, 1, 2, 3, 4, 5, "put,x,2,put,y,3");
	cout
		<< "-------" << endl
		<< STR(sarpc)              << " = \"" << sarpc << "\"" << endl
		<< STR(arpc->rpcKind)      << " = " << StrRPCKind(arpc->rpcKind) << endl
		<< STR(arpc->term)         << " = " << arpc->term                << endl
		<< STR(arpc->leaderId)     << " = " << arpc->leaderId            << endl
		<< STR(arpc->prevLogIndex) << " = " << arpc->prevLogIndex        << endl
		<< STR(arpc->prevLogTerm)  << " = " << arpc->prevLogTerm         << endl
		<< STR(arpc->leaderCommit) << " = " << arpc->leaderCommit        << endl
		<< STR(arpc->entries)      << " = " << arpc->entries             << endl;
	char sarpc2[MESSAGE_SIZE];
	arpc2str(arpc, sarpc2);
	cout << STR(sarpc2)	<< " = \"" << sarpc2 << "\"" << endl;
	free(arpc);

	// request_vote_rpc
	char srrpc[MESSAGE_SIZE] = "1 1 2 3 4";
	request_vote_rpc* rrpc = (request_vote_rpc*)malloc(sizeof(request_vote_rpc));
	//str2rrpc(srrpc, rrpc);
	rrpcByFields(rrpc, 1, 2, 3, 4);
	cout
		<< "-------" << endl
		<< STR(srrpc)              << " = \"" << srrpc << "\"" << endl
		<< STR(rrpc->rpcKind)      << " = " << StrRPCKind(rrpc->rpcKind) << endl
		<< STR(rrpc->term)         << " = " << rrpc->term                << endl
		<< STR(rrpc->candidateId)  << " = " << rrpc->candidateId         << endl
		<< STR(rrpc->lastLogIndex) << " = " << rrpc->lastLogIndex        << endl
		<< STR(rrpc->lastLogTerm)  << " = " << rrpc->lastLogTerm         << endl;
	char srrpc2[MESSAGE_SIZE];
	rrpc2str(rrpc, srrpc2);
	cout << STR(srrpc2)	<< " = \"" << srrpc2 << "\"" << endl;
	free(rrpc);

	// response_append_entries
	char srae[MESSAGE_SIZE] = "2 1 1";
	response_append_entries* rae = (response_append_entries*)malloc(sizeof(response_append_entries));
	//str2rae(srae, rae);
	raeByFields(rae, 1, 1);
	cout
		<< "-------" << endl
		<< STR(srae)         << " = \"" << srae << "\"" << endl
		<< STR(rae->rpcKind) << " = " << StrRPCKind(rae->rpcKind)   << endl
		<< STR(rae->term)    << " = " << rae->term                  << endl
		<< STR(rae->success) << " = " << rae->success               << endl;
	char srae2[MESSAGE_SIZE];
	rae2str(rae, srae2);
	cout << STR(srae2)	<< " = \"" << srae2 << "\"" << endl;
	free(rae);

	// response_request_vote
	char srrv[MESSAGE_SIZE] = "3 1 1";
	response_request_vote* rrv = (response_request_vote*)malloc(sizeof(response_request_vote));
	//str2rrv(srrv, rrv);
	rrvByFields(rrv, 1, 1);
	cout
		<< "-------" << endl
		<< STR(srrv)         << " = \"" << srrv << "\"" << endl
		<< STR(rrv->rpcKind) << " = " << StrRPCKind(rrv->rpcKind) << endl
		<< STR(rrv->term)    << " = " << rrv->term                << endl
		<< STR(rrv->success) << " = " << rrv->success             << endl;
	char srrv2[MESSAGE_SIZE];
	rrv2str(rrv, srrv2);
	cout << STR(srrv2)	<< " = \"" << srrv2 << "\"" << endl;
	free(rrv);

	cout
		<< "-------" << endl
		<< STR(discernRPC(sarpc)) << " = " << discernRPC(sarpc) << " = " << StrRPCKind(discernRPC(sarpc)) << endl
		<< STR(discernRPC(srrpc)) << " = " << discernRPC(srrpc) << " = " << StrRPCKind(discernRPC(srrpc)) << endl
		<< STR(discernRPC(srae))  << " = " << discernRPC(srae)  << " = " << StrRPCKind(discernRPC(srae))  << endl
		<< STR(discernRPC(srrv))  << " = " << discernRPC(srrv)  << " = " << StrRPCKind(discernRPC(srrv))  << endl;
	// ======= RPC TEST ========== //

	// === myrand() test ========= //
	int min = 100, max = 200;
	for (int i = 0; i < 10000; i++) {
		int v = myrand(min, max);
		if (v < min || max <= v) {
			cerr << "===== ERROR =====\n    myrand() does NOT work well\n===== ERROR =====" << endl;
		}
	}
	// === myrand() test ========= //

	// === split() test ========= //
	string spstr = "a:b:c";
	char delim = ':';
	vector<string> vec = split(spstr, delim);

	cout << "------\n";
	cout << "spstr = \"" << spstr << "\"\n";
	cout << "spstr = \'" << delim << "\'\n";
	for (int i = 0; i < vec.size(); i++) {
		cout << "vec[" << i << "] = " << vec[i] << endl;
	}
	// === split() test ========= //

	// === KVS test ====== //
	cout << "------\n";
	KVS* kvs = new KVS();
	kvs->put("a", "1");
	kvs->del("a");
	kvs->put("b", "1");
	kvs->put("b", "2");
	kvs->put("c", "3");

	cout << "kvs->size() = " << kvs->size() << endl;
	kvs->printAll();
	// === KVS test ====== //

	return 0;
}
