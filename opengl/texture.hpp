#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <memory>
#include <functional>

class Texture
{
public:
    Texture(const std::string& filename);
    Texture(GLenum internalFormat, GLsizei width, GLsizei height);

    const glm::ivec2& getSize() const;
    void bind(GLuint unit = 0) const;
    GLuint getID() const;

private:
    std::unique_ptr<GLuint, std::function<void(GLuint*)>> ID;
    glm::ivec2 size;
    static GLuint boundID;
};

#endif // TEXTURE_HPP
