#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include "bmp.h"
//#include "jpeglib.h"
#include <png.h>
#include <GL/gl.h>

#define WIDTHBYTES(bits)  (((bits)+31)/32*4)


void bmp_info(struct file_header file_header, struct bmp_header bmp_header)
{
	printf("file_type is %8x\n", file_header.file_type);
	printf("file_size is %d\n", file_header.file_size);
	printf("reserved1 %d\n", file_header.reserved1);
	printf("reserved2 %d\n", file_header.reserved2);
	printf("offset is %d\n", file_header.offset);

	printf("bmp size is :%d\n", bmp_header.size);
	printf("bmp width is :%d\n", bmp_header.width);
	printf("bmp height is :%d\n", bmp_header.height);
	printf("bmp planes is :%d\n", bmp_header.planes);
	printf("bmp bit count is :%d\n", bmp_header.bit_count);
	printf("bmp compression is :%d\n", bmp_header.compression);
	printf("bmp size image is :%d\n", bmp_header.size_image);
	printf("bmp xpels is :%d\n", bmp_header.xpels);
	printf("bmp ypels is :%d\n", bmp_header.ypels);
	printf("bmp colors_used is :%d\n", bmp_header.colors_used);
	printf("bmp colors_important is :%d\n", bmp_header.colors_important);
}

int read_bmp(tBMP *bmp_data)
{
	struct file_header file_header;
	struct bmp_header bmp_header;
	FILE * fd_in;
	unsigned int len;
	unsigned int l_width;
	int i, j;
	int k = 0;
	int index = 0;
	unsigned int width;
	unsigned int height;
	unsigned char * red;
	unsigned char * green;
	unsigned char * blue;
	unsigned char * alpha;
	unsigned int align_row;
	unsigned char *data_buf = NULL;

//	printf("inside read_bmp...\n");

	fd_in = fopen((const char *)bmp_data->file_name, "rb");
	if(fd_in == NULL) {
		printf("file open failed\n");
		return -1;
	}

	fread(&file_header, 1, 14, fd_in);
	if(file_header.file_type != 0x4d42) {
		printf("file is not .bmp file!\n");
		return -1;
	}
	fread(&bmp_header, 1, 40, fd_in);
	if(bmp_header.bit_count < 32) {
		printf("bmp_header.bit_count: %d, bit count < 32.\n", bmp_header.bit_count);  // do nothing now.
//		return -1;
	}

//	bmp_info(file_header, bmp_header);

	bmp_data->width = bmp_header.width;
	bmp_data->height = bmp_header.height;
	bmp_data->depth = bmp_header.bit_count;
	bmp_data->image_size = bmp_header.size_image;
	len = bmp_data->width * bmp_data->height;

	bmp_data->rgb.red = (unsigned char *)malloc(len);
	bmp_data->rgb.green = (unsigned char *)malloc(len);
	bmp_data->rgb.blue = (unsigned char *)malloc(len);
	bmp_data->rgb.alpha = (unsigned char *)malloc(len);
	if(bmp_data->rgb.red   == NULL |
	   bmp_data->rgb.green == NULL |
	   bmp_data->rgb.blue  == NULL |
	   bmp_data->rgb.alpha == NULL ) {
		printf("malloc err\n");
	}

	l_width = WIDTHBYTES(bmp_data->width * bmp_data->depth);
	len = l_width * bmp_data->height;
	bmp_data->image_data =(unsigned char *)malloc(len);
	if(bmp_data->image_data == NULL) {
		printf("malloc err \n");
		return -1;
	}
	fseek(fd_in, file_header.offset, SEEK_SET);
	if(fread(bmp_data->image_data, 1, len, fd_in) == 0) {
		printf("read file err\n");
		return -1;
	}

	width = bmp_data->width;
	height = bmp_data->height;
	red = bmp_data->rgb.red;
	green = bmp_data->rgb.green;
	blue = bmp_data->rgb.blue;
	alpha = bmp_data->rgb.alpha;
	align_row = l_width - width*4;
    unsigned char *buf = bmp_data->image_data;
	for(i=0; i<height; i++) {
		for(j=0; j<width; j++) {
			index = (height-1-i)*width + j;
//			index = i * width + j;
			blue[index]  = *(buf++);
			green[index] = *(buf++);
			red[index]   = *(buf++);
			alpha[index]   = *(buf++);
			//alpha[index] = 0x00;
		}
//        buf += align_row;
	}

	fclose(fd_in);
	return 0;
}

