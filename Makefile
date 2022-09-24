CC := gcc
CFLAGS := -Wall -Wpedantic
OBJS := key_util.o options.o config.o

skeylogger: src/skeylogger.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: src/%.c src/include/%.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o skeylogger
