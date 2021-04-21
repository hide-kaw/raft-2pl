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
#include "raftnode.h"
#include "debug.h"
#include <cassert>

RaftNode::RaftNode(std::string* hostname, int port) : Node(hostname, port) {
	this->votedForMe = false;
	this->rvrpcSent  = false;
	this->isme       = false;
}

bool RaftNode::isMe() {
	return this->isme;
}

void RaftNode::setIsMe(bool isme) {
	this->isme = isme;
}

bool RaftNode::hasVotedForMe() {
	return this->votedForMe;
}

void RaftNode::setVotedForMe(bool votedForMe) {
	this->votedForMe = votedForMe;
}

bool RaftNode::IhaveSentRequestVoteRPC() {
	return this->rvrpcSent;
}

void RaftNode::setRequestVoteRPCSent(bool rvrpcSent) {
	this->rvrpcSent = rvrpcSent;
}
