OBJS=main.o
LIBS=`pkg-config --libs sdl` -ljpeg -lpng
CFLAGS=-O2 -g
PROG=sdlview

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%o:%c
	gcc $(CFLAGS) $< -c

clean:
	rm -f $(PROG) $(OBJS)
