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


#include "header.h"
#include "debug.h"
#include "entry.h"
#include "functions.h"
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::random_device;
using std::stringstream;

// devide a std::string by a certain letter
vector<std::string> split(const string &s, char delim) {
	vector<std::string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		if (!item.empty()) {
			elems.push_back(item);
		}
	}
	return elems;
}

void
printRPC(uint line, RPCKind kind)
{
	printf("line=%d\t", line);
	switch (kind) {
		case APPEND_ENTRIES:	cout << "APPEND_ENTRIES" << endl; break;
		case REQUEST_VOTE:   cout << "REQUEST_VOTE"   << endl; break;
		case RESPONSE_APPEND_ENTRIES: cout << "RESPONSE_APPEND_ENTRIES" << endl; break;
		case RESPONSE_REQUEST_VOTE: cout << "RESPONSE_REQUEST_VOTE" << endl; break;
		case REQUEST_LOCATION: cout << "REQUEST_LOCATION" << endl; break;
		case RESPONSE_REQUEST_LOCATION: cout << "RESPONSE_REQUEST_LOCATION" << endl; break;
		case CLIENT_COMMAND: cout << "CLIENT_COMMAND" << endl; break;
		case COMMIT_MESSAGE: cout << "COMMIT_MESSAGE" << endl; break;
		default: cout << "No such RPC: " << kind << endl; ERR; break; 
	}
}

// mkdir
static int stat_mkdir(const char *filepath, mode_t mode) {
	struct stat sb = {0};
	int rc = 0;

	rc = stat(filepath, &sb);
	if (rc == 0) {
		if (!S_ISDIR(sb.st_mode)) {
			cout << "Error: Not a directory: " << *filepath << std::endl;
			return -1;
		}
		return 0;
	}

	rc = mkdir(filepath, mode);
	if (rc < 0) {
		cout << "Error: mkdir(: " << errno << ") " << strerror(errno) << ": " << *filepath << endl;
		return -1;
	}

	return 0;
}

static int mkdir_path(const char *filepath, mode_t mode) {
	char *p = NULL;
	char *buf = NULL;
	int rc = 0;

	buf = (char *)malloc(strlen(filepath) + 4);
	if (buf == NULL) {
		cout << "Error: malloc(" << errno << ") " << strerror(errno) << endl;
		return -1;
	}
	strcpy(buf, filepath);

	for (p = strchr(buf+1, '/'); p; p = strchr(p+1, '/')) {
		*p = '\0';
		rc = stat_mkdir(buf, mode);
		if (rc != 0) {
			free(buf);
			return -1;
		}
		*p = '/';
	}

	free(buf);
	return 0;
}

int mymkdir(const char *filepath) {
	int rc = mkdir_path(filepath, 0755);
	return rc;
}

int myrand(int min, int max) {
	//std::random_device rnd;
	random_device rnd;
	return rnd() % (max - min) + min;
}
