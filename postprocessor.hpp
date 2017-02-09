#ifndef POSTPROCESSOR_HPP
#define POSTPROCESSOR_HPP

#include <opengl/buffers.hpp>
#include <opengl/texture.hpp>
#include <opengl/shader.hpp>
#include <opengl/sampler.hpp>
#include <memory>

class PostProcessor
{
public:
    PostProcessor(int width, int height);

    void begRender() const;
    void endRender() const;
    void render() const;
    void update(float dt);

    void setWave(bool on);
    void setShake(bool on);
    // when time < 0 this effect is not used
    void setBlackOut(float time);

private:
    VAO vao;
    BO vbo;
    Sampler sampler;
    glm::ivec2 size;
    std::unique_ptr<Shader> shader_final, shader_blur, shader_blend;
    // framebuffers and textures
    FrameBuffer fb_1;
    // pp = ping pong
    FrameBuffer fb_pp1, fb_pp2;
    FrameBuffer fb_after_bloom;
    Texture tex_base;
    Texture tex_bright;
    Texture tex_pp1, tex_pp2;
    Texture tex_after_bloom;
};

#endif // POSTPROCESSOR_HPP
