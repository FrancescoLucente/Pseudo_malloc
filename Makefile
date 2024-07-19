# Use clang as the default compiler on macOS
CC=clang
CCOPTS=--std=gnu99 -Wall -D_LIST_DEBUG_ -target arm64-apple-macos11

AR=ar

OBJS=pool_allocator.o\
     linked_list.o\
     bit_map.o\
     buddy_allocator.o\
	 pseudo_malloc.o

HEADERS=linked_list.h  pool_allocator.h bit_map.h buddy_allocator.h pseudo_malloc.h

LIBS=libbuddy.a

BINS=pool_allocator_test buddy_test buddy_allocator_test test_pseudo_malloc

.phony: clean all

all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

libbuddy.a: $(OBJS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)

pool_allocator_test: pool_allocator_test.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ 

buddy_test: buddy_test.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm

buddy_allocator_test: buddy_allocator_test.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm

test_pseudo_malloc: test_pseudo_malloc.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm


clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
