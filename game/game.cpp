#include <game/game.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <soundsystem.hpp>
#include <input.hpp>

Game::Game(const glm::vec2& fbSize):
    Menu(MenuName::Game)
{
    glm::vec2 projSize(640.f, 480.f);
    projection = glm::ortho(0.f, projSize.x, projSize.y, 0.f);
    assert(projSize.x / projSize.y == fbSize.x / fbSize.y);
}

void Game::processInput(const Input<int, std::hash<int>>& keys)
{
    if(keys.wasPressed(GLFW_KEY_ESCAPE))
    {
        new_menu = MenuName::Pause;
    }
    if(keys.wasPressed(GLFW_KEY_Q))
    {
        isDead = true;
        new_menu = MenuName::LoseScreen;
    }
    if(keys.wasPressed(GLFW_KEY_W))
    {
        isDead = true;
        new_menu = MenuName::WinScreen;
    }
}

void Game::render(Renderer_2D& renderer) const
{
    Sprite sprite;
    sprite.size = glm::vec2(400.f, 400.f);
    sprite.position = glm::vec2(100.f, 100.f);
    sprite.bloom = true;
    sprite.color = glm::vec4(0.f, 255.f, 127.f, 1.f);
    renderer.load_projection(projection);
    renderer.render(sprite);
}
