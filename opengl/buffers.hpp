#ifndef BUFFERS_HPP
#define BUFFERS_HPP

#include <glad/glad.h>
#include <memory>
#include <functional>

class VAO
{
public:
    VAO();
    void bind() const;

private:
    static GLuint boundID;
    std::unique_ptr<GLuint, std::function<void(GLuint*)>> ID;
};

class BO
{
public:
    BO();
    void bind(GLenum target) const;

private:
    std::unique_ptr<GLuint, std::function<void(GLuint*)>> ID;
};

class FrameBuffer
{
public:
    FrameBuffer();
    void bind(GLenum target) const;

private:
    std::unique_ptr<GLuint, std::function<void(GLuint*)>> ID;
};

class RenderBuffer
{
public:
    RenderBuffer();
    void bind() const;
    GLuint getID() const;

private:
    std::unique_ptr<GLuint, std::function<void(GLuint*)>> ID;
};

#endif // BUFFERS_HPP
