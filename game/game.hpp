#ifndef GAME_HPP
#define GAME_HPP

#include <menu.hpp>

class Game: public Menu
{
public:
    Game(const glm::vec2& fbSize);

    void processInput(const Input<int, std::hash<int>>& keys) override;
    void render(Renderer_2D& renderer) const override;

private:
    glm::mat4 projection;
};

#endif // GAME_HPP
