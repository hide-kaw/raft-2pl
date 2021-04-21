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



#include <stdio.h>
#include <iostream>
#include "constant.h"
#include "header.h"
#include "entry.h"
#include "log.h"
#include "functions.h"

/*
void fields2entry(entry* e, int term, int conn_id, const char command[COMMAND_STR_LENGTH]) {
	e->term = term;
	e->conn_id = conn_id;
	//memcpy(e->command, command, COMMAND_STR_LENGTH);
}
*/
/*
static void str2entry(entry* e, char str[ENTRY_STR_LENGTH]) {
	vector<string> vec = split(str, ENTRY_DELIMITER);
	e->term = stoi(vec[0]);
	e->conn_id = stoi(vec[1]);
	memcpy(e->command, vec[2].c_str(), COMMAND_STR_LENGTH);
}
*/

	/*
void entry2str(entry* e, char str[ENTRY_STR_LENGTH]) {
	sprintf(
		str,
		"%d%c%d%c%s",
		e->term,
		ENTRY_DELIMITER,
		e->conn_id,
		ENTRY_DELIMITER,
		e->command
	);
}
	*/
