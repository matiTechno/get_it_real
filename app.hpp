#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <input.hpp>
#include <vector>
#include <menu.hpp>
struct GLFWwindow;
class Renderer_2D;
class PostProcessor;

class App
{
public:
    App();
    ~App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

private:
    static bool current;

    static void run();
    static void processInput();
    // false if no menu left
    static bool handleMenus();
    static void update(float frameTime);
    static void render();

    static GLFWwindow* window;
    static std::unique_ptr<Renderer_2D> renderer;
    static std::unique_ptr<PostProcessor> postProcessor;
    static Input<int, std::hash<int>> keys;
    static std::vector<std::unique_ptr<Menu>> menus;
    static glm::vec2 fbSize;

    static void key_callback(GLFWwindow*, int key, int, int action, int);
    static void window_focus_callback(GLFWwindow*, int focused);
};

#endif // APP_HPP
