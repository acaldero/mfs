
#
# Const
#

BIN_FILES=mfs_server mfs_client
CC=mpicc
CFLAGS=-w -g -O0 -Wall -I/usr/local/include/
LDFLAGS=


#
# Rules
#

all: $(BIN_FILES)
.PHONY: all clean

mfs_client: mfs_client.o
	$(CC) $(LDFLAGS) $^  -o $@

mfs_server: mfs_server.o
	$(CC) $(LDFLAGS) $^  -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<	

clean:
	rm -f *.o $(BIN_FILES)
