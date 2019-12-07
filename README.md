# GIFPAPER

## About

[Gifpaper] is a lightweight utility to draw gif frames to the X root window (i.e. *GIF wallpapers*).

Some of [Gifpaper]'s features include:

* drawing gifs onto the X root window
* playing a slideshow of gifs stored in a single directory
* cropping the gif before it gets displayed
* multihead support which replicates the gif on each monitor
* multihead support which scales and extends the gif over all monitors
* power saving mode which halts the gif if the battery is discharging

![Example-wallpaper](https://i.imgur.com/ZyL7hdG.mp4)

### Why [Gifpaper]?

[Gifpaper] is designed to minimize the performance cost of loading each frame to the root window. This
is primarily done by cacheing each frame of the gif as a Pixmap, so that at runtime the pixmap need only
be drawn to the background. The draw operation itself has also been optimized to use as few Xlib calls
as possible. [Gifpaper] is a **lightweight**, **performant**, and **customizable** utlility to create
GIF wallpapers. 

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