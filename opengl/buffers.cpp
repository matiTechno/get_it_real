#include <opengl/buffers.hpp>

GLuint VAO::boundID = 0;

VAO::VAO()
{
    auto deleter = [](GLuint* ID)
    {
        glDeleteVertexArrays(1, ID);
        delete ID;
    };

    ID = std::unique_ptr<GLuint, decltype(deleter)>(new GLuint, deleter);

    glGenVertexArrays(1, ID.get());
}

void VAO::bind() const
{
    if(*ID != boundID)
    {
        glBindVertexArray(*ID);
        boundID = *ID;
    }
}

BO::BO()
{
    auto deleter = [](GLuint* ID)
    {
        glDeleteBuffers(1, ID);
        delete ID;
    };

    ID = std::unique_ptr<GLuint, decltype(deleter)>(new GLuint, deleter);

    glGenBuffers(1, ID.get());
}

void BO::bind(GLenum target) const
{
    glBindBuffer(target, *ID);
}

FrameBuffer::FrameBuffer()
{
    auto deleter = [](GLuint* ID)
    {
        glDeleteFramebuffers(1, ID);
        delete ID;
    };

    ID = std::unique_ptr<GLuint, decltype(deleter)>(new GLuint, deleter);

    glGenFramebuffers(1, ID.get());
}

void FrameBuffer::bind(GLenum target) const
{
    glBindFramebuffer(target, *ID);
}

RenderBuffer::RenderBuffer()
{
    auto deleter = [](GLuint* ID)
    {
        glDeleteRenderbuffers(1, ID);
        delete ID;
    };

    ID = std::unique_ptr<GLuint, decltype(deleter)>(new GLuint, deleter);

    glGenRenderbuffers(1, ID.get());
}

void RenderBuffer::bind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, *ID);
}

GLuint RenderBuffer::getID() const
{
    return *ID;
}
