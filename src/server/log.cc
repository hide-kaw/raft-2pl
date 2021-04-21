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



#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "constant.h"
#include "header.h"
#include <unistd.h>
#include <cassert>
#include "entry.h"
#include "log.h"
#include "rpc.h"
#include "debug.h"

using std::cout;
using std::endl;

Log::Log(string dirname, int workerId) : FileHandler(dirname + "log-" + std::to_string(workerId))
{

	cout << "The Log file is \"" << this->getFileName() << "\"." << endl;

	this->fd = open((this->getFileName()).c_str(), O_RDONLY);	if (fd == -1) ERR;
	while (true) {
		entry e;
		int n = read(this->fd, &e, sizeof(entry)); if (n == -1) ERR;
		if (n == 0) break; 
		this->_entry.push_back(e);
	}
	close(this->fd);
	this->lastSyncedIndex = this->lastLogIndex();
	this->fd = open(this->getFileName().c_str(), O_WRONLY|O_APPEND);
}

Log::~Log()
{
	int ret = close(fd); if (ret<0)ERR;
}

void Log::lockEntry() {
	_mutexEntry.lock();
}

void Log::unlockEntry() {
	_mutexEntry.unlock();
}

int Log::getSizeEntry() {
	int ret;

	this->lockEntry();
	ret = this->_entry.size();
	this->unlockEntry();

	return ret;
}

int Log::getMaxIndexOfEntry()
{
	int ret;

	ret = this->_entry.size() - 1;

	return ret;
}

int Log::lastLogIndex() {
	return this->_entry.size() - 1;
}

int Log::lastLogTerm() {
	return this->getTerm(this->lastLogIndex());
}

int Log::getTerm(int index) {
	if (index < 0)
		return -1;

	if (index > this->lastLogIndex())
		return -1;

	entry e = this->getEntry(index);
	return e.term;	
}

bool Log::match(int prevLogIndex, int prevTerm)
{
	if (prevLogIndex < 0)
		return true;
	if (prevTerm == this->getTerm(prevLogIndex))
		return true;

	return false;
}

int Log::getPrevTerm(int prevLogIndex)
{
	int prevTerm;

	if (prevLogIndex == -1)
		prevTerm = -1;
	else {
		entry e = this->getEntry(prevLogIndex);
		prevTerm = e.term;
	}
	
	return prevTerm;
}

entry Log::getEntry(int index)
{
	entry e = this->_entry[index];
	return e;
}

// Original code
void Log::addEntry(entry e)
{
	// Memory
	this->_entry.push_back(e);
	// Storage
	int ret;
	ret = write(this->fd, &e, sizeof(entry)); if (ret<0)ERR;
	ret = fsync(this->fd); if (ret<0)ERR;
}


int Log::getLastSyncedIndex() {
	return this->lastSyncedIndex;
}

/*
void Log::addCommand(const client_command cc) {
	this->lockCommandQ();
	this->_command.push(cc);
	this->unlockCommandQ();
}
*/

/*
void Log::sync()
{
	int fd = open(this->getFileName().c_str(), O_WRONLY|O_APPEND);	
	int lastIndex = this->lastLogIndex();
	for (int i = this->getLastSyncedIndex() + 1; i <= lastIndex; i++) {
		tries_rpc arpc = this->get(i);
		write(fd, &arpc, sizeof(arpc));
	}
	fsync(fd);
	close(fd);
	this->lastSyncedIndex = lastIndex;
}
*/

