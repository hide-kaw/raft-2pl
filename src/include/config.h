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



#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>

using std::string;
using std::vector;

typedef struct _node_conf {
	string* hostname;
	int port;
} node_conf;

class Config {
private:
	string *configFileName;
	string *storageDirectoryName;
	int nNodes;
	vector<node_conf* > nodes;

public:
	Config(char* configFileName);

	string getStorageDirectoryName();
	void setStorageDirectoryName(string storageDirectoryName);

	int getNumberOfNodes();
	void setNumberOfNodes(int nNodes);

	vector<node_conf*> getNodes();
	void setNodes(vector<node_conf*> nodes);
};

//#include "config.cc"
#endif //CONFIG_H
