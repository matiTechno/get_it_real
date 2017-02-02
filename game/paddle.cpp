#include <game/paddle.hpp>
#include <glm/glm.hpp>

void Paddle::reflectVEL(Dir pene)
{
    if(ball->position.y + ball->getRadius() < position.y + size.y/2.f)
    {
        // P - paddle center
        // B - ball center
        float P = position.x + size.x/2.f;
        float B = ball->position.x + ball->getRadius();
        float reflectCoeff = glm::clamp((B - P)/(size.x/2.f), -1.f, 1.f);

        glm::vec2 ballVel = ball->velocity;
        float ballLenVel = glm::length(ballVel);

        float tanPow2 = glm::pow(glm::tan(minReflectAngle), 2.f);
        ballVel.x = glm::sqrt(glm::pow(ballLenVel, 2.f)/(tanPow2 + 1.f))
                * reflectCoeff;

        ballVel.y = -glm::sqrt(glm::pow(ballLenVel, 2.f) - glm::pow(ballVel.x, 2.f));

        ball->velocity = ballVel;
    }
    else
        reflectVel(*ball, pene);
}
