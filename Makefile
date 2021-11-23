
#
# Const
#

BIN_FILES=mfs_server mfs_client
CC=mpic++
CFLAGS=-w -g -O0 -Wall -I/usr/local/include/ -I./
LDFLAGS=-lpthread


#
# Rules
#

all: $(BIN_FILES)
.PHONY: all clean

mfs_client: mfs_client.o mfs_lib.o
	$(CC) $^  $(LDFLAGS)  -o $@

mfs_server: mfs_server.o mfs_lib.o
	$(CC) $^  $(LDFLAGS)  -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<	

clean:
	rm -f *.o $(BIN_FILES)
