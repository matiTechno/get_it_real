#ifndef POWERUP_HPP
#define POWERUP_HPP

#include <vector>
#include <game/animentity.hpp>
#include <renderer_2d.hpp>

struct PWEffect
{
    bool isActive;
};

struct PowerUp: public AnimEntity
{
    bool isDead;
    float immuneTime; // decrease in update
};

class PowerUp_System
{
public:
    void update(float dt);
    void render(Renderer_2D& rendr);

    std::vector<PowerUp> powerups;

private:

};

#endif // POWERUP_HPP
