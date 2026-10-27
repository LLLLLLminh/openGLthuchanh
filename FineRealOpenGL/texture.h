#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <iostream>
#include "stb_image.h"

class Texture {
public:
    unsigned int ID;        // OpenGL texture ID
    std::string type;       // optional (e.g., diffuse/specular)
    std::string path;       // file path (for debugging)

    Texture() : ID(0) {}

    // Loads texture from file and generates OpenGL texture
    bool generate(const std::string& filename, bool flipVertically = true) {
        glGenTextures(1, &ID);

        // Load image
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(flipVertically);
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

        if (!data) {
            std::cerr << "Failed to load texture: " << filename << std::endl;
            return false;
        }

        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cerr << "Unsupported channel count: " << nrChannels << std::endl;
            stbi_image_free(data);
            return false;
        }

        // Bind and set parameters
        glBindTexture(GL_TEXTURE_2D, ID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        path = filename;
        return true;
    }

    void bind(unsigned int unit = 0) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    void destroy() {
        glDeleteTextures(1, &ID);
    }
};

#endif#pragma once
