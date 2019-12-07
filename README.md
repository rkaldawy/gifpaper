# GIFPAPER

##About

This program runs .gif animations as wallpapers using X11. The code builds on how
feh loads background images.

The pixel map of each frame is cached and reused, which significantly lowers the
CPU and battery usage when compared to using something like feh to load each frame.

Uses ffmpeg to break gifs into frames. 

Now supports seamless slideshow mode! Just add -s flag, and indicate the rate at which gifs should be swapped.

Usage: gifpaper -f framerate thing.gif

gifpaper -f framerate -s sliderate directory/


Run make install to add gifpaper to your /usr/bin.

Special thanks to the feh team... I based my code off of their background wallpaper code.

NOTICE: Running compton's default backend with gifpaper WILL CAUSE GPU MEMORY TO LEAK.
To fix this, run instead compton --backend xrender ...