void rgba_to_stream(tBMP *bmp_data)
{
	unsigned char * red = bmp_data->rgb.red;
	unsigned char * green = bmp_data->rgb.green;
	unsigned char * blue = bmp_data->rgb.blue;
	unsigned char * alpha = bmp_data->rgb.alpha;
	unsigned char * data = (unsigned char *)bmp_data->image_data;
	unsigned width = bmp_data->width;
	unsigned height = bmp_data->height;
	int i, j;
	int index = 0;

	for(i=0; i < height * width; i++) {
		data[index + 0] = red[i];
		data[index + 1] = green[i];
		data[index + 2] = blue[i];
		data[index + 3] = alpha[i];
		index = index + 4;
	}
}
void stream_to_rgba(unsigned int * stream, tBMP bmp_data)
{
	unsigned char * red = bmp_data.rgb.red;
	unsigned char * green = bmp_data.rgb.green;
	unsigned char * blue = bmp_data.rgb.blue;
	unsigned char * alpha = bmp_data.rgb.alpha;
	unsigned char * data = (unsigned char *)stream;
	unsigned int width = bmp_data.width;
	unsigned int height = bmp_data.height;
	int i, j;
	int index = 0;
	unsigned int count = 0;

	for(i=0; i < height * width; i++, index+=4) {
		red[i]   = data[index + 0];
		green[i] = data[index + 1];
		blue[i]  = data[index + 2];
		alpha[i] = data[index + 3];
	}
}
int write_bmp(const char * file_name, tBMP bmp_data)
{
	FILE * fd_out;
	struct file_header file_header;
	struct bmp_header bmp_header;
	unsigned int image_size = bmp_data.image_size;

	unsigned int width = bmp_data.width;
	unsigned int height = bmp_data.height;
	unsigned int l_width;
	unsigned int len;
	unsigned char * data;
	int i, j, k=0;
	int index;
	int align_row;
	unsigned char * red = bmp_data.rgb.red;
	unsigned char * green = bmp_data.rgb.green;
	unsigned char * blue = bmp_data.rgb.blue;
	unsigned char * alpha = bmp_data.rgb.alpha;

	fd_out = fopen(file_name, "w");
	if(fd_out == NULL) {
		printf("[ERROR]: --%s--%d-- open bmp file (%s) failed\n", __FILE__, __LINE__,file_name);
		return -1;
	}

	file_header.file_type = 0x4d42;
	file_header.file_size = image_size + 54;
	file_header.reserved1 = 0;
	file_header.reserved2 = 0;
	file_header.offset = 54;
	fwrite((void *)&file_header, 1, 14, fd_out);

	bmp_header.size = 40;
	bmp_header.width = width;
	bmp_header.height = height;
	bmp_header.planes = 1;
	bmp_header.bit_count = 32;
	bmp_header.compression = 0;
	bmp_header.size_image = image_size;
	bmp_header.xpels = 1920;
	bmp_header.ypels = 1080;
	bmp_header.colors_used = 0;
	bmp_header.colors_important = 0;
	fwrite((void *)&bmp_header, 1, 40, fd_out);

//	bmp_info(file_header, bmp_header);

	l_width = WIDTHBYTES(width * 32);
	len = l_width * height;
	align_row = l_width - width*4;
	data = (unsigned char *)malloc(len);
	if(data == NULL) {
		printf("malloc err\n");
		return -1;
	}
	for(i=0; i<height; i++) {
		for(j=0; j<width; j++) {
//			index = (height - i -1)*width + j;
			index = i * width + j;
			data[k+0] = blue[index];
			data[k+1] = green[index];
			data[k+2] = red[index];
			data[k+3] = alpha[index];
			k+=4;
		}
		//k+= align_row;
	}
	fseek(fd_out, file_header.offset, SEEK_SET);
	fwrite(data, 1, len, fd_out);

	free(data);
	fclose(fd_out);
	return 0;
}

