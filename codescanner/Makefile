OBJS=codescanner.o
CFLAGS=-O2 -g
LIBS=-lzbar -lpng -ljpeg
LDFLAGS=
PROG=codescanner

all:$(OBJS)
	gcc -o $(PROG) $(LDFLAGS) $(LIBS) $(OBJS)

%o:%c
	gcc $(CFLAGS) $< -c

clean:
	rm -f $(OBJS) $(PROG)
