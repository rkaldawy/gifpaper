#include "gifpaper.h"

// signal primitives
pthread_mutex_t timer_lock;
int timer_signal = 0;

void *timer_thread(void *args) {
  struct timespec w = *(struct timespec *)args;
  nanosleep(&w, NULL);
  pthread_mutex_lock(&timer_lock);
  timer_signal = 1;
  pthread_mutex_unlock(&timer_lock);
}

struct timespec time_diff(struct timespec start, struct timespec end) {
  struct timespec temp;
  if ((end.tv_nsec - start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }
  return temp;
}

int display_as_slideshow(char *dirpath, long framerate) {

  pthread_mutex_init(&timer_lock, NULL);

  SlideshowEntry *c_gif = load_slideshow_paths(dirpath);
  if (c_gif == NULL) {
    printf("Error: gif directory is empty.\n");
    return -1;
  } else if (c_gif->next == c_gif) {
    printf("Lul");
    display_as_gif(c_gif->path, framerate);
    return 0;
  }

  Frame *c_frame, *n_frame, *n_frame_head;
  // generate the first gif in the series
  if (break_gif_into_images(c_gif->path) < 0) {
    printf("Error: file was not readable.\n");
    return -1;
  }
  c_frame = load_images_to_list();
  if (c_frame == NULL) {
    return -1;
  }
  n_frame = NULL;
  c_gif = c_gif->next;

  struct timespec w_frame, w_actual;
  w_frame.tv_sec = 0;
  w_frame.tv_nsec = 999999999 / framerate; // 1 second divided by frame rate

  struct timespec start, end, diff;

  int frames_processed, file_count;
  frames_processed = 0;

  struct timespec w_slideshow;
  w_slideshow.tv_sec = 60; // TODO: take this as an argument
  w_slideshow.tv_nsec = 0;

  pthread_t tid;
  pthread_create(&tid, NULL, timer_thread, &w_slideshow);

  while (True) {
    printf("%s\n", c_gif->path);

    pthread_mutex_lock(&timer_lock);
    if (timer_signal) {
      // TODO: add a check in case the gif hasnt been fully loaded yet
      c_frame = n_frame_head;
      n_frame = NULL;
      timer_signal = 0;
      c_gif = c_gif->next;
      pthread_create(&tid, NULL, timer_thread, &w_slideshow);
      printf("Switching...\n");
    }
    pthread_mutex_unlock(&timer_lock);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    // STEP 1: Send the frame of the current gif to the background
    printf("%x\n", c_frame);
    set_background(c_frame->pmap);
    c_frame = c_frame->next;

    // STEP 2: Begin pre-loading operation
    if (n_frame == NULL) {
      printf("We are starting gif preparation.\n");
      // prepare the gif frames and count them
      if (break_gif_into_images(c_gif->path) < 0) {
        printf("Error: file was not readable.\n");
        return -1;
      }
      frames_processed = 0;
      file_count = count_frames_in_gif();
      n_frame = (Frame *)malloc(sizeof(Frame));
      n_frame_head = n_frame;
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    } else {
      // start writing frames to the circular list
      if (frames_processed < file_count) {
        printf("We are loading another frame! %d\n", file_count);
        load_image_to_list(n_frame, frames_processed);
        if (frames_processed + 1 == file_count) {
          free(n_frame->next);
          n_frame->next = n_frame_head;
        } else {
          n_frame = n_frame->next;
        }
        frames_processed += 1;
      }
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    }

    diff = time_diff(start, end);

    if (diff.tv_sec > 0 || diff.tv_nsec >= w_frame.tv_nsec) {
      printf("Timing failure! Expect choppy frames...\n");
      continue;
    } else {
      w_actual.tv_sec = 0;
      w_actual.tv_nsec = w_frame.tv_nsec - diff.tv_nsec;
    }

    nanosleep(&w_actual, NULL);
  }
}
