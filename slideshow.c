#include "gifpaper.h"

// signal primitives
static pthread_mutex_t timer_lock;
static int timer_signal = 0;

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

struct timespec time_combine(struct timespec a, struct timespec b) {
  struct timespec temp;
  temp.tv_sec = a.tv_sec + b.tv_sec;
  if (a.tv_nsec + b.tv_nsec > 999999999) {
    temp.tv_sec += 1;
  }
  temp.tv_nsec = (a.tv_nsec + b.tv_nsec) % 1000000000;
  return temp;
}

struct timespec generate_load_projection(struct timespec start,
                                         struct timespec load_start,
                                         struct timespec end) {
  struct timespec load_diff, diff, temp;

  load_diff = time_diff(load_start, end);
  diff = time_diff(start, end);
  temp.tv_sec = load_diff.tv_sec;
  temp.tv_nsec = (long)(0.2 * load_diff.tv_nsec);

  return time_combine(time_combine(diff, load_diff), temp);
}

int display_as_slideshow(char *dirpath, long framerate, long sliderate) {

  pthread_mutex_init(&timer_lock, NULL);

  SlideshowEntry *c_gif = load_slideshow_paths(dirpath);
  if (c_gif == NULL) {
    printf("Error: gif directory is empty.\n");
    return -1;
  } else if (c_gif->next == c_gif) {
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
  struct timespec proj, load_start;

  int frames_processed, file_count;
  frames_processed = 0;
  file_count = 0; // to shut up the warning

  struct timespec w_slideshow;
  w_slideshow.tv_sec = sliderate;
  w_slideshow.tv_nsec = 0;

  pthread_t tid;
  pthread_create(&tid, NULL, timer_thread, &w_slideshow);

  while (True) {
    pthread_mutex_lock(&timer_lock);
    if (timer_signal) {
      // finish queueing next gif if not yet complete
      if (frames_processed < file_count) {
        printf("Delaying play to finish queueing next gif...\n");
      }
      while (frames_processed < file_count) {
        load_image_to_list(n_frame, frames_processed);
        if (frames_processed + 1 == file_count) {
          free(n_frame->next);
          n_frame->next = n_frame_head;
        } else {
          n_frame = n_frame->next;
        }
        frames_processed += 1;
      }
      // swap out gifs, and reset timer
      clean_gif_frames(c_frame);
      c_frame = n_frame_head;
      n_frame = NULL;
      timer_signal = 0;
      c_gif = c_gif->next;
      pthread_create(&tid, NULL, timer_thread, &w_slideshow);
      printf("Switching gifs...\n");
    }
    pthread_mutex_unlock(&timer_lock);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    set_background(c_frame->pmap);
    c_frame = c_frame->next;

    if (n_frame == NULL) {
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
      while (frames_processed < file_count) {

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &load_start);
        printf("Preloading frame %d of the next gif in the slideshow.\n",
               frames_processed);
        load_image_to_list(n_frame, frames_processed);
        if (frames_processed + 1 == file_count) {
          free(n_frame->next);
          n_frame->next = n_frame_head;
        } else {
          n_frame = n_frame->next;
        }
        frames_processed += 1;

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        proj = generate_load_projection(start, load_start, end);
        if (proj.tv_sec > 0 || proj.tv_nsec >= w_frame.tv_nsec) {
          break;
        }
      }
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    }

    diff = time_diff(start, end);

    if (diff.tv_sec > 0 || diff.tv_nsec >= w_frame.tv_nsec) {
      printf("Timing failure! Expect a choppy frame...\n");
      continue;
    } else {
      w_actual.tv_sec = 0;
      w_actual.tv_nsec = w_frame.tv_nsec - diff.tv_nsec;
    }

    nanosleep(&w_actual, NULL);
  }
}
