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


#ifndef STATE_H
#define STATE_H

#define STR(var) #var

enum State {
	FOLLOWER  = 0,
	CANDIDATE = 1,
	LEADER    = 2,
};

struct StrState : public std::string {
	StrState(State e) {
		switch(e) {
			break; case FOLLOWER  : { assign("FOLLOWER"     ); }
			break; case CANDIDATE : { assign("CANDIDATE"    ); }
			break; case LEADER    : { assign("LEADER"       ); }
			break; default        : { assign("illegal state"); }
		}
	}
};

#endif 
