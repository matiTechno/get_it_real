#ifndef MENU_HPP
#define MENU_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <vector>
#include <text.hpp>
#include <renderer_2d.hpp>
#include <sprite.hpp>
#include <particle.hh>
class PostProcessor;
template<typename T, typename Hash>
class Input;

enum class MenuName
{
    None,
    Intro,
    MainMenu,
    Game,
    Pause,
    NewGameMenu,
    LoseScreen,
    WinScreen,
    NewGame_from_pause
};

class Menu
{
public:
    Menu(MenuName this_menu);
    virtual ~Menu() = default;

    virtual void processInput(const Input<int, std::hash<int>>& keys){(void)keys;}
    virtual void update(float frameTime, PostProcessor& postProccesor){(void)frameTime;(void)postProccesor;}
    virtual void render(Renderer_2D& renderer) = 0;
    bool popMenu() const
    {
        return isDead;
    }
    MenuName pushMenu()
    {
        MenuName cached = new_menu;
        new_menu = MenuName::None;
        return cached;
    }
    MenuName getType()
    {
        return this_menu;
    }

protected:
    bool isDead;
    MenuName new_menu;
    MenuName this_menu;
};

class Intro: public Menu
{
public:
    Intro(const glm::vec2& fbSize);

    void processInput(const Input<int, std::hash<int>>& keys) override;
    void update(float frameTime, PostProcessor& postProccesor) override;
    void render(Renderer_2D& renderer) override;

private:
    std::unique_ptr<Text> text;
    glm::mat4 projection;
    glm::vec2 fbSize;
    float max_scale, text_scaling_time, intro_time_left, max_intro_time;
    Font font;
    std::unique_ptr<ParticleGenerator> generator;
};

class MainMenu: public Menu
{
public:
    MainMenu(const glm::vec2& fbSize);

    void processInput(const Input<int, std::hash<int>>& keys) override;
    void render(Renderer_2D& renderer) override;

private:
    std::vector<std::unique_ptr<Text>> options;
    glm::mat4 projection;
    Font font;
    std::size_t current_option;
    Texture tex;
    glm::vec2 fbSize;
};

class Pause: public Menu
{
public:
    Pause(const glm::vec2& fbSize);

    void processInput(const Input<int, std::hash<int>>& keys) override;
    void render(Renderer_2D& renderer) override;

private:
    std::vector<std::unique_ptr<Text>> options;
    glm::mat4 projection;
    Font font;
    std::size_t current_option;
    glm::vec2 bigbb;
    Sprite sprite;
};

class NewGameMenu: public Menu
{
public:
    NewGameMenu(const glm::vec2& fbSize);

    void processInput(const Input<int, std::hash<int>>& keys) override;
    void render(Renderer_2D& renderer) override;

private:
    std::vector<std::unique_ptr<Text>> options;
    glm::mat4 projection;
    Font font;
    Texture tex;
    glm::vec2 fbSize;
    std::size_t current_option;
};

class WinScreen: public Menu
{
public:
    WinScreen(const glm::vec2& fbSize);

    void update(float frameTime, PostProcessor& postProccesor) override;
    void render(Renderer_2D& renderer) override;

private:
    std::unique_ptr<Text> text;
    glm::mat4 projection;
    Font font;
    std::unique_ptr<ParticleGenerator> generator;
    float scene_time, scene_time_left;
};

class LoseScreen: public Menu
{
public:
    LoseScreen(const glm::vec2& fbSize);

    void update(float frameTime, PostProcessor& postProccesor) override;
    void render(Renderer_2D& renderer) override;

private:
    std::unique_ptr<Text> text;
    glm::mat4 projection;
    Font font;
    std::unique_ptr<ParticleGenerator> generator;
    float scene_time, scene_time_left;
};

#endif // MENU_HPP
