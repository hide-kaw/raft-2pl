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



#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <iostream>

using std::ifstream;
using std::ofstream;
using std::string;

class FileHandler {
private:
	string _fileName;
	ifstream* _ifs;
	ofstream* _ofs;
	bool _append_mode;

public:
	FileHandler(string fileName);

	ifstream* getIFStream();
	ofstream* getOFStream(bool append_mode);
	void closeIFStream();
	void closeOFStream();
	string getFileName();
};

#endif //FILEHANDLER_H
