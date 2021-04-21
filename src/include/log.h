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


#ifndef LOG_H
#define LOG_H

#include <string>
#include <vector>
#include <mutex>
#include <queue>
#include "fileHandler.h"
#include "rpc.h"
#include "entry.h"
#include "status.h"

using std::string;
using std::vector;
using std::mutex;
using std::queue;

class Log : public FileHandler {
private:
	mutex _mutexEntry;
	vector<entry> _entry;
	//queue<client_command> _command;
	int lastSyncedIndex;

public:
	Log(string storageDirectoryName, int workerId);
  ~Log();

  int fd;
	int getSizeEntry();
	int getMaxIndexOfEntry();
	int lastLogIndex();
	int lastLogTerm();
	int getTerm(int index);
	int getPrevTerm(int prevLogIndex);
	int getConnectionId(int index);
	bool match(int index, int term);
	entry getEntry(int index);
	// void addEntryLeader(append_entries_rpc arpc);
	//void addEntryFollower(entry groupEntry[], uint szGroup);
	void addEntry(entry e);
	void lockEntry();
	void unlockEntry();
	int getLastSyncedIndex();
	void sync();
	void printAll();
};

#endif //LOG_H
