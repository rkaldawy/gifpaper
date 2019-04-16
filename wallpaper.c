#include "gifpaper.h"
#include "wallpaper.h"

Display *disp = NULL;
Visual *vis = NULL;
Screen *scr = NULL;
Colormap cm;
int depth;
XContext xid_context = 0;
Window root = 0;

void init_x_and_imlib(void)
{

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
  //TODO: might need to reenable this
	//wmDeleteWindow = XInternAtom(disp, "WM_DELETE_WINDOW", False);

	//imlib_set_cache_size(opt.cache_size * 1024 * 1024);

	return;
}

//todo: find out how image gets loaded
void set_background(Imlib_Image im)
{
  XGCValues gcvalues;
  XGCValues gcval;
	GC gc;

	Pixmap pmap_d1, pmap_d2;

	XColor color;
  Colormap cmap = DefaultColormap(disp, DefaultScreen(disp));

	XAllocNamedColor(disp, cmap, "black", &color, &color);
	pmap_d1 = XCreatePixmap(disp, root, scr->width, scr->height, depth);

  imlib_context_set_image(im);
  imlib_context_set_drawable(pmap_d1);
  imlib_context_set_anti_alias(0);
  imlib_context_set_dither(1);
  imlib_context_set_blend(1);
  imlib_context_set_angle(0);

  imlib_render_image_on_drawable_at_size(0, 0, scr->width, scr->height);

  Display *disp2;
  Window root2;
  int depth2;
  
	disp2 = XOpenDisplay(NULL);
		if (!disp2)
      return;
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
  XFreePixmap(disp, pmap_d1);

	Atom prop_root, prop_esetroot, type;
  int format, i;
  unsigned long length, after;
  unsigned char *data_root = NULL, *data_esetroot = NULL;

  prop_root = XInternAtom(disp2, "_XROOTPMAP_ID", True);
  prop_esetroot = XInternAtom(disp2, "ESETROOT_PMAP_ID", True);

  //this kills the client?
  //why is it bad if the root and esetroot properties align?
  //or is it just terminating the currently loaded session, before inserting the new image
  if (prop_root != None && prop_esetroot != None) {
    XGetWindowProperty(disp2, root2, prop_root, 0L, 1L,
           False, AnyPropertyType, &type, &format, &length, &after, &data_root);
    if (type == XA_PIXMAP) {
      XGetWindowProperty(disp2, root2,
             prop_esetroot, 0L, 1L,
             False, AnyPropertyType,
             &type, &format, &length, &after, &data_esetroot);
      if (data_root && data_esetroot) {
        if (type == XA_PIXMAP && *((Pixmap *) data_root) == *((Pixmap *) data_esetroot)) {
          XKillClient(disp2, *((Pixmap *) data_root));
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

  if (prop_root == None || prop_esetroot == None)
    return;

  XChangeProperty(disp2, root2, prop_root, XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &pmap_d2, 1);
  XChangeProperty(disp2, root2, prop_esetroot, XA_PIXMAP, 32,
      PropModeReplace, (unsigned char *) &pmap_d2, 1);

  XSetWindowBackgroundPixmap(disp2, root2, pmap_d2);
  XClearWindow(disp2, root2);
  XFlush(disp2);
  XSetCloseDownMode(disp2, RetainPermanent);
	XCloseDisplay(disp2);

	return;
}
