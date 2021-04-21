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


#include <cstdio>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;
int
main(void)
{
	vector<int> lock_list;

	lock_list.push_back(3);
	lock_list.push_back(4);
	lock_list.push_back(1);

	for (auto itr = lock_list.begin(); itr != lock_list.end(); itr++) cout << *itr;
	sort(lock_list.begin(), lock_list.end());
	for (auto itr = lock_list.begin(); itr != lock_list.end(); itr++) cout << *itr;

	return 0;
}
