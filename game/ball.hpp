#ifndef BALL_HPP
#define BALL_HPP

#include <game/entity.hpp>
class Paddle;

class Ball: public Entity
{
public:
    Ball();

    float getRadius() const
    {
        return size.x / 2.f;
    }

    float isImmune_to_paddle()
    {
        return isImmune_to_paddle_ >= 0.f;
    }

    void reset();

    Paddle* paddle;

    bool isStuck_;
    float isImmune_to_paddle_;

    void update(float dt) override
    {
        if(!isStuck_)
            move(dt);
        if(isImmune_to_paddle_ > 0.f)
            isImmune_to_paddle_ -= dt;
    }
};

#endif // BALL_HPP
