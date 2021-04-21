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
#include <memory>
#include <thread>
#include <signal.h>
#include "header.h"
#include "raft.h"
#include "debug.h"

using std::cout;
using std::endl;
using std::thread;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;

void signal_handler( int signal_num ) {
	cout << "The interrupt signal is (" << signal_num << "). \n";
	// terminate program
	pthread_exit(NULL);
} 


int
main(int argc, char* argv[])
{
	if (argc < 2) {
		cout << "Please specify a config file name." << endl;
		return 0;
	}

	// Do not stop if I receive SIGPIPE signal
	signal(SIGPIPE, SIG_IGN);

	printf("==================================\n");
	printf("===                            ===\n");
	printf("===            RAFT            ===\n");
	printf("===                            ===\n");
	printf("==================================\n");
	
	if (auto raft = std::make_shared<Raft>(argv[1])) {
		auto receiveThread = thread([&raft]{ raft->receiver(); });
		sleep_for(milliseconds(1000));
		auto timerThread = thread([&raft]{ raft->transmitter(); });
		
		// Service thread
		receiveThread.join();

		// Continual monitoring thread
		timerThread.join();

		//inputThread.join();
	}

	return 0;
}
