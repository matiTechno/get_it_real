#ifndef HEALTHBAR_HPP
#define HEALTHBAR_HPP

#include <game/animentity.hpp>
#include <vector>
#include <renderer_2d.hpp>
class Game;

class HealthBar
{
public:
    HealthBar(const glm::vec2& position, AnimEntity& prototype):
        position(position),
        prototype(prototype)
    {}

    bool isDead() const
    {
        return !hearts.size();
    }

    void update(float dt)
    {
        for(auto& heart: hearts)
            heart.update(dt);
    }

    void addLife()
    {
        prototype.position = position + glm::vec2(float(hearts.size()) * (prototype.size.x + 10.f), 0.f);
        hearts.push_back(prototype);
    }

    void loseLife(Game& game);

    void render(const Renderer_2D& renderer)
    {
        for(auto& heart: hearts)
            renderer.render(heart.getSprite());
    }

private:
    glm::vec2 position;
    std::vector<AnimEntity> hearts;
    AnimEntity prototype;
};

#endif // HEALTHBAR_HPP
