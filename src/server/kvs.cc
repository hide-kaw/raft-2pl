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


#include "kvs.h"

#include <assert.h>
#include <unistd.h>

KVS::KVS() {
	for (int i = 0; i < SIZ_DATABASE; i++) {
		this->key[i] = i;
		this->value[i] = rand();
		pthread_mutex_init(&this->_lock[i], NULL);
	}
	pthread_mutex_init(&this->_lock_memo_index, NULL);

	// Construct Hash Table (open addressing)
	for (int i = 0; i < SIZ_HASH_BUCKET; i++) this->hash[i].key = INVALID;
	for (int i = 0; i < SIZ_DATABASE; i++) {
		int key = this->key[i];
		int bid = key % SIZ_HASH_BUCKET;
		while (this->hash[bid].key != INVALID) {
			bid += HASH_DELTA;
			if (bid >= SIZ_HASH_BUCKET) bid -= SIZ_HASH_BUCKET;
		}
		this->hash[bid].key = this->key[i];
		this->hash[bid].value = this->value[i];
	}
}

void
KVS::printMemo(void)
{
	for (auto itr = this->memo_index.begin(); itr != this->memo_index.end(); itr++) {
		D(itr->index);
		D(itr->clientID);
	}
}

MEMO
KVS::getLockMemo(const int index)
{
	MEMO m;
	m.clientID = INVALID; // Set illegal value;
	
	pthread_mutex_lock(&this->_lock_memo_index);
	for (auto itr = this->memo_index.begin(); itr != this->memo_index.end(); itr++) {
		if (itr->index == index) {
			m = *itr;
			memo_index.erase(itr);
			break;
		}
	}
	
	// Already unlocked by another thread. I have nothing to do here.
	pthread_mutex_unlock(&this->_lock_memo_index);
	return m;
}


//KVS::setLockIndex(const int lastLogIndex, const int from_key[MAX_CLI_REQ], const int to_key[MAX_CLI_REQ], const int clientID)
void
KVS::setLockMemo(const int lastLogIndex, const std::vector<int> lock_list, const int clientID)
{
	MEMO m;
	pthread_mutex_lock(&this->_lock_memo_index);
	// error check
	for (auto itr = this->memo_index.begin(); itr != this->memo_index.end(); itr++) {
		if (itr->index == lastLogIndex) ERR;
	}
	m.index = lastLogIndex;
	std::copy(lock_list.begin(), lock_list.end(), std::back_inserter(m.lock_list));
	m.clientID = clientID;
	this->memo_index.push_back(m);
	
	pthread_mutex_unlock(&this->_lock_memo_index);
}

int KVS::size() {
	//return this->mp.size();
	ERR;
	return 0;
}

/*
KVS::lock(const int key)
{
	assert(key >= 0 && key < SIZ_DATABASE);
	int ret;
	while (true) {
		ret = pthread_mutex_trylock(&this->_lock[key]);
		if (ret == EBUSY) {
			_mm_pause();
			continue;
		}
		else if (ret == EAGAIN) {
			perror("trylock too many (EAGAIN)"); ERR;
		}
		else if (ret != 0) ERR;
		else break;
	}
}
*/

bool
KVS::lock(const int key)
{
	assert(key >= 0 && key < SIZ_DATABASE);
	int ret;
	ret = pthread_mutex_trylock(&this->_lock[key]);
	if (ret == EBUSY) {
    usleep(10);
		return false;
	}
	else if (ret == EAGAIN) {
		perror("trylock too many (EAGAIN)"); ERR;
	}
	else if (ret != 0) ERR;
	return true;
}

void KVS::unlock(const int key)
{
	assert(key >= 0 && key < SIZ_DATABASE);
	int ret;
	ret = pthread_mutex_unlock(&this->_lock[key]);
	if (ret != 0) ERR;
}

int KVS::read(const int key)
{
	for (int i = 0; i < SIZ_DATABASE; i++) {
		if (key == this->key[i]) {
			return this->value[i];
		}
	}

	return 0;
}

/* fordebug
int KVS::index_read(const int key)
{
  int read_val;
  for (int i = 0; i < SIZ_DATABASE; i++) {
    if (key == this->key[i]) {
      read_val = this->value[i];
      break;
    }
  }
  int bid = key % SIZ_HASH_BUCKET;
  while (true) {
    if (this->hash[bid].key == key) break;
    bid += HASH_DELTA;
    if (bid >= SIZ_HASH_BUCKET) bid -= SIZ_HASH_BUCKET;
  }

  if(read_val != this->hash[bid].value){
    D(key);
    D(read_val);
    D(this->hash[bid].value);
    ERR;
  }
  return this->hash[bid].value;
}
*/

int KVS::index_read(const int key)
{
  int bid = key % SIZ_HASH_BUCKET;
  while (true) {
    if (this->hash[bid].key == key) break;
    bid += HASH_DELTA;
    if (bid >= SIZ_HASH_BUCKET) bid -= SIZ_HASH_BUCKET;
  }
  return this->hash[bid].value;
}

void KVS::update(const int key, const int value) {
  for (int i = 0; i < SIZ_DATABASE; i++) {
    if (key == this->key[i]) {
      this->value[i] = value;
    }
  }
}

void KVS::index_update(const int key, const int value)
{
  int bid = key % SIZ_HASH_BUCKET;
  while (true) {
    if (this->hash[bid].key == key) break;
    bid += HASH_DELTA;
    if (bid >= SIZ_HASH_BUCKET) bid -= SIZ_HASH_BUCKET;
  }
  this->hash[bid].value = value;
}

void KVS::get(const char key[LEN_KEY], char value[LEN_VALUE]) {
	//memcpy(value, this->mp[key].c_str(), LEN_VALUE);
}

void KVS::put(const char key[LEN_KEY], const char value[LEN_VALUE]) {
	//this->mp[string(key)] = string(value);
}

void KVS::del(const char key[LEN_KEY]) {
	//this->mp.erase(string(key));
}

void KVS::printAll() {
	/*
	cout << "---kvs---\n";
	for (auto x : this->mp) {
		cout << x.first << " => " << x.second << endl;
	}
	cout << "---kvs end---\n";
	*/
}
