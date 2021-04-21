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
#include <fstream>
#include <sstream>
#include <random>

#include "constant.h"
#include "header.h"
#include "entry.h"
#include "functions.h"

//#define DATABASE_SIZE (1000*1000)

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::ofstream;
using std::vector;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Usage: createInput OUTPUT_FILENAME NUM_OF_TX" << endl;
		return 0;
	}

	char* filename = argv[1];
	int num = std::stoi(argv[2]);
	//float updateRatio = std::stof(argv[3]);

	cout << "Output file is \"" << filename << "\"." << endl;
	cout << "The number of lines is " << num << "." << endl;
	//cout << "The ratio of updates to whole commands is " << updateRatio * 100 << "%" << endl;

	// create the directory
	struct stat st;
	if (stat(filename, &st) != 0) {
	  cout << "Creating the Directory \"" << filename << "\" ..." << flush;
	  int rc = mymkdir(filename);
	  if (rc == 0) {
	    cout << "Success!" << endl;
	  } else {
	    cout << "Failed." << endl;
	  }
	} else {
	  cout << "The Directory \"" << filename << "\" already exists." << endl;
	}
	
	// open an out file stream
	ofstream ofs(filename);
	if (!ofs) {
		cerr << "File \"" << filename << "\" cannot be opened." << endl;
		exit(1);
	}
	
	// create input file
	cout << "creating..." << flush;
	std::random_device rnd;
	vector<char*> keyList;// = new vector<char[KEY_LENGTH]>;
	for (int i = 0; i < num; i++) {
	  int from_key = rand() / SIZ_DATABASE;
		int diff_val = rand() % 1000;
	  int to_key = rand() / SIZ_DATABASE;

		ofs << from_key << " " << to_key << " " << diff_val << endl;
		
	  /*
	  char commandKind = (keyList.empty() || float(rnd()) / RAND_MAX / 2 < updateRatio) ? UPDATE : READ;
	  if (commandKind == UPDATE) {
	    for (int j = 0; j < KEY_LENGTH-1; j++) {
	      key[j] = 'a' + rnd() % 26;
	    }
	    key[KEY_LENGTH-1] = '\0';
	    
	    char *str = (char*)malloc(KEY_LENGTH);
	    strcpy(str, key);
	    keyList.push_back(str);
	  } else {
	    int index = rnd() % keyList.size();
	    strcpy(key, keyList[index]);
	    }
	  
	  ofs << commandKind << COMMAND_DELIMITER << key << COMMAND_DELIMITER << i << endl;
	  */
	}
	cout << "done.\n";

	ofs.close();

	return 0;
}
