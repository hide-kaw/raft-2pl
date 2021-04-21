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
#include <fstream>
#include <sstream>

#include "debug.h"
#include "config.h"
#include "header.h"
#include "entry.h"
#include "functions.h"

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;

Config::Config(char* configFileName)
{
	this->configFileName = new string(configFileName);
	cout << "Config file is \"" << *this->configFileName << "\"." << endl;
	// open an input file stream
	ifstream ifs(configFileName);
	if (!ifs) ERR;

	string str;
	// load a path to directory to save log, currentTerm and votedFor
	bool loaded = false;
	while (!loaded) {
		getline(ifs, str);
		if (!str.empty()) {
			this->setStorageDirectoryName(str);
			loaded = true;
		}
	}
	cout << "The directory to save log and so on is \"" << this->getStorageDirectoryName() << "\"." << endl;
	// load information of cluster nodes
	int cnt = 0;
	while (getline(ifs, str)) {
		if (!str.empty()) {
			vector<string> strs = split(str, ':');
			node_conf* h = (node_conf*)malloc(sizeof(node_conf));
			h->hostname = new string(strs[0]);
			h->port = stoi(strs[1]);
			this->nodes.push_back(h);
			cnt++;
		}
	}
	ifs.close();
	this->setNumberOfNodes(cnt);

	// output
	cout << "Config file is loaded." << endl;
	cout << "Number of nodes is " << this->getNumberOfNodes() << "." << endl;
	for (node_conf* n : this->nodes) {
		cout << "   " << *n->hostname << ":" << n->port << endl;
	}
}

string Config::getStorageDirectoryName()
{
	return *this->storageDirectoryName;
}

void Config::setStorageDirectoryName(string storageDirectoryName) {
	this->storageDirectoryName = new string(storageDirectoryName);
}

int Config::getNumberOfNodes() {
	return this->nNodes;
}
void Config::setNumberOfNodes(int nNodes) {
	this->nNodes = nNodes;
}

vector<node_conf*> Config::getNodes() {
	return this->nodes;
}
void Config::setNodes(vector<node_conf*> nodes) {
	this->nodes = nodes;
}
