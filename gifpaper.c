#include "gifpaper.h"

int display_as_gif(char *gifpath, long framerate) {
  Frame *head = load_images_to_list(gifpath);
  if (head == NULL) {
    return -1;
  }

  struct timespec w;
  w.tv_sec = 0;
  w.tv_nsec = 999999999 / framerate; // 1 second divided by frame rate

  while (True) {
    check_power_conditions();
    set_background(head);
    head = head->next;
    nanosleep(&w, NULL);
  }
}

// display_as_slideshow lives in slideshow.c

static struct option long_options[] = {{"crop", required_argument, NULL, 'c'},
                                       {"replicate", no_argument, NULL, 'r'},
                                       {"extend", no_argument, NULL, 'e'},
                                       {"help", no_argument, NULL, 'h'},
                                       {"power-save", no_argument, NULL, 'p'},
                                       {NULL, 0, NULL, 0}};

const char *help_string =
    "Gifpaper: a tool for drawing gifs to the X root window (i.e., the wallpaper).\n\
Syntax: gifpaper [options] wallpaper.gif \n\
\n\
Options: \n\
-f FRAMERATE              Set the framerate of the gif. \n\
-s SLIDESHOW_RATE         Slideshow mode. Must provide a directory with gifs. \n\
-h, --help                Show this help menu. \n\
    --crop 'x0 y0 x1 y1'  Crop gif to the dimensions speficied by the coordinates. \n\
    --power-save           Only run the gif if the battery is charging. \n\
\n\
Multihead Options : \n\
    --extend              Extend the gif, scaled, across all monitors. \n\
    --replicate           Replicate the gif across each monitor. \n\
\n\
Please report bugs to <remykaldawy@gmail.com>.\n";

int needs_crop = 0;
int battery_saver = 0;

int crop_params[4] = {0};

int display_mode = 0;

int main(int argc, char **argv) {

  printf("%d\n", detect_charging());

  long framerate = 12;
  int slideshow_mode = 0;
  int sliderate = 180;
  int opt;

  char *endptr;

  while ((opt = getopt_long(argc, argv, "s:f:hc:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'f':
      framerate = strtol(optarg, &endptr, 10);
      if (*optarg == '\0' || *endptr != '\0') {
        printf("Error: framerate argument not an integer.\n");
        return -1;
      }
      if (framerate <= 0 || framerate > 60) {
        printf("Error: Framerate must be between 1Hz and 60Hz.\n");
        return -1;
      }
      break;
    case 's':
      slideshow_mode = 1;
      sliderate = strtol(optarg, &endptr, 10);
      if (*optarg == '\0' || *endptr != '\0') {
        printf("Error: the slideshow rate argument is not an integer.\n");
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
          printf("Error: crop option needs four integer parameters.\n");
          return -1;
        }
        num_str = strtok(NULL, " ");
      }
      if (i < 4) {
        printf("Usage: --crop 'x0 y0 x1 y1'.");
        return -1;
      }
      break;
    case 'r':
      display_mode = DISPLAY_MODE_REPLICATE;
      break;
    case 'e':
      display_mode = DISPLAY_MODE_EXTEND;
      break;
    case 'h':
      printf("%s", help_string);
      return 0;
    case 'p':
      if (detect_charging() < 0) {
        printf("Warning: cannot use battery saving mode.\n");
      } else {
        battery_saver = 1;
      }
      break;
    default:
      printf("Error: invalid option at '%s'\n", argv[optind]);
      return -1;
    }
  }

  if (optind >= argc) {
    printf("Error: no gif or directory specified.\n");
    return -1;
  }
  char *gifpath = argv[optind];

  init_x();
  init_xinerama();

  if (slideshow_mode) {
    printf("Hello.\n");
    display_as_slideshow(gifpath, framerate, sliderate);
  } else {
    display_as_gif(gifpath, framerate);
  }
}
