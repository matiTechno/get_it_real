#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include <glad/glad.h>
#include <memory>
#include <functional>

class Sampler
{
public:
    Sampler();
    void bind(GLenum unit = 0) const;
    GLuint getID() const;

private:
    static GLuint boundID;
    std::unique_ptr<GLuint, std::function<void(GLuint*)>> ID;
};

#endif // SAMPLER_HPP
