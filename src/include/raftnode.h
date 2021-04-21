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


#ifndef RAFT_NODE_H
#define RAFT_NODE_H

class RaftNode : public Node {
private:
//	int nextIndex;  // found in paper
	//int matchIndex; // found in paper
	//int sentIndex;  // sent to follower
	bool isme;
	bool votedForMe;
	bool rvrpcSent;     // requestVoteRPC

public:
	RaftNode(std::string* hostname, int port);

  /*
	int  getNextIndex();
	void setNextIndex(int nextIndex);

	int  getMatchIndex();
	void setMatchIndex(int matchIndex);

	int  getSentIndex();
	void setSentIndex(int sentIndex);
	void addSentIndex(int szGroup);
  */
	
	bool isMe();
	void setIsMe(bool isme);

	bool hasVotedForMe();
	void setVotedForMe(bool votedForMe);

	bool IhaveSentRequestVoteRPC();
	void setRequestVoteRPCSent(bool rvrpcSent);
};

#endif
