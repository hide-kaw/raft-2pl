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


#ifndef WORKER_H
#define WORKER_H

#include <vector>
#include <queue>
#include <mutex>

#include "log.h"
#include "entry.h"
#include "header.h"

using std::vector;
using std::queue;
using std::mutex;

class Log;

typedef struct _follower_info {
	int followerNodeId;
	int sockfd;
	int nextIndex;
	int sentIndex;
} follower_info;

typedef struct _task {
	int workerId;
	int lsn;             // log sequence number, required at committer
	int nlog;
	trans_req xactSet[MAX_GROUP_ENTRY]; // required at client
} task;


class WorkerInfo {
private:
	int workerId;
	Log* log;
	vector<follower_info*>* followers;
	int commitIndex;        // -1  // committer 排他制御
	int matchIndex;         // -1
	int lastApplied;        // -1
	mutex _mtx_xactQ;
	mutex _mtx_taskList;
	queue<trans_req> xactQ;
	vector<task> taskList;
public:
	WorkerInfo(int workerId, vector<follower_info*> *followers);
	int getWorkerId();
	vector<follower_info*>* getFollowers();
  follower_info* getFollowerInfoById(int rNodeId);
	Log* getLog();
	int getCommitIndex();
  void setCommitIndex(int commitIndex);
  int getNextIndex();
  void setNextIndex(int nextIndex);
	trans_req xactDequeue();
	void xactEnqueue(trans_req t);
	void taskStore(task t);
	task taskLoad(const int lsn);
};

#endif
