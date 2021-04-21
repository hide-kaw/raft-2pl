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


#ifndef CONSTANT_H
#define CONSTANT_H
#include "for_distance_delay.h"

// hostname
#define LEN_HOST                       (128)
#define LEN_PORT                       (6)

// RPC's message length related
#define MAX_GROUP_ENTRY                (100)
#define MAX_CLUSTER_NUM                (7)

// Key-Value Store
#define LEN_KEY                        (8)
#define LEN_VALUE                      (8)
#define SIZ_DATABASE                   (1000 * 1000 * 10)
#define SIZ_HASH_BUCKET                (SIZ_DATABASE * 10)
#define HASH_DELTA                     (13)

// timeout related
#define MIN_TIMEOUTTIME_MICROSECONDS   (1000000)  // 1     sec
#define MAX_TIMEOUTTIME_MICROSECONDS   (1500000)  // 1.5  sec
#define LOG_TIMEOUT                    (1000 * 100) // 100 msec
 
// append entries rpc related
#define HEARTBEAT_INTERVAL             (50000) // 0.05 sec
#define GROUP_TRANSFER_INTERVAL        (0)
#define RPC_ID_MAX                     (1000000000)
#define CLIENTS_MAX                    (1000)

// measure
#define MEASURE_LOG_SIZE               (1000)

// storage latency
#define STORAGE_LATENCY                (8)

// Max number of client request sent in a batch from client to leader
#define MAX_CLI_REQ (100)

#define WORKER_NUM (9)
#define COMMITTER_NUM (1)


// Invaild value for KVS, which is used for initialization
#define INVALID (-100)

#endif //CONSTANT_H
