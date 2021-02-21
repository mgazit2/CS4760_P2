#MAKEFILE
#MATAN GAZIT

CC = gcc
CFLAGS = -Wall -g -std=gnu99

EXE = master
OBJ = main.o child.o
OUT = $(EXE)

.SUFFIXES: .c .o
.PHONY: all clean

all: $(OUT)

$(EXE): $(OBJ)
	 $(CC) $(CFLAGS) -o  $@ $(OBJ)

.c.o:
	 $(CC) $(CFLAGS) -c $<

clean:
	 /bin/rm -rf *.o $(EXE)
