OBJS=codescanner.o
CFLAGS=-O2
LIBS=-lzbar -lpng
LDFLAGS=
PROG=codescanner

all:$(OBJS)
	gcc -o $(PROG) $(LDFLAGS) $(LIBS) $(OBJS)

%o:%c
	gcc $(CFLAGS) $< -c

clean:
	rm -f $(OBJS) $(PROG)
