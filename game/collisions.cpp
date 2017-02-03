#include <game/collisions.hpp>
#include <glm/glm.hpp>
#include <game/entity.hpp>
#include <game/ball.hpp>

Dir vecDir(const glm::vec2& vec)
{
    glm::vec2 dirs[] =
    {
        glm::vec2(0.f, -1.f),
        glm::vec2(0.f, 1.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(-1.f, 0.f)
    };

    int dir = 0;
    float max = 0.f;

    for(int i = 0; i < 4; ++i)
    {
        float dotProduct = glm::dot(glm::normalize(vec), dirs[i]);
        if(dotProduct > max)
        {
            max = dotProduct;
            dir = i;
        }
    }
    return static_cast<Dir>(dir);
}

Collision rectRectCollision(const Entity& rect1, const Entity& rect2)
{
    bool x = rect1.position.x + rect1.size.x > rect2.position.x
            &&
            rect2.position.x + rect2.size.x > rect1.position.x;

    bool y = rect1.position.y + rect1.size.y > rect2.position.y
            &&
            rect2.position.y + rect2.size.y > rect1.position.y;

    // R1 - rect1 center
    // R2 - rect2 center
    // P - closest point from rect2 to rect1

    glm::vec2 R1 = rect1.position + rect1.size/2.f;
    glm::vec2 R2 = rect2.position + rect2.size/2.f;
    glm::vec2 R2R1 = R1 - R2;
    glm::vec2 R2P = glm::clamp(R2R1, -rect2.size/2.f, rect2.size/2.f);
    glm::vec2 P = R2 + R2P;
    glm::vec2 R1P = P - R1;

    // vec_pene not implemented now (no need)
    return Collision{x && y, vecDir(R1P), P, glm::vec2(0.f, 0.f)};
}

Collision ballRectCollision(const Ball& ball, const Entity& rect)
{
    // B - ball center
    // R - rect center
    // P - closest point from rect to circle

    glm::vec2 B = ball.position + ball.getRadius();
    glm::vec2 R = rect.position + rect.size/2.f;
    glm::vec2 RB = B - R;
    glm::vec2 RP = glm::clamp(RB, -rect.size/2.f, rect.size/2.f);
    glm::vec2 P = R + RP;
    glm::vec2 BP = P - B;

    return Collision{glm::length(BP) < ball.getRadius(), vecDir(BP), P, glm::normalize(BP) * (ball.getRadius() - glm::length(BP))};
}

void reflectVel(Ball& ball, Dir penDir)
{
    glm::vec2 vel = ball.velocity;
    switch(penDir)
    {
    case Dir::up:
        ball.velocity = glm::vec2(vel.x, glm::abs(vel.y));
        break;
    case Dir::down:
        ball.velocity = glm::vec2(vel.x, -glm::abs(vel.y));
        break;
    case Dir::right:
        ball.velocity = glm::vec2(-glm::abs(vel.x), vel.y);
        break;
    case Dir::left:
        ball.velocity = glm::vec2(glm::abs(vel.x), vel.y);
    }
}
