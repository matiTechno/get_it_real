#include <game/game.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <soundsystem.hpp>
#include <input.hpp>

Game::Game(const glm::vec2& fbSize):
    Menu(MenuName::Game),
    background_tex("res/background_Jerom.png"),
    bricks_tex("res/breakout_pieces.png"),
    ball_tex("res/ball_tex.png"),
    wall_texture("res/sci-fi-platformer-tiles-32x32-extension.png"),
    fire_tex("res/explosion00.png"),
    heart_tex("res/heart.png"),
    max_frame_time(0.5f),
    time_passed(0.f),
    accumulator(0.f),
    dt_update(0.01666f)
{
    std::random_device rd;
    rn_engine.seed(rd());

    glm::vec2 projSize(640.f, 480.f);
    projection = glm::ortho(0.f, projSize.x, projSize.y, 0.f);
    assert(projSize.x / projSize.y == fbSize.x / fbSize.y);

    bottom_line = projSize.y;

    background.size = projSize;
    background.color = glm::vec4(166.f, 67.f, 33.f, 0.3f);
    background.texture = &background_tex;
    background.texCoords = glm::vec4(0.f, 0.f, background_tex.getSize().x, background_tex.getSize().y);

    // don't break brick order
    {
        Brick brick;
        brick.color.a = 0.7f;
        brick.texture = &wall_texture;
        brick.texCoords = glm::vec4(0, 1312, 64, 32);
        brick.position = glm::vec2(0.f, 0.f);
        brick.size = glm::vec2(projSize.x, 30.f);
        brick.b_type = Brick_type::solid;
        bricks.push_back(std::move(brick));
    }
    {
        Brick brick;
        brick.position = glm::vec2(0.f, bricks[0].size.y);
        brick.texture = &wall_texture;
        brick.color.a = 0.7f;
        brick.texCoords = glm::vec4(96, 1280, 32, 64);
        brick.size = glm::vec2(30.f, projSize.y - bricks[0].size.y);
        brick.b_type = Brick_type::solid;
        bricks.push_back(std::move(brick));
    }
    {
        Brick brick;
        brick.color.a = 0.7f;
        brick.position = glm::vec2(projSize.x - 30.f, bricks[0].size.y);
        brick.texture = &wall_texture;
        brick.texCoords = glm::vec4(96, 1280, 32, 64);
        brick.size = glm::vec2(30.f, projSize.y - bricks[0].size.y);
        brick.b_type = Brick_type::solid;
        bricks.push_back(std::move(brick));
    }

    paddle.texture = &bricks_tex;
    paddle.texCoords = glm::vec4(47, 112, 64, 16);
    paddle.size = glm::vec2(100.f, 30.f);
    paddle.position = glm::vec2(projSize.x / 2.f - paddle.size.x / 2.f, projSize.y - 80.f);

    ball.texture = &ball_tex;
    ball.texCoords = glm::vec4(0.f, 0.f, ball.texture->getSize().x, ball.texture->getSize().y);
    ball.color = glm::vec4(255.f, 0.f, 255.f, 1.f);
    ball.glow = true;
    ball.size = glm::vec2(20, 20);
    ball.position = glm::vec2(200.f, 200.f);

    glm::vec4 br_place(bricks[1].size.x, bricks[0].size.y,
            bricks[2].position.x - (bricks[1].size.x  - bricks[1].position.x), projSize.y / 2.5f);

    std::size_t bricks_x = 8;
    std::size_t bricks_y = 5;
    glm::vec2 br_size(br_place.z / bricks_x, br_place.w / bricks_y);

    std::uniform_int_distribution<int> uni(1, 10);

    for(std::size_t i = 0; i < bricks_x * bricks_y; ++i)
    {
        Brick brick;
        brick.texture = &bricks_tex;
        brick.size = br_size;
        brick.position = glm::vec2(br_place.x + (i % bricks_x) * br_size.x, br_place.y +  i / bricks_x * br_size.y);

        int num = uni(rn_engine);
        if(num < 7)
        {
            brick.b_type = Brick_type::one_hit;
            brick.texCoords = glm::vec4(8, 88, 32, 16);
        }
        else if(num < 9)
        {
            brick.b_type = Brick_type::solid;
            brick.texCoords = glm::vec4(264, 28, 32, 16);
        }
        else
        {
            brick.b_type = Brick_type::two_hit_effect;
            brick.texCoords = glm::vec4(8, 28, 32, 16);
        }
        bricks.push_back(std::move(brick));
    }
    {
        PData pdata{true, glm::vec2(5, 10), glm::vec4(255.f, 255.f, 255.f, 1.f), glm::vec4(255.f, 255.f, 255.f, 1.f),
                    glm::vec2(0.5f, 1.f), &fire_tex, glm::vec4(0.f, -10.f, 0.f, -50.f), glm::vec4(0.f, 0.f, 0.f, -10.f),
                    nullptr};

        generators.emplace_back(0.0001f, glm::vec2(bricks[1].size.x, projSize.y), -1000.f,
                glm::vec4(0.f, 0.f, projSize.x - 2 * bricks[1].size.x, projSize.y),
                false, GL_SRC_ALPHA, GL_ONE, pdata);
    }
    {
        Animation anim(1.5f, loadTexCoords("res/heart.sprites", std::vector<float>{0.4f, 0.1f, 0.1f}),
                       true, Origin::middle);

        AnimEntity anime(glm::vec2(0.f, 0.f), heart_tex, anim);
        anime.color.a = 0.7f;
        hp_bar = std::make_unique<HealthBar>(glm::vec2(bricks[1].size.x + 10.f, projSize.y - 20.f - anime.size.y), anime);
        hp_bar->addLife();
        hp_bar->addLife();
        hp_bar->addLife();
    }
}

