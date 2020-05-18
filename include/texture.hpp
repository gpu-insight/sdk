#pragma once

#include <iostream>
#include <cmath>
#include <GL/gl.h>
#include "botson_sdk.hpp"

BOTSON_SDK_API
class Texture {
public:
    Texture(GLenum target, GLsizei width = 0, GLsizei height = 0, GLsizei depth = 1, void *pixel = nullptr);
    ~Texture();


    template <typename... Files>
    static Texture *create_texture_from_image(GLenum target, const Files... files);

    void tex_parameter(GLenum pname, GLfloat param);
    void tex_parameter(GLenum pname, GLint param);

    GLuint id() const { return name; };
    GLsizei width() const { return w; };
    GLsizei height() const { return h; };

private:
    void read_png(std::string png);
    void read_jpg(std::string jpg);
    void load_image(std::string filename);

private:
    GLuint  name;
    GLenum  t;
    GLsizei w;
    GLsizei h;
    GLsizei slices;
    char *data;
};


template <typename... Files>
Texture *Texture::create_texture_from_image(GLenum target, const Files... files)
{
    GLuint levels = 1;
    int width, height;

    const int fnum = sizeof...(files);
    std::string farr[fnum] = {files...};

    Texture *tex = new Texture(target);

    tex->load_image(farr[0]);
    if (nullptr == tex->data) {
        std::cout << "could not create texture from: " << farr[0] << "\n";
        return nullptr;
    }

    width = tex->w;
    height = tex->h;

    switch (target)
    {
    case GL_TEXTURE_2D:
        levels += std::floor(std::log2(std::max(width, height)));

        glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGB8, width, height);

        for(int level = 0; level < levels; level++) {
            glTexSubImage2D(GL_TEXTURE_2D, level,
                            0, 0, width >> level, height >> level,
                            GL_RGB, GL_UNSIGNED_BYTE, tex->data);
        };

        break;
    case GL_TEXTURE_CUBE_MAP:
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                     GL_RGB8, width, height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, tex->data);

        for (int i = 1; i < fnum; i++) {
            tex->load_image(farr[i]);
            if (nullptr == tex->data) {
                std::cout << "could not create texture from: " << farr[i] << "\n";
                break;
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                        GL_RGB8, width, height, 0,
                        GL_RGB, GL_UNSIGNED_BYTE, tex->data);
        }

        break;
    default:
        break;
    }

    if (nullptr == tex->data)
    {
        delete tex;
        return nullptr;
    }
    else
    {
        return tex;
    }
}

