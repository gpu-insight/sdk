#ifndef BMP_H
#define BMP_H

struct file_header {
    unsigned short file_type;
    unsigned int file_size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offset;
} __attribute__((packed));

struct bmp_header {
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned short planes;
    unsigned short bit_count;
    unsigned int compression;
    unsigned int size_image;
    unsigned int xpels;
    unsigned int ypels;
    unsigned int colors_used;
    unsigned int colors_important;
} __attribute__((packed));

struct rgb_quad {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char reserved;
};

struct rgb_data {
    unsigned char *red;
    unsigned char *green;
    unsigned char *blue;
    unsigned char *alpha;
};

struct bmp_data {
    char *file_name;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    unsigned int image_size;
    struct rgb_data rgb;
    unsigned char *image_data;
};

struct texture_data {
    char *file_name;
    int width;
    int height;
    //	int depth;
    unsigned char *data;
};

typedef struct texture_data TexData;
typedef struct bmp_data tBMP;

void bmp_info(struct file_header file_header, struct bmp_header bmp_header);
int read_bmp(tBMP *bmp_data);
int rgba_to_stream(tBMP *bmp_data);
int stream_to_rgba(unsigned int *stream, tBMP bmp_data);
int write_bmp(const char *file_name, tBMP bmp_data);
void rgb_info(tBMP bmp_data);
void read_png(TexData *image);  // GL_RGBA
void read_jpeg(TexData *image); // GL_RGB

#endif
