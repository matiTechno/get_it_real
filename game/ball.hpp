#ifndef BALL_HPP
#define BALL_HPP

#include <game/entity.hpp>

class Ball: public Entity
{
    float getRadius()
    {
        return size.x / 2.f;
    }
};

#endif // BALL_HPP
