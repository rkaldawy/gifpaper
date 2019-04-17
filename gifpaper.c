#include "gifpaper.h"

char *generate_filename(int idx)
{
  char buf[100];
  memset(buf, 0x00, 100);
  
  if(idx < 10){
    snprintf(buf, 100, "./gif/frame_%c%d_delay-0.1s.gif", '0', idx);
  } else {
    snprintf(buf, 100, "./gif/frame_%d_delay-0.1s.gif", idx);
  }
  
  return strndup(buf, 100);
}

void main(void)
{
  init_x_and_imlib();
  
  Frame *c = (Frame *)malloc(sizeof(Frame));
  Frame *head = c;

  for (int i = 0; i < 32; i++){
    Imlib_Image im; 
    char *filename = generate_filename(i);
    if (!load_image(&im, filename)){
      printf("Background picture doesn't exist!\n");
      return;
    }
    c->pmap = generate_pmap(im);
    c->next = (Frame *)malloc(sizeof(Frame));
    if(i < 31){
      c = c->next; 
    }
  }
  free(c->next);
  c->next = head;

  struct timespec w;
  w.tv_sec = 0;
  w.tv_nsec = 166666666;
  
  while(1){
    set_background(head->pmap);
    head = head->next;
    nanosleep(&w, NULL);
  } 
}
