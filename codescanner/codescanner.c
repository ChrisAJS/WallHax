/**
* This source is based on the scan_image.c example code distributed with zbar-0.10
*/

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <jpeglib.h>
#include <zbar.h>

zbar_image_scanner_t *scanner = NULL;

void print_usage();

void print_csv(const zbar_symbol_t *symbol);

/* to complete a runnable example, this abbreviated implementation of
 * get_data() will use libpng to read an image file. refer to libpng
 * documentation for details
 */
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
        png_set_palette_to_rgb(png);
    /*if(color == PNG_COLOR_TYPE_GRAY && bits < 8)
        png_set_gray_1_2_4_to_8(png);*/
    if(bits == 16)
    {
        png_set_strip_16(png);
    }

    if(color & PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png);
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

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

void my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
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



  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }
  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
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
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */

   /*
   * Allocate us a buffer big enough to hold the image in memory once it's decompressed
   */
   *raw = malloc(cinfo.output_width * cinfo.output_height * 3);
   *width = cinfo.output_width;
   *height = cinfo.output_height;
   int rowReadCount = 0;
  while (cinfo.output_scanline < cinfo.output_height) {
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

int main (int argc, char **argv)
{
    if(argc < 2)
    {
        print_usage();
        return 1;
    }

    /* create a reader */
    scanner = zbar_image_scanner_create();

    /* configure the reader */
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

    /* obtain image data */
    int width = 0, height = 0;
    void *raw = NULL;
    if(strncmp("png", argv[1] + strlen(argv[1])-3, 3) == 0)
    {
       get_data(argv[1], &width, &height, &raw);
    }
    else
    {
        read_JPEG_file(argv[1], &raw, &width, &height);
    }

    /* wrap image data */
    zbar_image_t *image = zbar_image_create();
    zbar_image_set_format(image, *(int*)"Y800");
    zbar_image_set_size(image, width, height);
    zbar_image_set_data(image, raw, width * height, zbar_image_free_data);

    /* scan the image for barcodes */
    int n = zbar_scan_image(scanner, image);

    /* extract results */
    const zbar_symbol_t *symbol = NULL;

    for(symbol = zbar_image_first_symbol(image); symbol != NULL; symbol = zbar_symbol_next(symbol))
    {
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        if(strncmp(zbar_get_symbol_name(typ), "QR-Code", 7) != 0)
        {
            fprintf(stderr, "Encountered unsupported barcode format: %s\n", zbar_get_symbol_name(typ));
        }
        else
        {
            print_csv(symbol);
        }
    }

    /* clean up */
    zbar_image_destroy(image);
    zbar_image_scanner_destroy(scanner);

    return(0);
}

void print_usage()
{
    printf("Usage: zbar <in-file.png|jpg>\n");
}

void print_csv(const zbar_symbol_t *symbol)
{
    printf("%d,%d,%s\n", zbar_symbol_get_loc_x(symbol, 0), zbar_symbol_get_loc_y(symbol, 0), zbar_symbol_get_data(symbol));
}
