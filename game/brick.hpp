#ifndef BRICK_HPP
#define BRICK_HPP

#include <game/entity.hpp>

enum class Brick_type
{
    wall,
    gone,
    one_hit,
    solid,
    two_hit_effect
};

class Brick: public Entity
{
public:
    Brick_type b_type;
};

#endif // BRICK_HPP
