#include "gifpaper.h"

int break_gif_into_images(char *filename) {
  char path[200];
  memset(path, 0x00, 200);
  char *subpath[3] = {".config", "gifpaper", ".frames"};

  // build the path of the gif frames
  char *home = getenv("HOME");
  if (!home) {
    printf("Error: HOME directory does not exist!");
    return -1;
  }
  snprintf(path, 200, "%s", home);

  for (int i = 0; i < 3; i++) {
    snprintf(path + strlen(path), 200, "/%s", subpath[i]);

    struct stat st;
    if (stat(path, &st) == -1) {
      mkdir(path, 0700);
    }
  }
  snprintf(path + strlen(path), 200, "/%s", "frame%05d.png");

  // make sure the directory is clean
  clear_image_dir();

  char *argv[5] = {"ffmpeg", "-i", filename, path, NULL};

  pid_t child_pid = fork();
  int child_status;
  pid_t tpid = 0;

  if (child_pid == 0) {
    execvp(argv[0], argv);
    exit(0);
  } else {
    do {
      tpid = wait(&child_status);
    } while (tpid != child_pid);
  }
  return 0;
}

int clear_image_dir(void) {
  char basepath[200];
  memset(basepath, 0x00, 200);
  DIR *dirp;
  struct dirent *entry;

  char *home = getenv("HOME");
  if (!home) {
    printf("Error: HOME directory does not exist!");
    return -1;
  }
  snprintf(basepath, 200, "%s/%s", home, ".config/gifpaper/.frames");

  dirp = opendir(basepath);
  while ((entry = readdir(dirp)) != NULL) {
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
      char fullpath[200];
      snprintf(fullpath, 200, "%s/%s", basepath, entry->d_name);
      unlink(fullpath);
    }
  }
  closedir(dirp);

  return 0;
}

int load_image(Imlib_Image *im, char *filename) {
  imlib_context_set_progress_function(NULL);

  if (!filename)
    return (0);

  *im = imlib_load_image_without_cache(filename);
  if (!im) {
    return (0);
  }
  return (1);
}

char *generate_filename(char *prefix, int idx) {
  char buf[200];
  memset(buf, 0x00, 200);
  snprintf(buf, 200, "%s/frame%05d.png", prefix, idx);
  return strndup(buf, 200);
}

Frame *load_image_to_list(Frame *c, int frame_num) {
  char basepath[200];
  memset(basepath, 0x00, 200);

  char *home = getenv("HOME");
  if (!home) {
    printf("Error: HOME directory does not exist!");
    return NULL;
  }
  snprintf(basepath, 200, "%s/%s", home, ".config/gifpaper/.frames");

  Imlib_Image im;
  char *filename = generate_filename(basepath, frame_num + 1);
  if (!load_image(&im, filename)) {
    printf("Background picture doesn't exist!\n");
    free(filename);
    return NULL;
  }
  free(filename);

  c->pmap = generate_pmap(im);
  c->next = (Frame *)malloc(sizeof(Frame));

  return c;
}

void scale(unsigned char *dst, int dstWidth, int dstX, int dstY, int dstW,
           int dstH, unsigned char *src, int srcWidth, int srcX, int srcY,
           int srcW, int srcH) {
  for (int y = 0; y < dstH; y++) {
    for (int x = 0; x < dstW; x++) {
      int indexDst = ((dstY + y) * dstWidth + (dstX + x)) * 4;
      if (0) {
        float x2f = srcX + (x * srcW / (float)dstW);
        float y2f = srcY + (y * srcH / (float)dstH);
        int x2 = (int)x2f;
        int y2 = (int)y2f;
        float dx = x2f - x2;
        float dy = y2f - y2;

        int index1 = (y2 * srcWidth + x2) * 4;
        int index2 = (y2 * srcWidth + (x2 + 1)) * 4;
        int index3 = ((y2 + 1) * srcWidth + x2) * 4;
        int index4 = ((y2 + 1) * srcWidth + (x2 + 1)) * 4;

        for (int i = 0; i < 3; i++) {
          int dstTest =
              (int)((float)src[index1 + i] * (1.0f - dx) * (1.0f - dy) +
                    (float)src[index2 + i] * (dx) * (1.0f - dy) +
                    (float)src[index3 + i] * (1.0f - dx) * (dy) +
                    (float)src[index4 + i] * (dx) * (dy));
          if (dstTest > 255)
            dst[indexDst + i] = 255;
          else if (dstTest < 0)
            dst[indexDst + i] = 0;
          else
            dst[indexDst + i] = dstTest;
        }
      } else {
        int indexSrc = ((srcY + (y * srcH / dstH)) * srcWidth +
                        (srcX + (x * srcW / dstW))) *
                       3;
        dst[indexDst + 2] = src[indexSrc + 0];
        dst[indexDst + 1] = src[indexSrc + 1];
        dst[indexDst + 0] = src[indexSrc + 2];
      }
    }
  }
}

