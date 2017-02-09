#include <menu.hpp>
#include <input.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <postprocessor.hpp>
#include <soundsystem.hpp>

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

    PData pdata{true, glm::vec2(2, 5), glm::vec4(0.f, 0.f, 0.f, 1.f), glm::vec4(255.f, 255.f, 255.f, 1.f),
                glm::vec2(0.5f, 4.f), nullptr, glm::vec4(-50.f, 50.f, 50.f, -50.f), glm::vec4(-50.f, 50.f, 50.f, -50.f),
                nullptr};

    generator = std::make_unique<ParticleGenerator>(0.001f, fbSize/2.f, 3.f, glm::vec4(-50.f, -50.f, 100.f, 100.f),
                                                    true, GL_SRC_ALPHA, GL_ONE, pdata);
}

void Intro::processInput(const Input<int, std::hash<int>>& keys)
{
    if(keys.wasPressed(GLFW_KEY_ESCAPE) || keys.wasPressed(GLFW_KEY_SPACE) || keys.wasPressed(GLFW_KEY_ENTER))
    {
        intro_time_left = 0.f;
    }
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
        postProccesor.setWave(true);
    }
    else
    {
        postProccesor.setWave(false);
    }

    generator->update(frameTime);
    postProccesor.update(frameTime);
}

void Intro::render(Renderer_2D& renderer)
{
    renderer.load_projection(projection);
    renderer.render(*text);
    renderer.render(*generator);
}

MainMenu::MainMenu(const glm::vec2& fbSize):
    Menu(MenuName::MainMenu),
    projection(glm::ortho(0.f, fbSize.x, fbSize.y, 0.f)),
    font(Renderer_2D::loadFont("res/MotionControl-Bold.otf", 60)),
    current_option(0),
    tex("res/background_Jerom.png"),
    fbSize(fbSize)
{
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "start game"));
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "show intro"));
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
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
        SoundSystem::playSwitch();
    }
    if(keys.wasPressed(GLFW_KEY_S) || keys.wasPressed(GLFW_KEY_DOWN))
    {
        options[current_option]->bloom = false;
        if(current_option == options.size() - 1)
            current_option = 0;
        else
            ++current_option;
        SoundSystem::playSwitch();
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
        SoundSystem::playClick();
    }
    options[current_option]->bloom = true;
}

void MainMenu::render(Renderer_2D& renderer)
{
    renderer.load_projection(projection);
    Sprite sprite;
    sprite.color = glm::vec4(166.f, 67.f, 33.f, 0.15f);
    sprite.position = glm::vec2(0.f, 0.f);
    sprite.size = fbSize;
    sprite.texture = &tex;
    sprite.texCoords = glm::vec4(0.f, 0.f, tex.getSize());
    renderer.load_projection(projection);
    renderer.render(sprite);
    for(auto& option: options)
        renderer.render(*option);
}

Pause::Pause(const glm::vec2& fbSize):
    Menu(MenuName::Pause),
    projection(glm::ortho(0.f, fbSize.x, fbSize.y, 0.f)),
    font(Renderer_2D::loadFont("res/MotionControl-Bold.otf", 60)),
    current_option(0)
{
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "resume"));
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "new game"));
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
    sprite.color = glm::vec4(0.f, 0.f, 0.f, 0.7f);
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
        SoundSystem::playSwitch();
    }
    if(keys.wasPressed(GLFW_KEY_S) || keys.wasPressed(GLFW_KEY_DOWN))
    {
        options[current_option]->bloom = false;
        if(current_option == options.size() - 1)
            current_option = 0;
        else
            ++current_option;
        SoundSystem::playSwitch();
    }
    if(keys.wasPressed(GLFW_KEY_ENTER) || keys.wasPressed(GLFW_KEY_SPACE))
    {
        if(current_option == 0)
        {
            isDead = true;
            new_menu = MenuName::Game;
            SoundSystem::resume_game_sounds();
        }
        if(current_option == 1)
        {
            isDead = true;
            new_menu = MenuName::NewGame_from_pause;
            SoundSystem::stop_all_game_sounds();
        }
        else if(current_option == 2)
        {
            isDead = true;
            new_menu = MenuName::MainMenu;
            SoundSystem::stop_all_game_sounds();
        }
        else if(current_option == 3)
        {
            isDead = true;
            new_menu = MenuName::None;
            SoundSystem::stop_all_game_sounds();
        }
        SoundSystem::playClick();
    }
    if(keys.wasPressed(GLFW_KEY_ESCAPE))
    {
        isDead = true;
        new_menu = MenuName::Game;
        SoundSystem::resume_game_sounds();
    }

    options[current_option]->bloom = true;
}

void Pause::render(Renderer_2D& renderer)
{
    renderer.load_projection(projection);
    renderer.render(sprite);

    for(auto& option: options)
        renderer.render(*option);
}

