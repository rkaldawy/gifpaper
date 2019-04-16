#include "gifpaper.h"

int load_image(Imlib_Image * im, char *filename)
{
  Imlib_Load_Error err;
  imlib_context_set_progress_function(NULL);

  if (!filename)
    return (0);

  *im = imlib_load_image_with_error_return(filename, &err);
  if ((err) || (!im)) {
    return (0);
  }
  return (1);
}
