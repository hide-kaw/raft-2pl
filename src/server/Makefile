
T1 := raft

CC := g++
CFLAGS := -g -c -O0 -std=c++11 -I../include -Wall 
LDFLAGS := -lm -pthread 

S1 := clientnode.cc config.cc filehandler.cc functions.cc kvs.cc log.cc main.cc node.cc raft.cc raftnode.cc rpc.cc status.cc worker.cc
O1 := $(S1:%.cc=%.o)

all: $(T1) 

$(T1): $(O1)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.cc
	$(CC) $(CFLAGS) $<

run: all
	./raft ../../raft.conf

clean:
	rm -f *~ *.exe *.stackdump $(T1) *.o
