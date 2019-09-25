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
    set_background(head);
    head = head->next;
    nanosleep(&w, NULL);
  }
}

// display_as_slideshow in slideshow.c

static struct option long_options[] = {{"crop", required_argument, NULL, 'c'},
                                       {"replicate", no_argument, NULL, 'r'},
                                       {"extend", no_argument, NULL, 'e'},
                                       {NULL, 0, NULL, 0}};

int needs_crop = 0;
int crop_params[4] = {0};

int display_mode = 0;

int main(int argc, char **argv) {

  long framerate = 6;
  int slideshow_mode = 0;
  int sliderate = 180;
  int opt;

  char *endptr;

  while ((opt = getopt_long(argc, argv, "s:f:c:", long_options, NULL)) != -1) {
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
      sliderate = strtol(optarg, &endptr, 10);
      if (*optarg == '\0' || *endptr != '\0') {
        printf("Error: slideshow rate argument not an integer.\n");
        return -1;
      }
      if (sliderate <= 30) {
        printf("Warning: fast slideshow rates may incur performance costs and "
               "choppiness.\n");
      }
      break;
    case 'c':
      needs_crop = 1;
      char *num_str = strtok(optarg, " ");
      int i;
      for (i = 0; i < 4 && num_str != NULL; i++) {
        crop_params[i] = strtol(num_str, &endptr, 10);
        if (*num_str == '\0' || *endptr != '\0') {
          printf("Error: crop argument not an integer.\n");
          return -1;
        }
        num_str = strtok(NULL, " ");
      }
      if (i < 4) {
        printf("Caught error!");
        return -1;
      }
      break;
    case 'r':
      display_mode = DISPLAY_MODE_REPLICATE;
      break;
    case 'e':
      display_mode = DISPLAY_MODE_EXTEND;
      break;
    default:
      printf("Usage: gifpaper [-fs] wallpaper.gif\n");
      return -1;
    }
  }

  if (optind >= argc) {
    printf("Usage: gifpaper [-fs] wallpaper.gif\n");
    return -1;
  }
  char *gifpath = argv[optind];

  init_x_and_imlib();
  init_xinerama();

  if (slideshow_mode) {
    display_as_slideshow(gifpath, framerate, sliderate);
  } else {
    display_as_gif(gifpath, framerate);
  }
}
