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


#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

class Node {
private:

	//std::mutex _mtx;

	int id;
	std::string* hostname;
	int listenPort;

	//int recvSock;
	//int sendSock;

	//pthread_t* worker;

public:
	Node(std::string* hostname, int port);

	int getID();
	void setID(int id);

	bool _send(char* message, int length);

	std::string getHostname();
	void setHostname(std::string hostname);


	int getListenPort();
	void setListenPort(int listenPort);

  /*
	pthread_t* getWorker();
	void setWorker(pthread_t* worker);
  */
};

#endif

