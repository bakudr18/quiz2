TARGET = list_merge_sort bitcpy
CC ?= cc
CFLAGS ?= -Wall -O0 -g
SORT_OBJS := list_merge_sort.o
BITCPY_OBJS := bitcpy.o

all: list_merge_sort

bitcpy: $(BITCPY_OBJS)
	$(CC) $(CFLAGS) $< -o $@

list_merge_sort: $(SORT_OBJS)
	$(CC) $(CFLAGS) $(SORT_OBJS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(TARGET) *~
