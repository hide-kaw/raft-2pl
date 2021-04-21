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
#include "clientnode.h"
#include "header.h"
#include "entry.h"
#include "debug.h"
#include "functions.h"


ClientNode::ClientNode(std::string* hostname, int port) : Node(hostname, port)
{
	this->lastLogIndex   = -1;
	this->commitIndex = -1;
	this->lastCommandId      = -1;
	this->committedCommandId = -1;
	this->needReadRequest = false;
	this->readRPCID = -1;
	this->recvSock = -1;
	this->sendSock = -1;
}

int ClientNode::getLastLogIndex()
{
	return this->lastLogIndex;
}

void ClientNode::setLastLogIndex(int lastLogIndex)
{
	this->lastLogIndex = lastLogIndex;
}

int ClientNode::getCommitIndex()
{
	return this->commitIndex;
}

void ClientNode::setCommitIndex(int commitIndex)
{
	this->commitIndex = commitIndex;
}

int ClientNode::getLastCommandId()
{
	return this->lastCommandId;
}

void ClientNode::setLastCommandId(int lastCommandId)
{
	this->lastCommandId = lastCommandId;
}

int ClientNode::getCommittedCommandId()
{
	return this->committedCommandId;
}

void ClientNode::setCommittedCommandId(int committedCommandId)
{
	this->committedCommandId = committedCommandId;
}

void ClientNode::setReadRPCID(int rpcid) {
	_mtx.lock();
	this->readRPCID = rpcid;
	_mtx.unlock();
}

int ClientNode::getReadRPCID()
{
	int ret;
	_mtx.lock();
	ret = this->readRPCID;
	_mtx.unlock();
	return ret;
}

void ClientNode::setNeedReadRequest(bool b)
{
	_mtx.lock();
	this->needReadRequest = b;
	_mtx.unlock();
}

bool ClientNode::getNeedReadRequest()
{
	bool ret;
	_mtx.lock();
	ret = this->needReadRequest;
	_mtx.unlock();
	return ret;
}

int ClientNode::getReadGrantsNum(int size) {
	int num = 1; // myself
	_mtx.lock();
	for (bool isGranted : this->readGrants) {
		if (isGranted) { num++; }
	}
	_mtx.unlock();

	return num;
}

void ClientNode::grant(int raftNodeId)
{
	_mtx.lock();
	readGrants[raftNodeId] = true;
	_mtx.unlock();
}

void ClientNode::resetReadGrants(int size)
{
	_mtx.lock();
	for (int i = 0; i < size; i++) {
		if ((uint)i < readGrants.size()) {
			readGrants[i] = false;
		} else {
			readGrants.push_back(0);
		}
	}
	_mtx.unlock();
}

int ClientNode::getRecvSock() {
	return this->recvSock;
}
void ClientNode::setRecvSock(int sock) {
	this->recvSock = sock;
}
int ClientNode::getSendSock() {
	return this->sendSock;
}
void ClientNode::setSendSock(int sock) {
	this->sendSock = sock;
}

void ClientNode::closeRecvSock()
{
	int ret = close(this->recvSock);
	if (ret<0) ERR;
}

void ClientNode::closeSendSock()
{
	int ret = close(this->sendSock);
	if (ret<0) ERR;
}


