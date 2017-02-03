#include <game/game.hpp>

void HealthBar::loseLife(Game& game)
{
    if(hearts.size())
    {
        PData pdata{true, glm::vec2(1, 3), glm::vec4(255.f, 0.f, 0.f, 0.2f), glm::vec4(255.f, 0.f, 0.f, 1.f),
                    glm::vec2(5.f, 8.f), &game.fire_tex, glm::vec4(0.f, -50.f, 0.f, -100.f), glm::vec4(10.f, 50.f, -10.f, 50.f),
                    nullptr};

        game.generators.emplace_back(0.0001f, hearts.back().position + hearts.back().size / 2.f, 1.f,
                                     glm::vec4(-10.f, -10.f, 20.f, 20.f),
                                     true, GL_SRC_ALPHA, GL_ONE, pdata);
        hearts.pop_back();
    }
}
