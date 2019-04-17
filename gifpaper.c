#include "gifpaper.h"

int main(int argc, char **argv)
{
  if(argc < 2){
    printf("Usage: gifpaper gif");
    return -1;
  }
  char *gifpath = argv[1];

  init_x_and_imlib();

  clear_image_dir();

  int ret = break_gif_into_images(gifpath);
  if(ret < 0){
    return -1;
  }

  Frame *head = load_images_to_list();
  if(head == NULL){
    return -1;
  }
  
  struct timespec w;
  w.tv_sec = 0;
  w.tv_nsec = 166666666 / 2;
  
  while(1){
    set_background(head->pmap);
    head = head->next;
    nanosleep(&w, NULL);
  } 
}
