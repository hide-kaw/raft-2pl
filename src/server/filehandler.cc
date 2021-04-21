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



#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <string>

#include "fileHandler.h"
#include "debug.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;


FileHandler::FileHandler(string fileName) {
	this->_fileName = fileName;
	this->_ifs = NULL;
	this->_ofs = NULL;
}

ifstream* FileHandler::getIFStream() {
	this->_ifs = new ifstream(this->getFileName().c_str());
	return this->_ifs;
}

ofstream* FileHandler::getOFStream(bool append_mode) {
	if (!this->_ofs || this->_append_mode != append_mode) {
		if (this->_ofs) {
			this->closeOFStream();
		}
		if (append_mode) {
			this->_ofs = new ofstream(this->getFileName().c_str(), ios::app);
		} else {
			this->_ofs = new ofstream(this->getFileName().c_str());
		}
	}
	this->_append_mode = append_mode;

	return this->_ofs;
}

void FileHandler::closeIFStream()
{
	this->_ifs->close();
}

void FileHandler::closeOFStream()
{
	this->_ofs->close();
}

string FileHandler::getFileName()
{
	return this->_fileName;
}