Frame *__load_images_to_list(char *gifpath) {

  Frame *p = NULL;
  Frame *c = (Frame *)malloc(sizeof(Frame));
  Frame *head = c;

  gd_GIF *gif = gd_open_gif(gifpath);
  uint8_t *buffer = (uint8_t *)malloc(gif->width * gif->height * 4);
  uint8_t *scaled = (uint8_t *)malloc(scr->width * scr->height * 4);
  for (int i = 0; gd_get_frame(gif); i++) {
    gd_render_frame(gif, buffer);
    scale(scaled, scr->width, 0, 0, scr->width, scr->height, buffer, gif->width,
          0, 0, gif->width, gif->height);
    printf("%d %d\n", buffer[1], scaled[1]);
    c->pmap = __generate_pmap(scaled);
    if (i == 0) { // the first frame
      set_background(c);
    }
    c->next = (Frame *)malloc(sizeof(Frame));
    p = c;
    c = c->next;
  }

  if (p != NULL) {
    free(c);
    c = p;
  }
  c->next = head;

  return head;
}

Frame *load_images_to_list(void) {
  int file_count = 0;
  DIR *dirp;
  struct dirent *entry;

  char basepath[200];
  memset(basepath, 0x00, 200);

  char *home = getenv("HOME");
  if (!home) {
    printf("Error: HOME directory does not exist!");
    return NULL;
  }
  snprintf(basepath, 200, "%s/%s", home, ".config/gifpaper/.frames");

  dirp = opendir(basepath);
  while ((entry = readdir(dirp)) != NULL) {
    if (entry->d_type == DT_REG) {
      file_count++;
    }
  }
  closedir(dirp);

  Frame *c = (Frame *)malloc(sizeof(Frame));
  Frame *head = c;

  for (int i = 0; i < file_count; i++) {
    Imlib_Image im;
    char *filename = generate_filename(basepath, i + 1);
    if (!load_image(&im, filename)) {
      printf("Background picture doesn't exist!\n");
      return NULL;
    }
    free(filename);
    if (needs_crop) {
      im = crop_image(im, crop_params[0], crop_params[1], crop_params[2],
                      crop_params[3]);
    }
    c->pmap = generate_pmap(im);
    if (i == 0) { // the first frame
      set_background(c);
    }
    c->next = (Frame *)malloc(sizeof(Frame));
    if (i < file_count - 1) {
      c = c->next;
    }
  }
  free(c->next);
  c->next = head;

  return head;
}

int count_frames_in_gif() {
  int file_count = 0;
  DIR *dirp;
  struct dirent *entry;

  char basepath[200];
  memset(basepath, 0x00, 200);

  char *home = getenv("HOME");
  if (!home) {
    printf("Error: HOME directory does not exist!");
    return -1;
  }
  snprintf(basepath, 200, "%s/%s", home, ".config/gifpaper/.frames");

  dirp = opendir(basepath);
  while ((entry = readdir(dirp)) != NULL) {
    if (entry->d_type == DT_REG) {
      file_count++;
    }
  }
  closedir(dirp);

  return file_count;
}

/*
 * Grabs all file paths in a directory, and loads them into a linked list
 */
SlideshowEntry *load_slideshow_paths(char *gifpath) {
  SlideshowEntry *p = NULL;
  SlideshowEntry *c = (SlideshowEntry *)malloc(sizeof(SlideshowEntry));
  SlideshowEntry *head = c;

  DIR *dirp;
  struct dirent *entry;

  dirp = opendir(gifpath);

  while ((entry = readdir(dirp)) != NULL) {
    if (entry->d_type == DT_REG) {
      snprintf(c->path, 200, "%s/%s", gifpath, entry->d_name);
      c->next = (SlideshowEntry *)malloc(sizeof(SlideshowEntry));
      p = c;
      c = c->next;
    }
  }
  free(c);

  if (p == NULL) {
    return NULL;
  }

  p->next = head;
  return head;
}

void clean_gif_frames(Frame *head) {
  Frame *c = head->next;
  Frame *temp;
  while (c != head) {
    temp = c->next;
    clear_pmap(c->pmap);
    free(c);
    c = temp;
  }
  clear_pmap(head->pmap);
  free(head);
}
