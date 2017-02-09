#ifndef POWERUP_HPP
#define POWERUP_HPP

#include <vector>
#include <game/animentity.hpp>
#include <renderer_2d.hpp>
#include <random>

enum class PW_Type
{
    pene,
    hp,
    ball_speed,
    clear,
    rain
};

class PowerUp: public AnimEntity
{
public:
    PowerUp(const glm::vec2& pos, Texture& tex, Animation& anim, PW_Type);
    bool isDead = false;
    float immuneTime = 0.3f;
    PW_Type type;
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
