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


#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include "rpckind.h"
#include "constant.h"

typedef struct _HEADER {
	RPCKind kind;
	uint size; // payload size
} HEADER;

// client_command
typedef struct _client_request {
	int from;
	int to;
	int diff; // size of money to be transferred
	//int client_id;
	int txId;
} client_request;

typedef struct _client_command {
	RPCKind rpcKind;
	int clientId;	
	int commandId;
	client_request req[MAX_CLI_REQ];
} client_command;

typedef struct _trans_req{
 	int clientId;
	int commandId;
	client_request req;
} trans_req;


#endif
