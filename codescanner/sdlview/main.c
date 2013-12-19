/**
* This program renders a png or a jpeg as would be passed into the ZBar library
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <jpeglib.h>
#include <png.h>

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

void my_error_exit (j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  (*cinfo->err->output_message) (cinfo);

  longjmp(myerr->setjmp_buffer, 1);
}

int read_JPEG_file(char *filename, void **raw, int *width, int *height)
{
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;

  struct my_error_mgr jerr;
  /* More stuff */
  FILE * infile;        /* source file */
  JSAMPARRAY buffer;        /* Output row buffer */
  int row_stride;       /* physical row width in output buffer */

  if ((infile = fopen(filename, "rb")) == NULL)
  {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  if (setjmp(jerr.setjmp_buffer))
  {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }

  /* Establish the setjmp return context for my_error_exit to use. */
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);

  row_stride = cinfo.output_width * cinfo.output_components;

  buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  *raw = malloc(cinfo.output_width * cinfo.output_height * 3);
  *width = cinfo.output_width;
  *height = cinfo.output_height;
  int rowReadCount = 0;
  while (cinfo.output_scanline < cinfo.output_height)
  {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    jpeg_read_scanlines(&cinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
    if(cinfo.output_scanline < cinfo.output_height-1)
    {
        memcpy(*raw + (cinfo.output_scanline * row_stride), buffer[0], row_stride);
    }
  }

  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);

  /* And we're done! */
  return 1;
}


static void get_data (const char *name,
                      int *width, int *height,
                      void **raw)
{
    FILE *file = fopen(name, "rb");
    if(file == NULL)
    {
        exit(2);
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png)
    {
        exit(3);
    }

    if(setjmp(png_jmpbuf(png)))
    {
        exit(4);
    }

    png_infop info = png_create_info_struct(png);
    if(!info)
    {
        exit(5);
    }

    png_init_io(png, file);
    png_read_info(png, info);
    /* configure for 8bpp grayscale input */
    int color = png_get_color_type(png, info);
    int bits = png_get_bit_depth(png, info);
    if(color & PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png);
    }

    /*if(color == PNG_COLOR_TYPE_GRAY && bits < 8)
        png_set_gray_1_2_4_to_8(png);*/
    if(bits == 16)
    {
        png_set_strip_16(png);
    }

    if(color & PNG_COLOR_MASK_ALPHA)
    {
        png_set_strip_alpha(png);
    }

    if(color & PNG_COLOR_MASK_COLOR)
    {
        png_set_rgb_to_gray_fixed(png, 1, -1, -1);
    }

    /* allocate image */
    *width = png_get_image_width(png, info);
    *height = png_get_image_height(png, info);
    *raw = malloc(*width * *height);
    png_bytep rows[*height];
    int i;

    for(i = 0; i < *height; i++)
    {
        rows[i] = *raw + (*width * i);
    }

    png_read_image(png, rows);
}

int main(int argc, char **argv)
{
	int displayx = 0;
	int displayy = 0;

	if (argc < 2)
	{
		fprintf(stderr, "Please provide a JPG as an argument\n");
		return -1;
	}

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Surface *screen = SDL_SetVideoMode(1024, 768, 0, SDL_SWSURFACE);

	void *pixelData = NULL;
	int width;
	int height;

	SDL_Surface *jpegImage = NULL;

	if(strncmp(argv[1]+strlen(argv[1])-3, "png", 3) == 0)
	{
		fprintf(stderr, "Reading PNG\n");
		get_data(argv[1], &width, &height, &pixelData);
		jpegImage = SDL_CreateRGBSurfaceFrom(pixelData, width, height, 24, width*3, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00);
	}
	else
	{
		fprintf(stderr, "Reading JPG\n");
		read_JPEG_file(argv[1], &pixelData, &width, &height);
		jpegImage = SDL_CreateRGBSurfaceFrom(pixelData, width, height, 24, width*3, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x0);
	}

	SDL_Event event;
	while(SDL_WaitEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			break;
		}

		SDL_BlitSurface(jpegImage, NULL, screen, NULL);
		SDL_Flip(screen);
	}

	SDL_Quit();

	return 0;
}
