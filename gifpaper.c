#include "gifpaper.h"

void main(void)
{
  init_x_and_imlib();
  
  Imlib_Image im; 
  char *filename = "./test.jpg";
  load_image(&im, filename);

  set_background(im);
}