void rgb_info(tBMP bmp_data)
{
	int i;
	for(i=0; i<bmp_data.width * bmp_data.height; i++) {
		printf("(%02x,", bmp_data.rgb.blue[i]);
		printf(" %02x,", bmp_data.rgb.green[i]);
		printf(" %02x,", bmp_data.rgb.red[i]);
		printf(" %02x)", bmp_data.rgb.alpha[i]);
		printf("\n");
	}
}
#if 0
void read_png(TexData *image)
{
 unsigned char header[8];
 int k;
 png_byte color_type;
 png_byte bit_depth;
 png_structp png_ptr;
 png_infop info_ptr;
 int number_of_passes;
 png_bytep * row_pointers;
 int row,col,pos;
 FILE *fp=fopen(image->file_name,"rb");
 if(!fp)
 {
  fclose(fp);
  return;
 }
 fread(header, 1, 8, fp);
 if(png_sig_cmp(header,0,8))
 {
  fclose(fp);
  return;
 }

 png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
 if(!png_ptr)
 {
  fclose(fp);
  return;
 }

 info_ptr=png_create_info_struct(png_ptr);

 if(!info_ptr)
 {
  png_destroy_read_struct(&png_ptr,(png_infopp)NULL,(png_infopp)NULL);
  fclose(fp);
  return;
 }
 if (setjmp(png_jmpbuf(png_ptr)))
 {
   png_destroy_read_struct(&png_ptr,(png_infopp)NULL,(png_infopp)NULL);
   fclose(fp);
   return;
 }


 png_init_io(png_ptr,fp);
 png_set_sig_bytes(png_ptr, 8);

 png_read_info(png_ptr, info_ptr);

 image->width = png_get_image_width(png_ptr, info_ptr);
 image->height = png_get_image_height(png_ptr, info_ptr);
 color_type = png_get_color_type(png_ptr, info_ptr);

 bit_depth = png_get_bit_depth(png_ptr, info_ptr);

 number_of_passes = png_set_interlace_handling(png_ptr);

 png_read_update_info(png_ptr, info_ptr);

 if (setjmp(png_jmpbuf(png_ptr))){
  fclose(fp);
  return;
 }
 image->data=(unsigned char *)malloc(image->width * image->height * 4);

 row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * image->height);

 for (k = 0; k < image->height; k++)
  row_pointers[k] = NULL;

 for (k=0; k<image->height; k++)
  row_pointers[k] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr,
         info_ptr));
 png_read_image(png_ptr, row_pointers);

 pos = (image->width * image->height * 4) - (4 * image->width);
 for( row = 0; row < image->height; row++)
 {
  for( col = 0; col < (4 * image->width); col += 4)
  {
   image->data[pos++] = row_pointers[row][col];
   image->data[pos++] = row_pointers[row][col + 1];
   image->data[pos++] = row_pointers[row][col + 2];
   image->data[pos++] = row_pointers[row][col + 3];
  }
  pos=(pos - (image->width * 4)*2);
 }
 free(row_pointers);
 fclose(fp);
}


void read_jpeg(TexData *image)
{
    unsigned char header[8];
    int k;
    FILE *input_file;
    input_file=fopen(image->file_name,"rb");
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo,input_file);
    jpeg_read_header(&cinfo,TRUE);
    jpeg_start_decompress(&cinfo);

    unsigned long width = cinfo.output_width;
    unsigned long height = cinfo.output_height;
    unsigned short depth = cinfo.output_components;
    image->data = (unsigned char *)malloc(width * height * depth);
    memset(image->data, 0, sizeof(unsigned char) * width * height * depth);
    unsigned char *point = image->data;
    JSAMPARRAY buffer;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, width*depth, 1);

    while(cinfo.output_scanline < height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(point, *buffer, width*depth);
        point += width * depth;
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    image->width = width;
    image->height = height;

    fclose(input_file);
}
#endif
void save2bmp(char *filename, int windowWidth, int windowHeight)
{   
    unsigned int *readPixelBuffer;
    readPixelBuffer = (unsigned int *)malloc(windowWidth * windowHeight * sizeof(unsigned int));
    glReadPixels(0, 0, windowWidth, windowHeight, GL_RGBA, GL_UNSIGNED_BYTE, readPixelBuffer);
    struct bmp_data bmp_data;
    bmp_data.width = windowWidth;
    bmp_data.height = windowHeight;
    bmp_data.image_size = windowWidth * windowHeight;
    bmp_data.rgb.red = (unsigned char *)malloc(windowWidth * windowHeight);
    bmp_data.rgb.green = (unsigned char *)malloc(windowWidth * windowHeight);
    bmp_data.rgb.blue = (unsigned char *)malloc(windowWidth * windowHeight);
    bmp_data.rgb.alpha = (unsigned char *)malloc(windowWidth * windowHeight);
    stream_to_rgba((unsigned int *)readPixelBuffer, bmp_data);
    write_bmp(filename, bmp_data);
}

