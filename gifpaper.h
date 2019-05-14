#ifndef WALLPAPER_H
#define WALLPAPER_H

#define _GNU_SOURCE

#include <X11/Intrinsic.h> /* Xlib, Xutil, Xresource, Xfuncproto */
#include <X11/Xatom.h>
#include <X11/Xfuncproto.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <pwd.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#include <Imlib2.h>

typedef struct Frame {
  Pixmap pmap;
  struct Frame *next;
} Frame;

typedef struct Gif {
  struct Frame *head;
} Gif;

typedef struct SlideshowEntry {
  char path[200];
  struct SlideshowEntry *next;
} SlideshowEntry;

// might not be needed
extern Window ipc_win;
extern Atom ipc_atom;

extern Display *disp;
extern Visual *vis;
extern Screen *scr;
extern Colormap cm;
extern int depth;
extern XContext xid_context;
extern Window root;

int load_image(Imlib_Image *im, char *filename);
Frame *load_image_to_list(Frame *c, int frame_num);
Frame *load_images_to_list(void);
int count_frames_in_gif(void);
char *generate_filename(char *prefix, int idx);
int break_gif_into_images(char *filename);
int clear_image_dir(void);
void *slideshow_gif_thread(void *args);
SlideshowEntry *load_slideshow_paths(char *gifpath);

int display_as_gif(char *gifpath, long framerate);
int display_as_slideshow(char *dirpath, long framerate);

_XFUNCPROTOBEGIN
extern void init_x_and_imlib(void);
extern Pixmap generate_pmap(Imlib_Image im);
extern void set_background(Pixmap pmap_d1);
_XFUNCPROTOEND

#endif
