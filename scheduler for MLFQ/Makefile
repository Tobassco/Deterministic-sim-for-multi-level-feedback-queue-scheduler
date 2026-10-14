CC     = gcc
CFLAGS = -Wall -std=c11
SRCS   = main.c parse.c process.c queue.c scheduler.c output.c
OBJS   = $(SRCS:.c=.o)
HDRS   = parse.h process.h queue.h scheduler.h output.h

schedule: $(OBJS)
	$(CC) $(CFLAGS) -o schedule $(OBJS)

$(OBJS): $(HDRS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f $(OBJS) schedule