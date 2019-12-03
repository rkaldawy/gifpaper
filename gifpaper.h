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
#ifdef HAVE_LIBXINERAMA
#include <X11/X.h>
#include <X11/extensions/Xinerama.h>
#endif /* HAVE_LIBXINERAMA */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
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

#include "gifdec.h"
#include <Imlib2.h>

#define XY_IN_RECT(x, y, rx, ry, rw, rh)                                       \
  (((x) >= (rx)) && ((y) >= (ry)) && ((x) < ((rx) + (rw))) &&                  \
   ((y) < ((ry) + (rh))))

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

extern XineramaScreenInfo *xinerama_screens;
extern int xinerama_screen;
extern int num_xinerama_screens;

extern int needs_crop;
extern int crop_params[];
extern int display_mode;

#define DISPLAY_MODE_REPLICATE 1
#define DISPLAY_MODE_EXTEND 2

int load_image(Imlib_Image *im, char *filename);

Frame *append_image_to_list(gd_GIF *gif, Frame *c);
Frame *load_images_to_list(char *gifpath);

char *generate_filename(char *prefix, int idx);
int break_gif_into_images(char *filename);
int clear_image_dir(void);
void *slideshow_gif_thread(void *args);
SlideshowEntry *load_slideshow_paths(char *gifpath);
void clean_gif_frames(Frame *head);

int display_as_gif(char *gifpath, long framerate);
int display_as_slideshow(char *dirpath, long framerate, long sliderate);

uint8_t *scale_to_screen(unsigned char *src, int srcWidth, int srcX, int srcY,
                         int srcW, int srcH, int i);
void scale(unsigned char *dst, int dstWidth, int dstX, int dstY, int dstW,
           int dstH, unsigned char *src, int srcWidth, int srcX, int srcY,
           int srcW, int srcH);
int count_frames_in_gif(char *gifpath);

_XFUNCPROTOBEGIN
extern void init_x_and_imlib(void);
extern void init_xinerama(void);

Imlib_Image crop_image(Imlib_Image im, int x, int y, int w, int h);

extern Pixmap generate_pmap(gd_GIF *gif, uint8_t *buffer);
extern Pixmap generate_pmap_replicate(gd_GIF *gif, uint8_t *buffer);
extern Pixmap generate_pmap_extend(Imlib_Image im);

Pixmap _generate_pmap(Pixmap pmap, uint8_t *buffer, int x, int y, int w, int h);
void clear_pmap(Pixmap pmap);

extern int set_background(Frame *frame);
extern int _set_background(Frame *frame, Frame *prev);

_XFUNCPROTOEND

#endif
