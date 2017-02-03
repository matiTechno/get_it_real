#include <renderer_2d.hpp>
#include <sprite.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <opengl/shader.hpp>
#include <vector>
#include <text.hpp>
#include <particle.hh>

FT_Library Renderer_2D::ftLib;

bool Renderer_2D::current = false;

Renderer_2D::Renderer_2D()
{
    assert(!current);
    current = true;

    FT_Error error = FT_Init_FreeType(&ftLib);
    if(error)
        throw std::runtime_error("[FreeType initialization error]");

    GLfloat vertices[] =
    {
        // pos....texCoord
        0.f, 0.f, 0.f, 0.f,
        1.f, 0.f, 1.f, 0.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        0.f, 1.f, 0.f, 1.f,
        0.f, 0.f, 0.f, 0.f
    };

    vbo.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vao.bind();
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glSamplerParameteri(sampler.getID(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler.getID(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler.getID(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler.getID(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    std::string vs_source(
                "#version 330\n"
                "layout (location = 0) in vec4 vertex;"
                "uniform mat4 model;\n"
                "uniform mat4 projection;\n"
                "uniform vec2 texSize;\n"
                "uniform vec2 texShift;\n"
                "out vec2 texCoord;\n"
                "void main()\n"
                "{\n"
                "gl_Position = projection * model * vec4(vertex.xy, 0, 1);\n"
                "texCoord = vertex.zw * texSize + texShift;"
                "}");

    {
        std::string fs_source(
                    "#version 330\n"
                    "in vec2 texCoord;\n"
                    "layout (location = 0) out vec4 color;\n"
                    "layout (location = 1) out vec4 color_bloom;\n"
                    "uniform vec4 spriteColor;\n"
                    "uniform sampler2D sampl;\n"
                    "uniform bool bloom;\n"
                    "void main()\n"
                    "{\n"
                    "color = texture(sampl, texCoord) * spriteColor;\n"
                    "if(bloom)"
                    "color_bloom = color;"
                    "else\n"
                    "color_bloom = vec4(0, 0, 0, 0);"
                    "}");

        shader_texture = std::make_unique<Shader>(vs_source, fs_source);
    }
    {
        std::string fs_source(
                    "#version 330\n"
                    "layout (location = 0) out vec4 color;\n"
                    "layout (location = 1) out vec4 color_bloom;\n"
                    "uniform vec4 spriteColor;\n"
                    "uniform bool bloom;\n"
                    "void main()\n"
                    "{\n"
                    "color = spriteColor;\n"
                    "if(bloom)"
                    "color_bloom = color;"
                    "else\n"
                    "color_bloom = vec4(0, 0, 0, 0);"
                    "}");

        shader_rect = std::make_unique<Shader>(vs_source, fs_source);

        {
            std::string fs_source(
                        "#version 330\n"
                        "in vec2 texCoord;\n"
                        "layout (location = 0) out vec4 color;\n"
                        "layout (location = 1) out vec4 color_bloom;\n"
                        "uniform vec4 spriteColor;\n"
                        "uniform sampler2D sampl;\n"
                        "uniform bool bloom;\n"
                        "void main()\n"
                        "{\n"
                        "color = vec4(1, 1, 1, texture(sampl, texCoord).r) * spriteColor;\n"
                        "if(bloom)"
                        "color_bloom = color;"
                        "else\n"
                        "color_bloom = vec4(0, 0, 0, 0);"
                        "}");

            shader_font = std::make_unique<Shader>(vs_source, fs_source);
        }
        {
            std::string vs_source(
                        "#version 330\n"
                        "layout (location = 0) in vec4 vertex;"
                        "layout (location = 1) in vec4 color;\n"
                        "layout (location = 2) in vec3 model;\n"
                        "uniform mat4 projection;\n"
                        "out vec4 spriteColor;\n"
                        "out vec2 texCoord;\n"
                        "void main()\n"
                        "{\n"
                        "gl_Position = projection * vec4(vertex.xy * model.z + model.xy, 0, 1);\n"
                        "texCoord = vertex.zw;"
                        "spriteColor = color;"
                        "}");

            std::string fs_source(
                        "#version 330\n"
                        "in vec2 texCoord;\n"
                        "in vec4 spriteColor;\n"
                        "layout (location = 0) out vec4 color;\n"
                        "layout (location = 1) out vec4 color_bloom;\n"
                        "uniform sampler2D sampl;\n"
                        "uniform bool bloom;\n"
                        "uniform bool texture_b;\n"
                        "void main()\n"
                        "{\n"
                        "if(texture_b)\n"
                        "color = texture(sampl, texCoord) * spriteColor;\n"
                        "else\n"
                        "color = spriteColor;\n"
                        "if(bloom)"
                        "color_bloom = color;"
                        "else\n"
                        "color_bloom = vec4(0, 0, 0, 0);"
                        "}");

            shader_particle = std::make_unique<Shader>(vs_source, fs_source);
        }
    }
}

void Renderer_2D::render(const Sprite& sprite) const
{
    vao.bind();

    glm::mat4 model;
    model = glm::translate(model, glm::vec3(sprite.position, 0.f));
    model = glm::rotate(model, sprite.rotation, glm::vec3(0.f, 0.f, -1.f));
    model = glm::scale(model, glm::vec3(sprite.size, 1.f));

    Shader* shader;
    if(sprite.texture)
    {
        shader = shader_texture.get();
        shader->bind();
        sampler.bind();
        sprite.texture->bind();

        auto coords = sprite.texCoords;
        decltype(auto) tex = sprite.texture;
        glm::vec2 texSize(GLfloat(coords.z)/GLfloat(tex->getSize().x), GLfloat(coords.w)/GLfloat(tex->getSize().y));
        glm::vec2 texShift(GLfloat(coords.x)/GLfloat(tex->getSize().x), GLfloat(coords.y)/GLfloat(tex->getSize().y));
        glUniform2f(shader->getUniLoc("texSize"), texSize.x, texSize.y);
        glUniform2f(shader->getUniLoc("texShift"), texShift.x, texShift.y);
    }
    else
    {
        shader = shader_rect.get();
        shader->bind();
    }

    glUniformMatrix4fv(shader->getUniLoc("model"), 1, GL_FALSE, &model[0][0]);
    auto col = sprite.color;
    if(sprite.bloom)
        glUniform1i(shader->getUniLoc("bloom"), true);
    else
        glUniform1i(shader->getUniLoc("bloom"), false);

    glUniform4f(shader->getUniLoc("spriteColor"), col.x/255.f, col.y/255.f, col.z/255.f, col.w);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer_2D::load_projection(const glm::mat4& projection)
{
    shader_texture->bind();
    glUniformMatrix4fv(shader_texture->getUniLoc("projection"), 1, GL_FALSE, &projection[0][0]);
    shader_rect->bind();
    glUniformMatrix4fv(shader_rect->getUniLoc("projection"), 1, GL_FALSE, &projection[0][0]);
    shader_font->bind();
    glUniformMatrix4fv(shader_font->getUniLoc("projection"), 1, GL_FALSE, &projection[0][0]);
    shader_particle->bind();
    glUniformMatrix4fv(shader_particle->getUniLoc("projection"), 1, GL_FALSE, &projection[0][0]);
}

void Renderer_2D::render(const Text& text) const
{
    shader_font->bind();
    sampler.bind();
    text.font->texture.bind();
    vao.bind();

    auto col = text.color;
    if(text.bloom)
        glUniform1i(shader_font->getUniLoc("bloom"), true);
    else
        glUniform1i(shader_font->getUniLoc("bloom"), false);

    glUniform4f(shader_font->getUniLoc("spriteColor"), col.x/255.f, col.y/255.f, col.z/255.f, col.w);

    glm::vec2 pen_pos = text.position;

    for(auto c: text.text)
    {
        if(c == '\n')
        {
            pen_pos.x = text.position.x;
            pen_pos.y += text.font->newLineSpace * text.scale;
            continue;
        }

        auto character = text.font->characters.at(c);

        glm::vec2 glyph_pos(pen_pos.x + character.bearing.x * text.scale,
                            pen_pos.y + (text.font->maxBearingY - character.bearing.y) * text.scale);
        glm::vec2 glyph_size(character.texCoords.z * text.scale, character.texCoords.w * text.scale);

        glm::mat4 model;
        model = glm::translate(model, glm::vec3(glyph_pos, 0.f));
        model = glm::scale(model, glm::vec3(glyph_size, 1.f));
        glUniformMatrix4fv(shader_font->getUniLoc("model"), 1, GL_FALSE, &model[0][0]);

        auto coords = character.texCoords;
        const Texture* tex = &text.font->texture;
        glm::vec2 texSize(GLfloat(coords.z)/GLfloat(tex->getSize().x), GLfloat(coords.w)/GLfloat(tex->getSize().y));
        glm::vec2 texShift(GLfloat(coords.x)/GLfloat(tex->getSize().x), GLfloat(coords.y)/GLfloat(tex->getSize().y));
        glUniform2f(shader_font->getUniLoc("texSize"), texSize.x, texSize.y);
        glUniform2f(shader_font->getUniLoc("texShift"), texShift.x, texShift.y);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        pen_pos.x += character.advanceX * text.scale;
    }
}

Renderer_2D::~Renderer_2D()
{
    FT_Done_FreeType(ftLib);
}

Font Renderer_2D::loadFont(const std::string& filename, unsigned size)
{
    assert(current);

    FT_Face face;
    FT_Error error = FT_New_Face(ftLib, filename.c_str(), 0, &face);
    if(error == FT_Err_Unknown_File_Format)
        throw std::runtime_error("[FreeType: font format unsupported <" + filename + ">]");
    else if(error)
        throw std::runtime_error("[FreeType: could not open file <" + filename + ">]");

    error = FT_Set_Pixel_Sizes(face, 0, size);
    if(error)
    {
        FT_Done_Face(face);
        throw std::runtime_error("[FreeType: could not set font size <" + filename + ">]");
    }

    float newLineSpace = face->size->metrics.height >> 6;

    FT_GlyphSlot slot = face->glyph;

    unsigned maxSizeY = 0, summX = 0;
    float maxBearingY = 0;

    for(std::size_t i = 32; i < 127; ++i)
    {
        assert(!FT_Load_Char(face, i, FT_LOAD_RENDER));

        // to avoid edge artifacts
        summX += slot->bitmap.width + 1;
        if(slot->bitmap.rows > maxSizeY)
            maxSizeY = slot->bitmap.rows;
        if(slot->bitmap_top > maxBearingY)
            maxBearingY = slot->bitmap_top;
    }

    GLint unpackAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::vector<char> black(maxSizeY * summX, 0);

    Texture texture(GL_R8, static_cast<GLsizei>(summX), static_cast<GLsizei>(maxSizeY));
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GLsizei(summX), GLsizei(maxSizeY), GL_RED, GL_UNSIGNED_BYTE, black.data());

    std::unordered_map<char, Character> characters;
    unsigned x = 0;
    for(std::size_t i = 32; i < 127; ++i)
    {
        FT_Load_Char(face, i, FT_LOAD_RENDER);

        glTexSubImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(x), 0,
                        static_cast<GLsizei>(slot->bitmap.width), static_cast<GLsizei>(slot->bitmap.rows),
                        GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);

        characters.emplace(static_cast<char>(i), Character{glm::vec4(x, 0.f, slot->bitmap.width, slot->bitmap.rows),
                                                           glm::vec2(slot->bitmap_left, slot->bitmap_top),
                                                           static_cast<float>(slot->advance.x >> 6)});

        // to avoid edge artifacts
        x += slot->bitmap.width + 1;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);
    FT_Done_Face(face);

    return Font{std::move(texture), newLineSpace, std::move(characters), maxBearingY};
}

void Renderer_2D::render(const ParticleGenerator& generator)
{
    generator.vboDynamic.bind(GL_ARRAY_BUFFER);

    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    static_cast<GLsizeiptr>(static_cast<std::size_t>(generator.lastActive + 1) * sizeof(Instance)),
                    generator.vboData.data());

    generator.vao.bind();
    sampler.bind();

    GLint dFactor, sFactor;
    glGetIntegerv(GL_BLEND_DST_ALPHA, &dFactor);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &sFactor);

    glBlendFunc(generator.sFactor, generator.dFactor);

    shader_particle->bind();
    if(generator.pData.texture)
    {
        generator.pData.texture->bind();
        glUniform1f(shader_particle->getUniLoc("texture_b"), true);
    }
    else
        glUniform1f(shader_particle->getUniLoc("texture_b"), false);

    if(generator.pData.bloom)
        glUniform1f(shader_particle->getUniLoc("bloom"), true);
    else
        glUniform1f(shader_particle->getUniLoc("bloom"), false);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, generator.lastActive + 1);

    glBlendFunc(static_cast<GLenum>(sFactor), static_cast<GLenum>(dFactor));
}
