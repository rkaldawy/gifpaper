#include "gifpaper.h"

int display_as_gif(char *gifpath, long framerate) {
  int ret = break_gif_into_images(gifpath);
  if (ret < 0) {
    printf("Error: file was not readable.\n");
    return -1;
  }

  Frame *head = load_images_to_list();
  if (head == NULL) {
    return -1;
  }

  struct timespec w;
  w.tv_sec = 0;
  w.tv_nsec = 999999999 / framerate; // 1 second divided by frame rate

  while (True) {
    set_background(head->pmap);
    head = head->next;
    nanosleep(&w, NULL);
  }
}

// display_as_slideshow in slideshow.c

int main(int argc, char **argv) {

  long framerate = 6;
  int slideshow_mode = 0;
  int opt;

  char *endptr;

  while ((opt = getopt(argc, argv, "sf:")) != -1) {
    switch (opt) {
    case 'f':
      framerate = strtol(optarg, &endptr, 10);
      if (*optarg == '\0' || *endptr != '\0') {
        printf("Error: framerate argument not an integer.\n");
        return -1;
      }
      if (framerate <= 0 || framerate > 60) {
        printf("Error: Framerate must be between 1Hz and 60Hz\n");
        return -1;
      }
      break;
    case 's':
      slideshow_mode = 1;
      break;
    default:
      printf("Usage: gifpaper [-f] wallpaper.gif\n");
      return -1;
    }
  }

  if (optind >= argc) {
    printf("Usage: gifpaper [-f] wallpaper.gif\n");
    return -1;
  }
  char *gifpath = argv[optind];

  init_x_and_imlib();

  if (slideshow_mode) {
    display_as_slideshow(gifpath, framerate);
  } else {
    display_as_gif(gifpath, framerate);
  }
}
