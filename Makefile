all: gifpaper

gifpaper: wallpaper.c gifpaper.c image.c slideshow.c gifdec.c gifpaper.h gifdec.h
	gcc -g -O2 -o gifpaper wallpaper.c gifpaper.c gifdec.c image.c slideshow.c -lm -lpng -lX11 -lImlib2 -lpthread

clean:
	rm -rf gifpaper *.o *.inc

install: gifpaper
	@install -Dm755 gifpaper $(DESTDIR)/usr/bin/gifpaper
