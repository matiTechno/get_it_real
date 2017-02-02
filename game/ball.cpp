#include <game/ball.hpp>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <game/paddle.hpp>

Ball::Ball():
    isStuck_(true),
    isImmune_to_paddle_(0.f)
{

}

void Ball::reset()
{
    position = glm::vec2(paddle->position.x + paddle->size.x/2.f - getRadius(),
                         paddle->position.y - size.y);

    isStuck_ = true;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(0, 1);
    int randomInt = uni(rng);

    glm::vec2 vel_t = velocity;
    float lenVel = glm::length(vel_t);

    float tanPow2 = glm::pow(glm::tan(glm::pi<float>()/4.f), 2.f);
    vel_t.x = glm::sqrt(glm::pow(lenVel, 2.f)/(tanPow2 + 1.f));

    if(randomInt == 1)
        vel_t.x = -vel_t.x;

    vel_t.y = -glm::sqrt(glm::pow(lenVel, 2.f) - glm::pow(vel_t.x, 2.f));

    velocity = vel_t;
}