void Game::processInput(const Input<int, std::hash<int>>& keys)
{
    if(keys.wasPressed(GLFW_KEY_ESCAPE))
    {
        new_menu = MenuName::Pause;
    }
    if(keys.wasPressed(GLFW_KEY_Q))
    {
        hp_bar->loseLife(*this);
    }
    if(keys.wasPressed(GLFW_KEY_W))
    {
        isDead = true;
        new_menu = MenuName::WinScreen;
    }
    if(keys.wasPressed(GLFW_KEY_A) || keys.isPressed(GLFW_KEY_A)
            || keys.wasPressed(GLFW_KEY_LEFT) || keys.isPressed(GLFW_KEY_LEFT))
    {
        // move left
    }
    if(keys.wasPressed(GLFW_KEY_D) || keys.isPressed(GLFW_KEY_D)
            || keys.wasPressed(GLFW_KEY_RIGHT) || keys.isPressed(GLFW_KEY_RIGHT))
    {
        // move right
    }
    if(keys.wasPressed(GLFW_KEY_SPACE) || keys.isPressed(GLFW_KEY_SPACE))
    {
        // release ball
    }
}

void Game::update_logic(float dt_sec)
{
    //1
    for(auto& generator: generators)
    {
        generator.update(dt_sec);
    }
    generators.erase(std::remove_if(generators.begin(), generators.end(),
                                    [](ParticleGenerator& gen){return !gen.isActive();}), generators.end());
    //2
    hp_bar->update(dt_sec);

    if(hp_bar->isDead())
    {
        isDead = true;
        new_menu = MenuName::LoseScreen;
    }

    //3
    for(auto& anim: animations)
        anim.update(dt_sec);

    animations.erase(std::remove_if(animations.begin(), animations.end(),
                                    [](AnimEntity& anim){
                         if(anim.anim.getState() == State::end)
                         return true;
                         return false;
                     }), animations.end());
}

void Game::render(Renderer_2D& renderer)
{
    renderer.load_projection(projection);
    renderer.render(background);
    for(auto& brick: bricks)
        renderer.render(brick.getSprite());
    renderer.render(paddle.getSprite());
    renderer.render(ball.getSprite());
    for(auto& generator: generators)
        renderer.render(generator);
    hp_bar->render(renderer);
    for(auto& anim: animations)
        renderer.render(anim.getSprite());
}

void Game::update(float frameTime, PostProcessor&)
{
    time_passed += frameTime;
    if(frameTime > max_frame_time)
        frameTime = max_frame_time;
    accumulator += frameTime;
    while(accumulator >= dt_update)
    {
        update_logic(dt_update);
        accumulator -= dt_update;
    }
}
