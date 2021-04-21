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


#include "node.h"
#include "header.h"
#include "entry.h"
#include "debug.h"
#include "functions.h"

Node::Node(std::string* hostname, int listenPort) {
	this->setHostname(*hostname);
	this->setListenPort(listenPort);
}

int Node::getID() {
	return this->id;
}
void Node::setID(int id) {
	this->id = id;
}

bool
Node::_send(char* message, int length) {
/*
	//cout << "send message to " << this->getHostname() << " [" << message << "] sock=" << sendSock << endl;
	bool ok = true;

//	_mtx.lock();
	if (S_SEND(this->getSendSock(), message, length, 0) < 0) {
		perror("write");
		this->sendSock = -1;
		std::cout << "   node is " << this->getHostname() << std::endl;
		ok = false;
	}
//	_mtx.unlock();

	return ok;
*/
}

std::string Node::getHostname() {
	return *this->hostname;
}

void Node::setHostname(std::string hostname) {
	this->hostname = new std::string(hostname);
}

int Node::getListenPort() {
	return this->listenPort;
}

void Node::setListenPort(int listenPort) {
	this->listenPort = listenPort;
}
/*
pthread_t* Node::getWorker() {
	return this->worker;
}
void Node::setWorker(pthread_t* worker) {
	this->worker = worker;
}
*/
