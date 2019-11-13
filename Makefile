LDFLAGS := -lm -lpng -lX11 -lImlib2 -lpthread
CFLAGS := -g -O2

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

xinerama ?= 1

ifeq (${xinerama},1)
	CFLAGS += -DHAVE_LIBXINERAMA
	LDFLAGS += -lXinerama
endif

all: gifpaper

gifpaper: $(OBJS)
	gcc -o gifpaper $^ $(LDFLAGS)

%.o: %.c gifpaper.h
	gcc -c $(CFLAGS) $< -o $@

clean:
	rm -rf gifpaper *.o *.inc

install: gifpaper
	@install -Dm755 gifpaper $(DESTDIR)/usr/bin/gifpaper
