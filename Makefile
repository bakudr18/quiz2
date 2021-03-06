TARGET = list_merge_sort
CC ?= cc
CFLAGS ?= -Wall -O0 -g
OBJS := list_merge_sort.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(TARGET) *~
