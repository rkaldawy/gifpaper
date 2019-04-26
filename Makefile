all: gifpaper

gifpaper: wallpaper.c gifpaper.c image.c gifpaper.h
	gcc -g -O2 -o gifpaper wallpaper.c gifpaper.c image.c -lm -lpng -lX11 -lImlib2

clean:
	rm -rf gifpaper *.o *.inc

install: gifpaper
	@install -Dm755 gifpaper $(DESTDIR)/usr/bin/gifpaper
