#include <postprocessor.hpp>
#include <string>

PostProcessor::PostProcessor(int width, int height):
    size(width, height),
    tex_base(GL_RGB8, width, height),
    tex_bright(GL_RGB8, width, height),
    tex_pp1(GL_RGB8, width, height),
    tex_pp2(GL_RGB8, tex_pp1.getSize().x, tex_pp1.getSize().y),
    tex_after_bloom(GL_RGB8, width, height)
{
    GLfloat vertices[] =
    {
        -1.f, -1.f, 0.f, 0.f,
        -1.f, 1.f, 0.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, -1.f, 1.f, 0.f,
        -1.f, -1.f, 0.f, 0.f
    };

    vbo.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vao.bind();
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    fb_1.bind(GL_FRAMEBUFFER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_base.getID(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_bright.getID(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    const GLenum attachments[2] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1
    };

    glDrawBuffers(2, attachments);

    std::string vs_source(
                "#version 330\n"
                "layout (location = 0) in vec4 vertex;\n"
                "out vec2 texCoord;\n"
                "void main()\n"
                "{\n"
                "gl_Position = vec4(vertex.xy, 0, 1);\n"
                "texCoord = vertex.zw;\n"
                "}");

    {
        std::string vs_source(
                    "#version 330\n"
                    "layout (location = 0) in vec4 vertex;\n"
                    "uniform bool shake;\n"
                    "uniform float time;\n"
                    "out vec2 texCoord;\n"
                    "void main()\n"
                    "{\n"
                    "gl_Position = vec4(vertex.xy, 0, 1);\n"
                    "if(shake)\n"
                    "{\n"
                    "gl_Position.xy += cos(10 * 3.14 * time) * 0.008;\n"
                    "}\n"
                    "texCoord = vertex.zw;\n"
                    "}");

        std::string fs_source(
                    "#version 330\n"
                    "out vec4 color;\n"
                    "in vec2 texCoord;\n"
                    "uniform sampler2D sampl;\n"
                    "uniform float time;\n"
                    "uniform bool shake;\n"
                    "uniform bool wave;\n"
                    "uniform float blackout_t;\n"
                    "void main()\n"
                    "{\n"
                    "vec2 texCoord_w = texCoord\n;"
                    "if(wave)\n"
                    "texCoord_w.x += sin(texCoord.y * 4 * 2 * 3.14 + 4 * 3.14 * time) / 100;\n"
                    "if(shake || blackout_t > 0)"
                    "{\n"
                    "vec2 texelSize = 1.0 / textureSize(sampl, 0);\n"
                    "vec3 samples[9];\n"
                    "int counter = 0;"
                    "for(int i = -1; i < 2; ++i)\n"
                    "for(int j = -1; j < 2; ++j)\n"
                    "{\n"
                    "samples[counter] = vec3(texture(sampl, texCoord_w + vec2(i, j) * texelSize));"
                    "++counter;"
                    "}\n"
                    "float blur_kernel[9] = float[](1, 1, 1, 1, 1, 1, 1, 1, 1);\n"
                    "color = vec4(0, 0, 0, 1);\n"
                    "for(int i = 0; i < 9; ++i)\n"
                    "color.rgb += samples[i] * blur_kernel[i] * 1.f / 9.f;"
                    "}\n"
                    "else\n"
                    "color = vec4(texture(sampl, texCoord_w).xyz, 1);\n"
                    "if(blackout_t > 0)\n"
                    "color.gb *= abs(cos(blackout_t * 3.14));"
                    "}");

        shader_final = std::make_unique<Shader>(vs_source, fs_source);
    }
    {
        std::string fs_source(
                    "#version 330\n"
                    "#define kSize 5\n"
                    "out vec4 color;\n"
                    "in vec2 texCoord;\n"
                    "uniform sampler2D sampl;\n"
                    "uniform bool horizontal\n;"
                    "uniform float weights[kSize] = float[](0.227027, 0.1945946, 0.1216216, 0.1, 0.06);\n"
                    "void main()\n"
                    "{\n"
                    "vec2 texelSize = 1.0 / textureSize(sampl, 0);\n"
                    "vec3 result = texture(sampl, texCoord).rgb * weights[0];\n"
                    "if(horizontal)\n"
                    "{\n"
                    "for(int i = 1; i < kSize; ++i)\n"
                    "{\n"
                    "result += texture(sampl, texCoord + vec2(texelSize.x * i, 0)).rgb * weights[i];\n"
                    "result += texture(sampl, texCoord - vec2(texelSize.x * i, 0)).rgb * weights[i];\n"
                    "}\n"
                    "}\n"
                    "else\n"
                    "{\n"
                    "for(int i = 1; i < kSize; ++i)\n"
                    "{\n"
                    "result += texture(sampl, texCoord + vec2(0, texelSize.y * i)).rgb * weights[i];\n"
                    "result += texture(sampl, texCoord - vec2(0, texelSize.y * i)).rgb * weights[i];\n"
                    "}\n"
                    "}\n"
                    "color = vec4(result, 1);\n"
                    "}");

        shader_blur = std::make_unique<Shader>(vs_source, fs_source);

        {
            std::string fs_source(
                        "#version 330\n"
                        "out vec4 color;\n"
                        "in vec2 texCoord;\n"
                        "uniform sampler2D base;\n"
                        "uniform sampler2D blur;\n"
                        "void main()\n"
                        "{\n"
                        "color = vec4(texture(base, texCoord).xyz, 1) + vec4(texture(blur, texCoord).xyz, 1);\n"
                        "}");

            shader_blend = std::make_unique<Shader>(vs_source, fs_source);
            shader_blend->bind();
            glUniform1i(shader_blend->getUniLoc("base"), 0);
            glUniform1i(shader_blend->getUniLoc("blur"), 1);
        }
    }

    glSamplerParameteri(sampler.getID(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler.getID(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler.getID(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler.getID(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    fb_pp1.bind(GL_FRAMEBUFFER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_pp1.getID(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    fb_pp2.bind(GL_FRAMEBUFFER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_pp2.getID(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    fb_after_bloom.bind(GL_FRAMEBUFFER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_after_bloom.getID(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::begRender() const
{
    fb_1.bind(GL_FRAMEBUFFER);
    glClear(GL_COLOR_BUFFER_BIT);
}
void PostProcessor::endRender() const
{
    glViewport(0, 0, tex_pp1.getSize().x, tex_pp1.getSize().y);

    bool horizontal = true, first_iter = true;
    int blur_count = 2;
    shader_blur->bind();
    sampler.bind();
    vao.bind();
    for(int i = 0; i < 2 * blur_count; ++i)
    {
        if(horizontal)
        {
            fb_pp1.bind(GL_FRAMEBUFFER);
            glUniform1i(shader_blur->getUniLoc("horizontal"), true);
            if(first_iter)
                tex_bright.bind();
            else
                tex_pp2.bind();
        }
        else
        {
            fb_pp2.bind(GL_FRAMEBUFFER);
            glUniform1i(shader_blur->getUniLoc("horizontal"), false);
            tex_pp1.bind();
        }

        horizontal = !horizontal;
        glDrawArrays(GL_TRIANGLES, 0, 6);
        if(first_iter)
            first_iter = false;
    }

    glViewport(0, 0, size.x, size.y);

    fb_after_bloom.bind(GL_FRAMEBUFFER);
    sampler.bind(1);
    shader_blend->bind();
    tex_base.bind();
    tex_pp2.bind(1);
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::render() const
{
    sampler.bind();
    shader_final->bind();
    tex_after_bloom.bind();
    vao.bind();

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void PostProcessor::update(float dt)
{
    static float time = 0.f;
    time += dt;
    shader_final->bind();
    glUniform1f(shader_final->getUniLoc("time"), time);
}

void PostProcessor::setWave(bool on)
{
    shader_final->bind();
    if(on)
        glUniform1f(shader_final->getUniLoc("wave"), true);
    else
        glUniform1f(shader_final->getUniLoc("wave"), false);
}

void PostProcessor::setShake(bool on)
{
    shader_final->bind();
    if(on)
        glUniform1f(shader_final->getUniLoc("shake"), true);
    else
        glUniform1f(shader_final->getUniLoc("shake"), false);
}

void PostProcessor::setBlackOut(float time)
{
    shader_final->bind();
    glUniform1f(shader_final->getUniLoc("blackout_t"), time);
}
