#ifndef RENDERER_2D_HPP
#define RENDERER_2D_HPP

struct Sprite;
class Text;
#include <glm/mat4x4.hpp>
class Shader;
#include <opengl/buffers.hpp>
#include <opengl/sampler.hpp>
#include <memory>
#include <ft2build.h>
#include <glm/vec2.hpp>
#include <opengl/texture.hpp>
#include <unordered_map>
#include FT_FREETYPE_H
class ParticleGenerator;

struct Character
{
    glm::vec4 texCoords;
    glm::vec2 bearing;
    float advanceX;
};

struct Font
{
    Texture texture;
    float newLineSpace;
    std::unordered_map<char, Character> characters;
    float maxBearingY;
};

class Renderer_2D
{
public:
    Renderer_2D();
    ~Renderer_2D();
    Renderer_2D(const Renderer_2D&) = delete;
    Renderer_2D& operator=(const Renderer_2D&) = delete;

    void render(const Sprite& sprite) const;
    void render(const Text& text) const;
    void render(const ParticleGenerator& generator);
    void load_projection(const glm::mat4& projection);

    static Font loadFont(const std::string& filename, unsigned size);

private:
    std::unique_ptr<Shader> shader_texture, shader_rect, shader_font, shader_particle;
    BO vbo;
    VAO vao;
    Sampler sampler;
    static FT_Library ftLib;
    static bool current;
};

#endif // RENDERER_2D_HPP
