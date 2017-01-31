#include <menu.hpp>
#include <input.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <postprocessor.hpp>

Menu::Menu(MenuName this_menu):
    isDead(false),
    new_menu(MenuName::None),
    this_menu(this_menu)
{}

Intro::Intro(const glm::vec2& fbSize):
    Menu(MenuName::Intro),
    projection(glm::ortho(0.f, fbSize.x, fbSize.y, 0.f)),
    fbSize(fbSize),
    max_scale(20.f),
    text_scaling_time(3.f),
    intro_time_left(6.f),
    max_intro_time(intro_time_left),
    font(Renderer_2D::loadFont("res/DejaVuSans.ttf", 60))
{
    text = std::make_unique<Text>(glm::vec4(255.f, 255.f, 0.f, 1.f), true, glm::vec2(),
                                  font, max_scale, "I'm matimaci!\nOhaio.\nArkanoid demo...");
}

void Intro::processInput(const Input<int, std::hash<int>>& keys)
{
    if(keys.wasPressed(GLFW_KEY_ESCAPE) || keys.wasPressed(GLFW_KEY_SPACE) || keys.wasPressed(GLFW_KEY_ENTER))
        intro_time_left = 0.f;
}

void Intro::update(float frameTime, PostProcessor& postProccesor)
{
    intro_time_left -= frameTime;
    if(intro_time_left < 0.f)
    {
        isDead = true;
        new_menu = MenuName::MainMenu;
        postProccesor.setWave(false);
    }
    else if(max_intro_time - intro_time_left < text_scaling_time)
    {
        text->setScale(max_scale / (max_scale * (max_intro_time - intro_time_left) / text_scaling_time));
        text->position = glm::vec2(fbSize.x / 2.f - text->getSize().x / 2.f, fbSize.y / 2.f - text->getSize().y / 2.f);
    }
    else
    {
        postProccesor.setWave(true);
    }
}

void Intro::render(Renderer_2D& renderer) const
{
    renderer.load_projection(projection);
    renderer.render(*text);
}

MainMenu::MainMenu(const glm::vec2& fbSize):
    Menu(MenuName::MainMenu),
    projection(glm::ortho(0.f, fbSize.x, fbSize.y, 0.f)),
    font(Renderer_2D::loadFont("res/DejaVuSans.ttf", 60)),
    current_option(0)
{
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 0.f, 255.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "start game"));
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 0.f, 255.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "show intro"));
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 0.f, 255.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "quit"));

    float sizeY = 0.f;
    for(auto& option: options)
    {
        sizeY += option->getSize().y;
    }
    std::size_t count = 0;
    for(auto& option: options)
    {
        option->position = glm::vec2(fbSize.x / 2.f - option->getSize().x / 2.f,
                                     fbSize.y / 2.f - sizeY / 2.f + count * sizeY / options.size());
        ++count;
    }
}

void MainMenu::processInput(const Input<int, std::hash<int>>& keys)
{
    if(keys.wasPressed(GLFW_KEY_W) || keys.wasPressed(GLFW_KEY_UP))
    {
        options[current_option]->bloom = false;
        if(current_option == 0)
            current_option = options.size() - 1;
        else
            --current_option;
    }
    if(keys.wasPressed(GLFW_KEY_S) || keys.wasPressed(GLFW_KEY_DOWN))
    {
        options[current_option]->bloom = false;
        if(current_option == options.size() - 1)
            current_option = 0;
        else
            ++current_option;
    }
    if(keys.wasPressed(GLFW_KEY_ENTER) || keys.wasPressed(GLFW_KEY_SPACE))
    {
        if(current_option == 0)
        {
            isDead = true;
            new_menu = MenuName::Game;
        }
        else if(current_option == 1)
        {
            new_menu = MenuName::Intro;
        }
        else if(current_option == 2)
        {
            isDead = true;
            new_menu = MenuName::None;
        }
    }
    options[current_option]->bloom = true;
}

void MainMenu::render(Renderer_2D& renderer) const
{
    renderer.load_projection(projection);
    for(auto& option: options)
        renderer.render(*option);
}

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
        new_menu = MenuName::Pause;
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

Pause::Pause(const glm::vec2& fbSize):
    Menu(MenuName::Pause),
    projection(glm::ortho(0.f, fbSize.x, fbSize.y, 0.f)),
    font(Renderer_2D::loadFont("res/DejaVuSans.ttf", 60)),
    current_option(0)
{
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "resume"));
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "main menu"));
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "quit"));

    float sizeY = 0.f;
    for(auto& option: options)
        sizeY += option->getSize().y;

    std::size_t count = 0;
    for(auto& option: options)
    {
        option->position = glm::vec2(fbSize.x / 2.f - option->getSize().x / 2.f,
                                     fbSize.y / 2.f - sizeY / 2.f + count * sizeY / options.size());
        ++count;
    }

    float leftX = options[0]->position.x, rightX = leftX + options[0]->getSize().x;
    for(std::size_t i = 1; i < options.size(); ++i)
    {
        if(options[i]->position.x < leftX)
            leftX = options[i]->position.x;

        if(options[i]->position.x + options[i]->getSize().x > rightX)
            rightX = options[i]->position.x + options[i]->getSize().x;
    }

    bigbb = glm::vec2(rightX - leftX + 40.f, sizeY + 40.f);
    sprite.color = glm::vec4(0.f, 0.f, 0.f, 1.f);
    sprite.bloom = false;
    sprite.size = bigbb;
    sprite.position = glm::vec2(leftX - 20.f, options[0]->position.y - 20.f);
}

void Pause::processInput(const Input<int, std::hash<int>>& keys)
{
    if(keys.wasPressed(GLFW_KEY_W) || keys.wasPressed(GLFW_KEY_UP))
    {
        options[current_option]->bloom = false;
        if(current_option == 0)
            current_option = options.size() - 1;
        else
            --current_option;
    }
    if(keys.wasPressed(GLFW_KEY_S) || keys.wasPressed(GLFW_KEY_DOWN))
    {
        options[current_option]->bloom = false;
        if(current_option == options.size() - 1)
            current_option = 0;
        else
            ++current_option;
    }
    if(keys.wasPressed(GLFW_KEY_ENTER) || keys.wasPressed(GLFW_KEY_SPACE))
    {
        if(current_option == 0)
        {
            isDead = true;
            new_menu = MenuName::Game;
        }
        else if(current_option == 1)
        {
            isDead = true;
            new_menu = MenuName::MainMenu;
        }
        else if(current_option == 2)
        {
            isDead = true;
            new_menu = MenuName::None;
        }
    }
    options[current_option]->bloom = true;
}

void Pause::render(Renderer_2D& renderer) const
{
    renderer.load_projection(projection);
    renderer.render(sprite);

    for(auto& option: options)
        renderer.render(*option);
}
