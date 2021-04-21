#include <cstdio>
#include <string>
#include <jpeglib.h>
#include <png.h>
#include <string.h>
#include <sis/texture.hpp>


Texture::Texture(GLenum target, GLsizei width, GLsizei height, GLsizei depth, void *pixel)
{
    t = target;
    w = width;
    h = height;
    slices = depth;
    data = (char *)pixel;

    glGenTextures(1, &name);
    glBindTexture(t, name);
    glTexImage2D(target, 0, GL_RGBA, 2, 2,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
}

Texture::~Texture()
{
    glDeleteTextures(1, &name);
}

void Texture::tex_parameter(GLenum pname, GLfloat param)
{
    glTexParameterf(t, pname, param);
}

void Texture::tex_parameter(GLenum pname, GLint param)
{
    glTexParameteri(t, pname, param);
}

void Texture::read_png(std::string png)
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
    FILE *fp=fopen(png.c_str(),"rb");
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

    this->w = png_get_image_width(png_ptr, info_ptr);
    this->h = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);

    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    number_of_passes = png_set_interlace_handling(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr))){
        fclose(fp);
        return;
    }
    this->data=(char *)malloc(this->w * this->h * 4);

    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * this->h);

    for (k = 0; k < this->h; k++)
        row_pointers[k] = NULL;

    for (k = 0; k < this->h; k++)
        row_pointers[k] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr,
                                                                          info_ptr));
    png_read_image(png_ptr, row_pointers);

    pos = (this->w * this->h* 4) - (4 * this->w);
    for( row = 0; row < this->h; row++)
    {
        for( col = 0; col < (4 * this->w); col += 4)
        {
            this->data[pos++] = row_pointers[row][col];
            this->data[pos++] = row_pointers[row][col + 1];
            this->data[pos++] = row_pointers[row][col + 2];
            this->data[pos++] = row_pointers[row][col + 3];
        }
        pos=(pos - (this->w * 4)*2);
    }
    free(row_pointers);
    fclose(fp);
}


void Texture::read_jpg(std::string jpg)
{
    unsigned char header[8];
    int k;
    FILE *input_file;
    input_file=fopen(jpg.c_str(),"rb");
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
    this->data = (char *)malloc(width * height * depth);
    memset(this->data, 0, sizeof(char) * width * height * depth);
    char *point = this->data;
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
    this->w = width;
    this->h = height;

    fclose(input_file);
}

void Texture::load_image(std::string filename)
{
    if ((filename.find_last_of(".jpg") != std::string::npos)
        || (filename.find_last_of(".jpeg") != std::string::npos))
    {
        this->read_jpg(filename);
    }
    else if (filename.find_last_of(".png") != std::string::npos)
    {
        this->read_png(filename);
    }
    else
    {
        std::cerr << "Unsupported texture image format: " << filename << "\n";
        exit(1);
    }
}
