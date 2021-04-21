T1 := client
T2 := createInput

CC := g++
CFLAGS := -g -c -Wall -O0 -std=c++11 -I../include
LDFLAGS := -lm -pthread 

S1 := client.cc ../server/functions.cc #../server/rpc.cc
O1 := $(S1:%.cc=%.o)

S2 := createInput.cc ../server/functions.cc
O2 := $(S2:%.cc=%.o)

all: $(T1) $(T2)

$(T1): $(O1)
	$(CC) $(LDFLAGS) -o $@ $^

$(T2): $(O2)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.cc
	$(CC) $(CFLAGS) $<

clean:
	rm -f *~ *.exe *.stackdump $(T1) $(T2) *.o
