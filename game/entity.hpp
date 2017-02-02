#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <sprite.hpp>

class Entity
{
private:
    Sprite sprite;

public:
    Entity():
        sprite(),
        texture(sprite.texture),
        texCoords(sprite.texCoords),
        size(0.f, 0.f),
        velocity(0.f, 0.f),
        glow(sprite.bloom),
        color(sprite.color)
    {}

    virtual ~Entity() = default;

    const Sprite& getSprite()
    {
        sprite.position = position;
        sprite.size = size;
        sprite.bloom = glow;
        sprite.color = color;
        sprite.texture = texture;
        sprite.texCoords = texCoords;

        return sprite;
    }

    virtual void update(float dt);

    Texture* texture;
    glm::vec4 texCoords;
    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 velocity;
    bool glow;
    glm::vec4 color;

protected:
    void move(float dt)
    {
        position += dt * velocity;
    }
};

#endif // ENTITY_HPP
