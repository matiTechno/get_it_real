#include <opengl/texture.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <opengl/stb_image.h>

GLuint Texture::boundID = 0;

Texture::Texture(const std::string& filename)
{
    unsigned char* data = stbi_load(filename.c_str(), &size.x, &size.y, nullptr, 4);
    if(!data)
        throw std::runtime_error("[stbi_load failed: " + filename + ']');

    auto deleter = [](GLuint* ID)
    {
        glDeleteTextures(1, ID);
        delete ID;
    };

    ID = std::unique_ptr<GLuint, decltype(deleter)>(new GLuint, deleter);

    glGenTextures(1, ID.get());
    bind();
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, size.x, size.y);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

Texture::Texture(GLenum internalFormat, GLsizei width, GLsizei height):
    size(width, height)
{
    auto deleter = [](GLuint* ID)
    {
        glDeleteTextures(1, ID);
        delete ID;
    };

    ID = std::unique_ptr<GLuint, decltype(deleter)>(new GLuint, deleter);

    glGenTextures(1, ID.get());
    bind();
    glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, size.x, size.y);
}

const glm::ivec2& Texture::getSize() const
{
    return size;
}

void Texture::bind(GLuint unit) const
{
    if(*ID != boundID)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, *ID);
        boundID = *ID;
    }
}

GLuint Texture::getID() const
{
    return *ID;
}
