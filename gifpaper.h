#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <X11/Xfuncproto.h>
#include <X11/Intrinsic.h>	/* Xlib, Xutil, Xresource, Xfuncproto */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <dirent.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>

#include <Imlib2.h>

typedef struct Frame{
  Pixmap pmap;
  struct Frame *next;
} Frame;

typedef struct Gif{
  struct Frame *head;
} Gif;

//might not be needed
extern Window ipc_win;
extern Atom ipc_atom;

extern Display *disp;
extern Visual *vis;
extern Screen *scr;
extern Colormap cm;
extern int depth;
extern XContext xid_context;
extern Window root;

int load_image(Imlib_Image * im, char *filename);
Frame *load_images_to_list(void);
char *generate_filename(char *prefix, int idx);
int break_gif_into_images(char *filename);
int clear_image_dir(void);

_XFUNCPROTOBEGIN 
extern void init_x_and_imlib(void);
extern Pixmap generate_pmap(Imlib_Image im);
extern void set_background(Pixmap pmap_d1); 
_XFUNCPROTOEND

#endif
