# gifpaper

This program runs .gif animations as wallpapers using X11. The code builds on how
feh loads background images.

The pixel map of each frame is cached and reused, which significantly lowers the
CPU and battery usage when compared to using something like feh to load each frame.

Uses ffmpeg to break gifs into frames. 

Usage: gifpaper thing.gif

Run make install to add gifpaper to your /usr/bin.

Special thanks to the feh team... I based my code off of their background wallpaper code.
