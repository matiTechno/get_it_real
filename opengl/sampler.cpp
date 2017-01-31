#include <opengl/sampler.hpp>

GLuint Sampler::boundID = 0;

Sampler::Sampler()
{
    auto deleter = [](GLuint* ID)
    {
        glDeleteSamplers(1, ID);
        delete ID;
    };

    ID = std::unique_ptr<GLuint, decltype(deleter)>(new GLuint, deleter);

    glGenSamplers(1, ID.get());
}

void Sampler::bind(GLenum unit) const
{
    if(*ID != boundID)
    {
        glBindSampler(unit, *ID);
        boundID = *ID;
    }
}

GLuint Sampler::getID() const
{
    return *ID;
}
