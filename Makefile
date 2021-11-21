
#
# Const
#

EXE=mfs_server mfs_client
MCC=mpicc
CFLAGS=-O2 -Wall


#
# Rules
#

all: $(EXE)

% : %.c
	$(MCC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXE)

