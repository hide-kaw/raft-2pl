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


#ifndef RPC_KIND_H
#define RPC_KIND_H
enum RPCKind {
	APPEND_ENTRIES             = 10,
	REQUEST_VOTE               = 11,
	RESPONSE_APPEND_ENTRIES    = 12,
	RESPONSE_REQUEST_VOTE      = 13,
	REQUEST_LOCATION           = 14,
	RESPONSE_REQUEST_LOCATION  = 15,
	CLIENT_COMMAND             = 16,
	COMMIT_MESSAGE             = 17,
	CLIENT_EXIT                = 18,
	BYE_FROM_RAFT              = 19,

  CONNECT_NOTIFIER           = 20,
};
#endif
