#include "gifpaper.h"

void main(void)
{
  init_x_and_imlib();
  
  Imlib_Image im; 
  char *filename = "./test.jpg";
  if (!load_image(&im, filename)){
    printf("Background picture doesn't exist!\n");
    return;
  }
  set_background(im);
}
