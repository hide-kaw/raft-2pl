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


#ifndef STATUS_H
#define STATUS_H

#include <mutex>
#include <vector>
#include <map>
#include <cassert>

using std::string;
using std::vector;
using std::map;

//class ValueStored;
//class Log;

enum State {
	FOLLOWER  = 0,
	CANDIDATE = 1,
	LEADER    = 2,
};


class Status {
private:
	std::mutex _mtx;

	string* storageDirectoryName;
	//Log* log;
  //vector<int> votes;
  map<int, int> votes; 

	State state;

	//	ValueStored* currentTerm; 
	//ValueStored* votedFor;
	// Latest term server has seen (initialized to 0 on first boot, increases monotonically)
	int currentTerm; // Latest term server has seen (initialized to 0 on first boot, increases monotonically)
	int votedFor;  // CandidateId that received vote in current term (or null if none)

	//  vector<uint> commitIndex;
	//int commitIndex;
	//int lastApplied;
  //
	int timeouttime;
	void createDirectory();

public:
	Status(string storageDirectoryName);

	string getStorageDirectoryName();

	uint initCurrentTerm(string filename);
	uint initVotedFor(string filename);
		
	//Log* getLog();
	int getVoteCount(const int index);
	//void addVoteCount(const int index);
  int addVoteCount(const int index);

	State getState();
	void  setState(State state);
	bool isFollower();
	bool isCandidate();
	bool isLeader();
	void becomeFollower();
	void becomeCandidate();
	void becomeLeader();

	int  getCurrentTerm();
	void incrementCurrentTerm();

	int  getVotedFor();
	void setVotedFor(int node_id);

	//int  getCommitIndex();
	//void setCommitIndex(int commitIndex);

	//int  getLastApplied(int lastApplied);

	int  getTimeoutTime();
	void setTimeoutTime(int timeouttime);
};

//#include "status.cc"
#endif //STATUS_H
