CC=gcc
CFLAGS=-I.
DEPS = eventQueue.h
OBJ = eventQueue.o discontinuityTest.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

discTest.exe: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
