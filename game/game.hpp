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
#include <game/en_text.hpp>

class Game: public Menu
{
public:
    Game(const glm::vec2& fbSize);

    void processInput(const Input<int, std::hash<int>>& keys) override;
    virtual void update(float frameTime, PostProcessor& postProccesor) override;
    void render(Renderer_2D& renderer) override;

    void doCollisions();

    glm::vec2 projSize;
    glm::mat4 projection;
    Texture background_tex;
    Texture bricks_tex;
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
    Animation ball_drop_anim;

    // entities
    std::vector<Brick> bricks;
    Paddle paddle;
    Ball ball;
    std::unique_ptr<HealthBar> hp_bar;
    std::vector<ParticleGenerator> generators;
    std::vector<AnimEntity> animations;
    Texture tex_hit_bott;
    Animation ball_hit_paddle_anim;
    Texture tex_paddle_hit;
    Texture more_br_pieces;
    Texture tex_gen_ball;
    std::unique_ptr<ParticleGenerator> ball_parti;
    Animation brick_explo21;
    Texture explo21;
    Font font;
    std::vector<std::unique_ptr<Text_Entity>> t_entities;

    void update_logic(float dt_sec);
};

#endif // GAME_HPP
