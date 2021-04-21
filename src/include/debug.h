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


#ifndef DEBUG_H
#define DEBUG_H

#define C(val)  do {fprintf(stderr, "%ld %16s %4d %16s %16s: %c\n", (long int)pthread_self(), __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define D(val)  do {fprintf(stderr, "%ld %16s %4d %16s %16s: %d\n", (long int)pthread_self(), __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define P(val)  do {fprintf(stderr, "%ld %16s %4d %16s %16s: %p\n", (long int)pthread_self(), __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define L(val)  do {fprintf(stderr, "%ld %16s %4d %16s %16s: %ld\n", (long int)pthread_self(), __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define LL(val) do {fprintf(stderr, "%ld %16s %4d %16s %16s: %lld\n", (long int)pthread_self(), __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define S(val)  do {fprintf(stderr, "%ld %16s %4d %16s %16s: %s\n", (long int)pthread_self(), __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define F(val)  do {fprintf(stderr, "%ld %16s %4d %16s %16s: %f\n", (long int)pthread_self(), __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define N       do {fprintf(stderr, "%ld %16s %4d %16s\n", (long int)pthread_self(), __FILE__, __LINE__, __func__); fflush(stderr);} while (0)
#define ERR     do {perror("ERROR"); N; exit(1);} while (0)
#define ERR2    do {perror("ERROR"); N; pthread_exit(NULL);} while (0)

#endif
