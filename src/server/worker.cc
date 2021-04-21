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



#include "worker.h"
#include <unistd.h>


WorkerInfo::WorkerInfo(int workerId, vector<follower_info*> *followers) {
  this->workerId = workerId;
  this->followers = followers;
}

int WorkerInfo::getWorkerId(){
  return this->workerId;
}

vector<follower_info*>* WorkerInfo::getFollowers(){
  return this->followers;
}

follower_info* WorkerInfo::getFollowerInfoById(int rNodeId){
	follower_info* ret;
  for (follower_info* fi : *this->followers){ 
    if (fi->followerNodeId == rNodeId){
			ret = fi; break;
    }
  }

	return ret;
}

Log* WorkerInfo::getLog() {
	return this->log;
}

int WorkerInfo::getCommitIndex() {
	return this->commitIndex;
}


void WorkerInfo::setCommitIndex(int commitIndex){
	this->commitIndex = commitIndex;
}


void WorkerInfo::xactEnqueue(trans_req tx)
{
	_mtx_xactQ.lock();
	this->xactQ.push(tx);
	_mtx_xactQ.unlock();
}

// Which data type?
trans_req WorkerInfo::xactDequeue()
{
	trans_req tx;

	_mtx_xactQ.lock();
	if (this->xactQ.empty() == true) {
		tx.clientId = -1; // meaning INVALID
		_mtx_xactQ.unlock();
    usleep(10);
	}
	else {
		tx = this->xactQ.front();
		this->xactQ.pop();	
		_mtx_xactQ.unlock();
	}
	return tx;
}

task WorkerInfo::taskLoad(const int lsn)
{
	task t;
	t.lsn = -1;

	this->_mtx_taskList.lock();
	for (auto itr = taskList.begin(); itr != taskList.end(); itr++) {
		if (itr->lsn == lsn) {
			t = *itr;
			taskList.erase(itr); // delete
			break;
		}
	}
	this->_mtx_taskList.unlock();

	return t; // error with -1 (not found)
}

void WorkerInfo::taskStore(task t)
{
	this->_mtx_taskList.lock();
	taskList.push_back(t);
	this->_mtx_taskList.unlock();
}



