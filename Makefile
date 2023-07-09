CC=gcc
CFLAGS = -I.
CFLAGS += `pkg-config --cflags gtk+-3.0 gtksourceview-3.0`
LDFLAGS += `pkg-config --libs gtk+-3.0 gtksourceview-3.0`
SOURCES = main.c
OBJS = $(patsubst %.c, %.o, $(SOURCES))

lemon.exe: $(OBJS)
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
	
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)


clean:
	rm -f *.o lemon.exe