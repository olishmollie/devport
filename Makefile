PROG = sio
CFLAGS += -O -Wall -pedantic -std=c11
OBJS = sio.o

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(PROG)

%.o: %.c
	$(CC) $(CFLAGS) -c $^

.PHONY: clean
clean:
	rm -f *.o $(PROG)
