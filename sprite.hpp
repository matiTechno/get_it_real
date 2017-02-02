#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
class Texture;

struct Sprite
{
    Sprite():
        position(0.f, 0.f),
        rotation(0.f),
        color(255.f, 255.f, 255.f, 1.f),
        texture(nullptr),
        bloom(false)
    {}

    // top left corner
    glm::vec2 position;
    glm::vec2 size;
    // in radians
    float rotation;
    glm::vec4 color;
    // xy - top left corner, zw - size
    glm::vec4 texCoords;
    Texture* texture;
    bool bloom;
};

#endif // SPRITE_HPP
