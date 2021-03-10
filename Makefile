TARGET = list_merge_sort bitcpy bench_bitcpy
CC ?= cc
CFLAGS ?= -Wall -O3 -g
SORT_OBJS := list_merge_sort.o
BITCPY_OBJS := bitcpy.o
BITCPY_TEST_OBJS := test_bitcpy.o
BITCPY_BENCH_OBJS := bench_bitcpy.o

all: list_merge_sort

bitcpy: $(BITCPY_OBJS) $(BITCPY_TEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bench_bitcpy: $(BITCPY_OBJS) $(BITCPY_BENCH_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

list_merge_sort: $(SORT_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(TARGET) *~
