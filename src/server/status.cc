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


#include <sys/stat.h>
#include <sys/types.h>
#include <string>

#include "debug.h"
#include "status.h"
#include "header.h"
#include "entry.h"
//#include "log.h"
//#include "state.h"
//#include "valueStored.h"
#include "functions.h"

using std::cout;
using std::endl;
using std::stoi;
using std::string;
using std::to_string;

uint
Status::initVotedFor(string filename)
{
	int votedFor = -1;

	// load the value
	char buf[BUFSIZ];
	FILE *fp = fopen(filename.c_str(), "r");
	if (fgets(buf, sizeof(buf), fp)) {
		votedFor = atoi(buf);
	}
	fclose(fp);

	return votedFor;
}

uint
Status::initCurrentTerm(string filename)
{
	/* 
	 * currentTerm:
	 *
	 * latest term server has seen (initialized to 0
	 * on first boot, increases monotonically)
	 * votedFor candidateId that received vote in current
	 */
	int currentTerm = 0;

	// load the value
	char buf[BUFSIZ];
	FILE *fp = fopen(filename.c_str(), "r");
	if (fgets(buf, sizeof(buf), fp)) {
		currentTerm = atoi(buf);
	}
	fclose(fp);

	return currentTerm;
}

Status::Status(string storageDirectoryName)
{
	this->storageDirectoryName = new string(storageDirectoryName);
	this->createDirectory();
	//this->log = new Log(storageDirectoryName);
	this->currentTerm = initCurrentTerm(storageDirectoryName + "currentTerm");
	this->votedFor = initVotedFor(storageDirectoryName + "votedFor");
	//this->commitIndex = -1;
	//this->lastApplied = -1;
	cout << this->currentTerm << endl;
	cout << this->votedFor << endl;
}

void Status::createDirectory() {
	// crate the directory if it DOES NOT exist
	struct stat st;
	if (stat(this->getStorageDirectoryName().c_str(), &st) != 0) {
		cout << "Creating the Directory \"" << this->getStorageDirectoryName() << "\"." << endl;
		cout << "***";
		int rc = mymkdir(this->getStorageDirectoryName().c_str());
		if (rc == 0) {
			cout << "Success!" << endl;
		} else {
			cout << "Failed" << endl;
		}
	} else {
		cout << "The Directory \"" << this->getStorageDirectoryName() << "\" already exists." << endl;
	}
}

string Status::getStorageDirectoryName() {
	return *this->storageDirectoryName;
}

/*
Log* Status::getLog() {
	return this->log;
}
*/

int Status::getVoteCount(const int index) {
	assert(index >= 0);
	//D((uint)this->votes.size());
/*
if (this->votes[index] > 2 ){
for (auto& [key, value] : this->votes){
cout << key << "," << value << " ";
}
cout << endl;
}
*/
	return this->votes[index];
}

int Status::addVoteCount(const int index)
{
  int count = -1;
	if (index < 0) D(index);
	assert(index >= 0);
	_mtx.lock();
  if (this->votes.find(index) == this->votes.end()) {
    this->votes[index] = 1;
  } else {
    this->votes[index]++;
  }
  count = this->votes[index];
	_mtx.unlock();
  return count;
}

/* void Status::addVoteCount(const int index) */
/* { */
/* 	if (index < 0) D(index); */
/* 	assert(index >= 0); */
/* 	_mtx.lock(); */
/*   if (this->votes.find(index) == this->votes.end()) { */
/*     this->votes[index] = 1; */
/*   } else { */
/*     this->votes[index]++; */
/*   } */
/* 	_mtx.unlock(); */
/* } */

State Status::getState() {
	return this->state;
}

void Status::setState(State state) {
	_mtx.lock();
	this->state = state;
	_mtx.unlock();
}

bool Status::isFollower() {
	return this->state == FOLLOWER;
}

bool Status::isCandidate() {
	return this->state == CANDIDATE;
}

bool Status::isLeader() {
	return this->state == LEADER;
}

void Status::becomeFollower() {
	_mtx.lock();
	this->state = FOLLOWER;
	_mtx.unlock();
}

void Status::becomeCandidate() {
	_mtx.lock();
	this->state = CANDIDATE;
	_mtx.unlock();
}

void Status::becomeLeader() {
	_mtx.lock();
	this->state = LEADER;
	_mtx.unlock();
}

int Status::getCurrentTerm() {
	return this->currentTerm;
}

void Status::incrementCurrentTerm() {
	this->currentTerm++;
}

int Status::getVotedFor() {
	return this->votedFor;
}

void Status::setVotedFor(int node_id) {
	this->votedFor = node_id;
}
int Status::getTimeoutTime() {
	return this->timeouttime;
}
void Status::setTimeoutTime(int timeouttime) {
	this->timeouttime = timeouttime;
}
