#ifndef POWERUP_HPP
#define POWERUP_HPP

#include <vector>
#include <game/animentity.hpp>
#include <renderer_2d.hpp>
#include <random>

class PowerUp: public AnimEntity
{
public:
    PowerUp(const glm::vec2& pos, Texture& tex, Animation& anim);
    bool isDead = false;
    float immuneTime = 0.2f; // decrease in update
private:
    void update_after_anim(float dt) override;
};

class PowerUp_System
{
public:
    void update(float dt);
    void render(Renderer_2D& rendr);

    std::vector<std::unique_ptr<PowerUp>> powerups;
};

#endif // POWERUP_HPP
