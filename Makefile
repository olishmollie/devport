PROG = devport
CFLAGS += -O -Wall -pedantic -std=c11
OBJS = devport.o

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(PROG)

%.o: %.c
	$(CC) $(CFLAGS) -c $^

.PHONY: clean
clean:
	rm -f *.o $(PROG)
