/* wallpaper.c

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "gifpaper.h"

Display *disp = NULL;
Visual *vis = NULL;
Screen *scr = NULL;
Colormap cm;
int depth;
XContext xid_context = 0;
Window root = 0;

void init_x_and_imlib(void) {

  // XInitThreads(); // must always be the first call

  disp = XOpenDisplay(NULL);
  if (!disp)
    return;
  vis = DefaultVisual(disp, DefaultScreen(disp));
  depth = DefaultDepth(disp, DefaultScreen(disp));
  cm = DefaultColormap(disp, DefaultScreen(disp));
  root = RootWindow(disp, DefaultScreen(disp));
  scr = ScreenOfDisplay(disp, DefaultScreen(disp));
  xid_context = XUniqueContext();

  imlib_context_set_display(disp);
  imlib_context_set_visual(vis);
  imlib_context_set_colormap(cm);
  imlib_context_set_color_modifier(NULL);
  imlib_context_set_progress_function(NULL);
  imlib_context_set_operation(IMLIB_OP_COPY);

  // TODO: might need to reenable this
  // wmDeleteWindow = XInternAtom(disp, "WM_DELETE_WINDOW", False);

  // imlib_set_cache_size(opt.cache_size * 1024 * 1024);

  return;
}

Pixmap generate_pmap(Imlib_Image im) {
  XGCValues gcvalues;
  XGCValues gcval;
  GC gc;

  Pixmap pmap;
  pmap = XCreatePixmap(disp, root, scr->width, scr->height, depth);

  imlib_context_set_image(im);
  imlib_context_set_drawable(pmap);
  imlib_context_set_anti_alias(0);
  imlib_context_set_dither(1);
  imlib_context_set_blend(1);
  imlib_context_set_angle(0);

  imlib_render_image_on_drawable_at_size(0, 0, scr->width, scr->height);

  return pmap;
}

void clear_pmap(Pixmap pmap) { XFreePixmap(disp, pmap); }

void set_background(Pixmap pmap_d1) {
  XGCValues gcvalues;
  XGCValues gcval;
  GC gc;

  Display *disp2;
  Window root2;
  Pixmap pmap_d2;
  int depth2;

  disp2 = XOpenDisplay(NULL);
  if (!disp2) {
    return;
  }
  root2 = RootWindow(disp2, DefaultScreen(disp2));
  depth2 = DefaultDepth(disp2, DefaultScreen(disp2));
  XSync(disp, False);
  pmap_d2 = XCreatePixmap(disp2, root2, scr->width, scr->height, depth2);
  gcvalues.fill_style = FillTiled;
  gcvalues.tile = pmap_d1;
  gc = XCreateGC(disp2, pmap_d2, GCFillStyle | GCTile, &gcvalues);
  XFillRectangle(disp2, pmap_d2, gc, 0, 0, scr->width, scr->height);
  XFreeGC(disp2, gc);
  XSync(disp2, False);
  XSync(disp, False);

  Atom prop_root, prop_esetroot, type;
  int format, i;
  unsigned long length, after;
  unsigned char *data_root = NULL, *data_esetroot = NULL;

  prop_root = XInternAtom(disp2, "_XROOTPMAP_ID", True);
  prop_esetroot = XInternAtom(disp2, "ESETROOT_PMAP_ID", True);

  // this kills the client?
  // why is it bad if the root and esetroot properties align?
  // or is it just terminating the currently loaded session, before inserting
  // the new image
  if (prop_root != None && prop_esetroot != None) {
    XGetWindowProperty(disp2, root2, prop_root, 0L, 1L, False, AnyPropertyType,
                       &type, &format, &length, &after, &data_root);
    if (type == XA_PIXMAP) {
      XGetWindowProperty(disp2, root2, prop_esetroot, 0L, 1L, False,
                         AnyPropertyType, &type, &format, &length, &after,
                         &data_esetroot);
      if (data_root && data_esetroot) {
        if (type == XA_PIXMAP &&
            *((Pixmap *)data_root) == *((Pixmap *)data_esetroot)) {
          XKillClient(disp2, *((Pixmap *)data_root));
        }
      }
    }
  }

  if (data_root)
    XFree(data_root);

  if (data_esetroot)
    XFree(data_esetroot);

  /* This will locate the property, creating it if it doesn't exist */
  prop_root = XInternAtom(disp2, "_XROOTPMAP_ID", False);
  prop_esetroot = XInternAtom(disp2, "ESETROOT_PMAP_ID", False);

  if (prop_root == None || prop_esetroot == None) {
    return;
  }

  XChangeProperty(disp2, root2, prop_root, XA_PIXMAP, 32, PropModeReplace,
                  (unsigned char *)&pmap_d2, 1);
  XChangeProperty(disp2, root2, prop_esetroot, XA_PIXMAP, 32, PropModeReplace,
                  (unsigned char *)&pmap_d2, 1);

  XSetWindowBackgroundPixmap(disp2, root2, pmap_d2);
  XClearWindow(disp2, root2);
  XFlush(disp2);
  XSetCloseDownMode(disp2, RetainPermanent);
  XCloseDisplay(disp2);

  return;
}
