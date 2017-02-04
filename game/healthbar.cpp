#include <game/game.hpp>

void HealthBar::loseLife(Game& game)
{
    if(hearts.size())
    {
        PData pdata{true, glm::vec2(2, 6), glm::vec4(120.f, 0.f, 0.f, 1.f), glm::vec4(255.f, 0.f, 0.f, 1.f),
                    glm::vec2(2.f, 4.f), &game.tex_gen_ball, glm::vec4(10.f, -20.f, -10.f, -60.f), glm::vec4(0.f, 50.f, 0.f, 50.f),
                    nullptr};

        game.generators.emplace_back(0.0005f, hearts.back().position + hearts.back().size / 2.f, 0.5f,
                                     glm::vec4(-10.f, -10.f, 20.f, 20.f),
                                     true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, pdata);
        hearts.pop_back();
    }
}
