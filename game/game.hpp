#ifndef GAME_HPP
#define GAME_HPP

#include <menu.hpp>
#include <opengl/texture.hpp>
#include <game/brick.hpp>
#include <vector>
#include <game/paddle.hpp>
#include <game/ball.hpp>
#include <random>
#include <particle.hh>
#include <game/healthbar.hpp>

class Game: public Menu
{
public:
    Game(const glm::vec2& fbSize);

    void processInput(const Input<int, std::hash<int>>& keys) override;
    virtual void update(float frameTime, PostProcessor& postProccesor) override;
    void render(Renderer_2D& renderer) override;

    glm::mat4 projection;
    Texture background_tex;
    Texture bricks_tex;
    Texture ball_tex;
    Texture wall_texture;
    Texture fire_tex;
    Texture heart_tex;
    Sprite background;
    float max_frame_time;
    float time_passed;
    float accumulator;
    float dt_update;
    float bottom_line;
    std::mt19937 rn_engine;

    // entities
    std::vector<Brick> bricks;
    Paddle paddle;
    Ball ball;
    std::unique_ptr<HealthBar> hp_bar;
    std::vector<ParticleGenerator> generators;
    std::vector<AnimEntity> animations;

    void update_logic(float dt_sec);
};

#endif // GAME_HPP
