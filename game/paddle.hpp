#ifndef PADDLE_HPP
#define PADDLE_HPP

#include <game/entity.hpp>
#include <game/ball.hpp>
#include <game/collisions.hpp>

class Paddle: public Entity
{
public:
    float velMod;
    Ball* ball;
    float minReflectAngle;

    glm::vec2 prevPos;

    void update(float dt) override
    {
        prevPos = position;
        move(dt);
        if(ball->isStuck_)
        {
            glm::vec2 dPos = position - prevPos;
            ball->position += dPos;
        }
    }

    void reflectVEL(Dir pene);
};

#endif // PADDLE_HPP
