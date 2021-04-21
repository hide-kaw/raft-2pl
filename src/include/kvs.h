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


#ifndef KVS_H
#define KVS_H

#include <map>
#include <list>
#include <vector>
#include "debug.h"
#include <xmmintrin.h>
#include "constant.h"

using std::map;

typedef struct _MEMO {
	int index;
	std::vector<int> lock_list;
	int clientID;
} MEMO;

typedef struct _HASH_OBJECT {
	int key;
	int value;
} HASH_OBJECT;

class KVS {
private:
	//LOCK_OBJECT _lkobj[WORKER_NUM];
	HASH_OBJECT hash[SIZ_HASH_BUCKET];	
	int key[SIZ_DATABASE];
	int value[SIZ_DATABASE];
	pthread_mutex_t _lock[SIZ_DATABASE];
	std::list<MEMO> memo_index;
	pthread_mutex_t _lock_memo_index;
	
public:
	KVS();

	int size();

	void printMemo(void);
	bool lock(const int key);
	void unlock(const int key);
	void update(const int key, const int value);
	void index_update(const int key, const int value);
	int read(const int key);
	int index_read(const int key);
	
	void setLockMemo(const int lastLogIndex, std::vector<int> lock_list, const int clientID);	
	MEMO getLockMemo(const int index);

	void get(const char key[LEN_KEY], char value[LEN_VALUE]);
	void put(const char key[LEN_KEY], const char value[LEN_VALUE]);
	void del(const char key[LEN_KEY]);

	void printAll();
};

#endif //KVS_H
