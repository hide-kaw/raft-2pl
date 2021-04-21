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


#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <random>
#include <sys/types.h>
#include <sys/socket.h>

/* === rsocket === */
#ifdef ENABLE_RSOCKET
#include <rdma/rsocket.h>

// rsocket functions
#define S_SOCKET(f, t, p)            rsocket(f, t, p)
#define S_CLOSE(f)                   rclose(f)
#define S_SEND(s, b, l, f)           rsend(s, b, l, f)
#define S_RECV(s, b, l, f)           rrecv(s, b, l, f)
#define S_ADDRINFO                   rdma_addrinfo
#define S_GETADDRINFO(a, p, h, r)    rdma_getaddrinfo(a, p, h, r)
#define S_FREEADDRINFO(r)            rdma_freeaddrinfo(r)
#define S_SETSOCKOPT(s, l, n, v, ol) rsetsockopt(s, l, n, v, ol)

// server-specific
#define S_ACCEPT(s, a, l)       raccept(s, a, l)
#define S_BIND(s, a, l)         rbind(s, a, l)
#define S_GETSOCKNAME(s, n, l)  rgetsockname(s, n, l)
#define S_LISTEN(s, b)          rlisten(s, b)
#define S_SRC_ADDR(a)           a->ai_src_addr
#define S_SRC_ADDRLEN(a)        a->ai_src_len
// client-specific
#define S_CONNECT(s, a, l)           rconnect(s, a, l)
#define S_DST_ADDR(a)                a->ai_dst_addr
#define S_DST_ADDRLEN(a)             a->ai_dst_len

#else

// BSD-socket functions
//#define S_SOCKET(f, t, p)            ::socket(f, t, p)
#define S_SOCKET(f, t, p)            socket(f, t, p)
#define S_CLOSE(f)                   close(f)
#define S_SEND(s, b, l, f)           send(s, b, l, f)
#define S_RECV(s, b, l, f)           recv(s, b, l, f)
#define S_ADDRINFO                   addrinfo
#define S_GETADDRINFO(a, p, h, r)    getaddrinfo(a, p, h, r)
#define S_FREEADDRINFO(r)            freeaddrinfo(r)
#define S_SETSOCKOPT(s, l, n, v, ol) setsockopt(s, l, n, v, ol)

// server-specific
#define S_ACCEPT(s, a, l)       accept(s, a, l)
#define S_BIND(s, a, l)         bind(s, a, l)
#define S_GETSOCKNAME(s, n, l)  getsockname(s, n, l)
#define S_LISTEN(s, b)          listen(s, b)
#define S_SRC_ADDR(a)           a->ai_addr
#define S_SRC_ADDRLEN(a)        a->ai_addrlen
// client-specific
#define S_CONNECT(s, a, l)           connect(s, a, l)
#define S_DST_ADDR(a)                a->ai_addr
#define S_DST_ADDRLEN(a)             a->ai_addrlen

#endif // ENABLE_RSOCKET
/* === rsocket === */

#define STR(var) #var

int mymkdir(const char *filepath);
int myrand(int min, int max);
std::vector<std::string> split(const std::string &s, char delim);
void printRPC(uint line, RPCKind kind);

#endif
