#MAKEFILE
#MATAN GAZIT

CC = gcc
CFLAGS = -Wall -g -std=gnu99

MSTR = master
MOBJ = main.o
MOUT = $(MSTR)

SLVE = slave
SOBJ = child.o
SOUT = $(SLVE)

.SUFFIXES: .c .o
.PHONY: all clean

all: $(MOUT) $(SOUT)

$(MSTR): $(MOBJ)
	 $(CC) $(CFLAGS) -o  $@ $(MOBJ)

$(SLVE): $(SOBJ)
	 $(CC) $(CFLAGS) -o $@ $(SOBJ)

.c.o:
	 $(CC) $(CFLAGS) -c $<

clean:
	 /bin/rm -rf *.o output $(MSTR) $(SLVE)