NewGameMenu::NewGameMenu(const glm::vec2& fbSize):
    Menu(MenuName::NewGameMenu),
    projection(glm::ortho(0.f, fbSize.x, fbSize.y, 0.f)),
    font(Renderer_2D::loadFont("res/MotionControl-Bold.otf", 60)),
    tex("res/background_Jerom.png"),
    fbSize(fbSize),
    current_option(0)
{
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "play again"));
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
                                             font, 1.f, "main menu"));
    options.push_back(std::make_unique<Text>(glm::vec4(255.f, 128.f, 0.f, 1.f), false, glm::vec2(),
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

void NewGameMenu::processInput(const Input<int, std::hash<int>>& keys)
{
    if(keys.wasPressed(GLFW_KEY_W) || keys.wasPressed(GLFW_KEY_UP))
    {
        options[current_option]->bloom = false;
        if(current_option == 0)
            current_option = options.size() - 1;
        else
            --current_option;
        SoundSystem::playSwitch();
    }
    if(keys.wasPressed(GLFW_KEY_S) || keys.wasPressed(GLFW_KEY_DOWN))
    {
        options[current_option]->bloom = false;
        if(current_option == options.size() - 1)
            current_option = 0;
        else
            ++current_option;
        SoundSystem::playSwitch();
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
        SoundSystem::playClick();
    }
    options[current_option]->bloom = true;
}

void NewGameMenu::render(Renderer_2D& renderer)
{
    renderer.load_projection(projection);
    Sprite sprite;
    sprite.color = glm::vec4(166.f, 67.f, 33.f, 0.15f);
    sprite.position = glm::vec2(0.f, 0.f);
    sprite.size = fbSize;
    sprite.texture = &tex;
    sprite.texCoords = glm::vec4(0.f, 0.f, tex.getSize());
    renderer.render(sprite);
    for(auto& option: options)
        renderer.render(*option);
}

WinScreen::WinScreen(const glm::vec2& fbSize):
    Menu(MenuName::WinScreen),
    projection(glm::ortho(0.f, fbSize.x, fbSize.y, 0.f)),
    font(Renderer_2D::loadFont("res/DejaVuSans.ttf", 60)),
    scene_time(5.f),
    scene_time_left(scene_time)
{
    text = std::make_unique<Text>(glm::vec4(0.f, 0.f, 255.f, 0.f), true, glm::vec2(),
                                  font, 1.f, "You Win!");

    text->position = glm::vec2(fbSize.x / 2.f - text->getSize().x / 2.f, fbSize.y / 2.f - text->getSize().y / 2.f);

    PData pdata{true, glm::vec2(1, 3), glm::vec4(0.f, 0.f, 255.f, 1.f), glm::vec4(0.f, 0.f, 255.f, 1.f),
                glm::vec2(0.5f, 4.f), nullptr, glm::vec4(0.f, 100.f, 0.f, 100.f), glm::vec4(0.f, 0.f, 0.f, 0.f),
                nullptr};

    generator = std::make_unique<ParticleGenerator>(0.0005f, glm::vec2(0.f, 0.f), scene_time,
                                                    glm::vec4(0.f, -50.f, fbSize.x, fbSize.y + 50.f),
                                                    false, GL_SRC_ALPHA, GL_ONE, pdata);
}

void WinScreen::update(float frameTime, PostProcessor&)
{
    scene_time_left -= frameTime;
    if(scene_time_left < 0.f)
    {
        isDead = true;
        new_menu = MenuName::NewGameMenu;
    }

    generator->update(frameTime);

    text->color.a = (scene_time - scene_time_left) / scene_time;
}

void WinScreen::render(Renderer_2D& renderer)
{
    renderer.load_projection(projection);
    renderer.render(*generator);
    renderer.render(*text);
}

LoseScreen::LoseScreen(const glm::vec2& fbSize):
    Menu(MenuName::LoseScreen),
    projection(glm::ortho(0.f, fbSize.x, fbSize.y, 0.f)),
    font(Renderer_2D::loadFont("res/DejaVuSans.ttf", 60)),
    scene_time(5.f),
    scene_time_left(scene_time)
{
    text = std::make_unique<Text>(glm::vec4(255.f, 0.f, 0.f, 0.f), true, glm::vec2(),
                                  font, 1.f, "GAME OVER...");

    text->position = glm::vec2(fbSize.x / 2.f - text->getSize().x / 2.f, fbSize.y / 2.f - text->getSize().y / 2.f);

    PData pdata{true, glm::vec2(1, 3), glm::vec4(255.f, 0.f, 0.f, 1.f), glm::vec4(255.f, 0.f, 0.f, 1.f),
                glm::vec2(0.5f, 4.f), nullptr, glm::vec4(0.f, 100.f, 0.f, 100.f), glm::vec4(0.f, 0.f, 0.f, 0.f),
                nullptr};

    generator = std::make_unique<ParticleGenerator>(0.0005f, glm::vec2(0.f, 0.f), scene_time,
                                                    glm::vec4(0.f, -50.f, fbSize.x, fbSize.y + 50.f),
                                                    false, GL_SRC_ALPHA, GL_ONE, pdata);
}

void LoseScreen::update(float frameTime, PostProcessor&)
{
    scene_time_left -= frameTime;
    if(scene_time_left < 0.f)
    {
        isDead = true;
        new_menu = MenuName::NewGameMenu;
    }

    generator->update(frameTime);

    text->color.a = (scene_time - scene_time_left) / scene_time;
}

void LoseScreen::render(Renderer_2D& renderer)
{
    renderer.load_projection(projection);
    renderer.render(*generator);
    renderer.render(*text);
}
