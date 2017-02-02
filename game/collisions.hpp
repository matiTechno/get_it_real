#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <glm/vec2.hpp>
class Entity;
class Ball;

enum class Dir
{
    up,
    down,
    right,
    left
};

struct Collision
{
    bool isCollision;
    Dir penetration;
    glm::vec2 point;
};

// Collision::penetration is reserved for rect1
Collision rectRectCollision(const Entity& rect1, const Entity& rect2);
// ... for ball
Collision ballRectCollision(const Ball& ball, const Entity& rect);
void reflectVel(Ball& ball, Dir penDir);

#endif // COLLISIONS_HPP
