#include <app.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <string>
#include <chrono>
#include <renderer_2d.hpp>
#include <postprocessor.hpp>
#include <soundsystem.hpp>
#include <game/game.hpp>

void error_callback(int error, const char* description)
{
    std::cout << "[error: " << error << ", " << description << ']' << std::endl;
}

void App::key_callback(GLFWwindow*, int key, int, int action, int)
{
    if(action == GLFW_PRESS)
        keys.pressEvent(key);
    else if(action == GLFW_RELEASE)
        keys.releaseEvent(key);
}

void App::window_focus_callback(GLFWwindow*, int focused)
{
    if(!focused)
    {
        if(menus.back()->getType() == MenuName::Game)
            menus.push_back(std::make_unique<Pause>(fbSize));
    }
}

bool App::current = false;
GLFWwindow* App::window;
std::unique_ptr<Renderer_2D> App::renderer;
std::unique_ptr<PostProcessor> App::postProcessor;
Input<int, std::hash<int>> App::keys;
std::vector<std::unique_ptr<Menu>> App::menus;
glm::vec2 App::fbSize;

// sound xd
std::unique_ptr<sf::SoundBuffer> SoundSystem::click_buffer, SoundSystem::switch_buffer;
std::unique_ptr<sf::Sound> SoundSystem::click_sound, SoundSystem::switch_sound;
std::unique_ptr<sf::Music> SoundSystem::intro_music;

App::App()
{
    assert(!current);
    current = true;

    glfwSetErrorCallback(error_callback);

    std::cout << "glfw compile time info: " << glfwGetVersionString() << std::endl;

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    std::string version = std::to_string(major) + '.' + std::to_string(minor) + '.' + std::to_string(revision);
    std::cout << "runtime glfw version: " + version << std::endl;

    if(!glfwInit())
        throw std::runtime_error("[glfw initialization failed]");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(640, 480, "get_it_real", nullptr, nullptr);

    if(!window)
        throw std::runtime_error("[glfw window creation failed]");

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    int win_size_x, win_size_y;
    glfwGetWindowSize(window, &win_size_x, &win_size_y);

    glfwSetWindowPos(window, mode->width / 2 - win_size_x / 2,
                     mode->height / 2 - win_size_y / 2);

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("[glad initialization failed]");

    std::cout << "vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "gl version: " << glGetString(GL_VERSION) << std::endl;

    glfwSwapInterval(0);

    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer = std::make_unique<Renderer_2D>();

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    postProcessor = std::make_unique<PostProcessor>(fb_width, fb_height);

    fbSize = glm::vec2(fb_width, fb_height);

    SoundSystem::init();

    menus.push_back(std::make_unique<Intro>(fbSize));

    run();
}

App::~App()
{
    glfwTerminate();
}

void App::run()
{
    auto currentTime = std::chrono::high_resolution_clock::now();

    while(!glfwWindowShouldClose(window))
    {
        processInput();

        if(!handleMenus())
            continue;

        auto newTime = std::chrono::high_resolution_clock::now();
        auto frameTime = std::chrono::duration_cast<std::chrono::duration<float>>(newTime - currentTime).count();
        currentTime = newTime;
        static float accTime = 0.f;
        static int frameCount = 0;
        accTime += frameTime;
        ++frameCount;
        if(accTime > 1.f)
        {
            std::cout << "frame time (s): " << accTime / static_cast<float>(frameCount) << '\r';
            std::cout.flush();
            accTime = 0.f;
            frameCount = 0;
        }

        update(frameTime);

        render();
    }
}

void App::processInput()
{
    keys.begin_new_frame();
    glfwPollEvents();
    menus.back()->processInput(keys);
}

bool App::handleMenus()
{
    MenuName new_menu = menus.back()->pushMenu();
    if(menus.back()->popMenu())
    {
        if(menus.back()->getType() == MenuName::Pause && new_menu != MenuName::Game)
        {
            menus.pop_back();
            menus.pop_back();
        }
        else
            menus.pop_back();
    }

    if(!menus.empty())
        if(menus.back()->getType() == new_menu)
            return true;

    switch(new_menu)
    {
    case MenuName::None:
        break;
    case MenuName::Intro: menus.push_back(std::make_unique<Intro>(fbSize));
        break;
    case MenuName::MainMenu: menus.push_back(std::make_unique<MainMenu>(fbSize));
        break;
    case MenuName::Game: menus.push_back(std::make_unique<Game>(fbSize));
        break;
    case MenuName::Pause: menus.push_back(std::make_unique<Pause>(fbSize));
        break;
    case MenuName::NewGameMenu: menus.push_back(std::make_unique<NewGameMenu>(fbSize));
        break;
    case MenuName::LoseScreen: menus.push_back(std::make_unique<LoseScreen>(fbSize));
        break;
    case MenuName::WinScreen: menus.push_back(std::make_unique<WinScreen>(fbSize));
        break;
    }
    if(menus.empty())
    {
        glfwSetWindowShouldClose(window, 1);
        return false;
    }
    return true;
}

void App::update(float frameTime)
{
    menus.back()->update(frameTime, *postProcessor);
    postProcessor->update(frameTime);
}

void App::render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    postProcessor->begRender();

    if(menus.back()->getType() == MenuName::Pause)
    {
        auto it = menus.end();
        it -= 2;
        (*it)->render(*renderer);
    }
    menus.back()->render(*renderer);

    postProcessor->endRender();
    postProcessor->render();

    glfwSwapBuffers(window);
}
