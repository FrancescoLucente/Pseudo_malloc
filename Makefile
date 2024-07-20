# Use clang as the default compiler on macOS
CC=clang
CCOPTS=--std=gnu99 -Wall -D_LIST_DEBUG_ -target arm64-apple-macos11

AR=ar

OBJS=bit_map.o\
     buddy_allocator.o\
	 pseudo_malloc.o

HEADERS=bit_map.h buddy_allocator.h pseudo_malloc.h

LIBS=libbuddy.a

BINS=test_pseudo_malloc

.phony: clean all

all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

libbuddy.a: $(OBJS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)



test_pseudo_malloc: test_pseudo_malloc.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm


clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
