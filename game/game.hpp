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
#include <functional>
#include <game/powerup.hpp>

class Game: public Menu
{
public:
    Game(const glm::vec2& fbSize, PostProcessor& pp);

    void processInput(const Input<int, std::hash<int>>& keys) override;
    virtual void update(float frameTime, PostProcessor& postProccesor) override;
    void render(Renderer_2D& renderer) override;

    void doCollisions();

    void update_logic(float dt_sec);

    PostProcessor& pp;
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
    float bottom_line;
    std::mt19937 rn_engine;
    Animation ball_drop_anim;
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
    std::pair<std::size_t, float> combo_counter;
    bool start = true;
    float shake_time;
    std::unique_ptr<int, std::function<void(int*)>> pp_effects;
    float black_out_t;
    PowerUp_System pw_system;
    Texture tex_coin;
    Animation anim_coin;
    Texture tex_pw_ball;
    Animation anim_pw_ball;
    Animation heart_anim;
    Texture tex_pw_destro;
    Animation anim_pw_destro;
    Texture tex_candies;
    Animation anim_candie_orange;
    Texture tex_eyeball;
    Animation anim_eyeball;
    float ball_gen_nor_sp_time;
};

#endif // GAME_HPP
