#include <game/game.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <soundsystem.hpp>
#include <input.hpp>
#include <game/collisions.hpp>
#include <postprocessor.hpp>

Game::Game(const glm::vec2& fbSize, PostProcessor& pp):
    Menu(MenuName::Game),
    pp(pp),
    background_tex("res/background_Jerom.png"),
    bricks_tex("res/breakout_pieces.png"),
    wall_texture("res/sci-fi-platformer-tiles-32x32-extension.png"),
    fire_tex("res/explosion00.png"),
    heart_tex("res/heart.png"),
    max_frame_time(0.03f),
    time_passed(0.f),
    //accumulator(0.f),
    //dt_update(0.01672f),
    ball_drop_anim(1.f, loadTexCoords("res/effect_hit_bott.sprites", 0.03f), false, Origin::bottom),
    tex_hit_bott("res/effect_hit_bott.png"),
    ball_hit_paddle_anim(0.75f, loadTexCoords("res/effect_explo1.sprites", 0.06f), false, Origin::middle),
    tex_paddle_hit("res/effect_explo1.png"),
    more_br_pieces("res/more_breakout_pieces.png"),
    tex_gen_ball("res/smoke.png"),
    brick_explo21(1.f, loadTexCoords("res/explosion21.sprites", 0.05f), false, Origin::middle),
    explo21("res/Explosion21.png"),
    font(Renderer_2D::loadFont("res/MotionControl-Bold.otf", 40)),
    shake_time(0.f),
    black_out_t(0.f)
{
    std::random_device rd;
    rn_engine.seed(rd());

    auto effect_cancel = [this](int* i)
    {
        this->pp.setShake(false);
        this->pp.setWave(false);
        this->pp.setBlackOut(0.f);
        delete i;
    };
    pp_effects = std::unique_ptr<int, decltype(effect_cancel)>(new int, effect_cancel);

    combo_counter.first = 0;
    combo_counter.second = 0.f;

    glm::vec2 projSize(640.f, 480.f);
    this->projSize = projSize;
    projection = glm::ortho(0.f, projSize.x, projSize.y, 0.f);
    assert(projSize.x / projSize.y == fbSize.x / fbSize.y);

    bottom_line = projSize.y;

    background.size = projSize;
    background.color = glm::vec4(166.f, 67.f, 33.f, 0.2f);
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
        brick.b_type = Brick_type::wall;
        bricks.push_back(std::move(brick));
    }
    {
        Brick brick;
        brick.position = glm::vec2(0.f, bricks[0].size.y);
        brick.texture = &wall_texture;
        brick.color.a = 0.7f;
        brick.texCoords = glm::vec4(96, 1280, 32, 64);
        brick.size = glm::vec2(30.f, projSize.y - bricks[0].size.y);
        brick.b_type = Brick_type::wall;
        bricks.push_back(std::move(brick));
    }
    {
        Brick brick;
        brick.color.a = 0.7f;
        brick.position = glm::vec2(projSize.x - 30.f, bricks[0].size.y);
        brick.texture = &wall_texture;
        brick.texCoords = glm::vec4(96, 1280, 32, 64);
        brick.size = glm::vec2(30.f, projSize.y - bricks[0].size.y);
        brick.b_type = Brick_type::wall;
        bricks.push_back(std::move(brick));
    }

    paddle.texture = &bricks_tex;
    paddle.texCoords = glm::vec4(47, 112, 64, 16);
    paddle.size = glm::vec2(100.f, 30.f);
    paddle.position = glm::vec2(projSize.x / 2.f - paddle.size.x / 2.f, projSize.y - 80.f);
    paddle.minReflectAngle = glm::pi<float>() / 6.f;
    paddle.velMod = 220.f;

    ball.texture = &more_br_pieces;
    ball.paddle = &paddle;
    ball.texCoords = glm::vec4(784, 608, 16, 16);
    ball.color = glm::vec4(255.f, 0.f, 255.f, 1.f);
    ball.glow = true;
    ball.size = glm::vec2(20, 20);
    ball.velocity = glm::vec2(0.f, 200.f);
    ball.reset();

    paddle.ball = &ball;

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
        else if(num < 8)
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
    // fire glow base
    {
        PData pdata{true, glm::vec2(2, 8), glm::vec4(255.f, 255.f, 255.f, 0.2f), glm::vec4(255.f, 255.f, 255.f, 1.f),
                    glm::vec2(0.5f, 1.f), &fire_tex, glm::vec4(0.f, -10.f, 0.f, -50.f), glm::vec4(0.f, 0.f, 0.f, -10.f),
                    nullptr};

        generators.emplace_back(0.004f, glm::vec2(bricks[1].size.x, projSize.y), -1000.f,
                glm::vec4(0.f, 0.f, projSize.x - 2 * bricks[1].size.x, 0.f),
                false, GL_SRC_ALPHA, GL_ONE, pdata);
    }
    // fire flying particles
    {
        PData pdata{true, glm::vec2(3.f, 7.f), glm::vec4(255.f, 255.f, 255.f, 1.f), glm::vec4(255.f, 255.f, 255.f, 1.f),
                    glm::vec2(6.f, 12.f), &tex_gen_ball, glm::vec4(0.f, -30.f, 0.f, -60.f), glm::vec4(10.f, 10.f, -10.f, -10.f),
                    nullptr};

        generators.emplace_back(1.5f, glm::vec2(bricks[1].size.x, projSize.y), -1000.f,
                glm::vec4(0.f, 0.f, projSize.x - 2 * bricks[1].size.x, 0.f),
                false, GL_SRC_ALPHA, GL_ONE, pdata);
    }
    {
        Animation anim(1.5f, loadTexCoords("res/heart.sprites", std::vector<float>{0.4f, 0.1f, 0.1f}),
                       true, Origin::middle);

        AnimEntity anime(glm::vec2(0.f, 0.f), heart_tex, anim);
        anime.color.a = 0.8f;
        hp_bar = std::make_unique<HealthBar>(glm::vec2(bricks[1].size.x + 10.f, bricks[0].size.y + 10.f), anime);
        hp_bar->addLife();
        hp_bar->addLife();
        hp_bar->addLife();
    }
    {
        PData pdata{false, glm::vec2(10, 20), glm::vec4(0.f, 255.f, 0.f, 0.1f), glm::vec4(100.f, 255.f, 100.f, 0.3f),
                    glm::vec2(0.3f, 0.8f), &tex_gen_ball, glm::vec4(0.f, 0.f, 0.f, 0.f), glm::vec4(-20.f, -20.f, 20.f, 20.f),
                    nullptr};

        ball_parti = std::make_unique<ParticleGenerator>(0.01f, glm::vec2(0.f, 0.f), -1000.f,
                                                         glm::vec4(-ball.size / 2.f, ball.size),
                                                         true, GL_SRC_ALPHA, GL_ONE, pdata, [this](float)
        {
            ball_parti->setPosition(this->ball.position + this->ball.getRadius());
        });
    }
    {
        Text_Entity t(font);
        t.text.setText("<- / A   move left\n"
                       "-> / D   move right\n"
                       "SPACE   release ball\n"
                       "ESC       pause");
        t.text.position = fbSize / 2.f - t.text.getSize() / 2.f + glm::vec2(0.f, 60.f);
        t.text.bloom = true;
        t.text.color = glm::vec4(0.f, 255.f, 255.f, 0.3f);
        t.life = 10000000.f;
        t_entities.push_back(std::make_unique<Text_Entity>(t));
    }
}

void Game::processInput(const Input<int, std::hash<int>>& keys)
{
    paddle.velocity = glm::vec2(0.f, 0.f);

    if(keys.wasPressed(GLFW_KEY_ESCAPE))
    {
        new_menu = MenuName::Pause;
    }
    if(keys.wasPressed(GLFW_KEY_A) || keys.isPressed(GLFW_KEY_A)
            || keys.wasPressed(GLFW_KEY_LEFT) || keys.isPressed(GLFW_KEY_LEFT))
    {
        paddle.velocity.x -= paddle.velMod;
    }
    if(keys.wasPressed(GLFW_KEY_D) || keys.isPressed(GLFW_KEY_D)
            || keys.wasPressed(GLFW_KEY_RIGHT) || keys.isPressed(GLFW_KEY_RIGHT))
    {
        paddle.velocity.x += paddle.velMod;
    }
    if(keys.wasPressed(GLFW_KEY_SPACE))
    {
        ball.isStuck_ = false;
        if(start)
        {
            start = false;
            t_entities.erase(t_entities.begin());
        }
    }
}

void Game::update_logic(float dt_sec)
{
    //1 particles
    for(auto& generator: generators)
    {
        generator.update(dt_sec);
    }
    generators.erase(std::remove_if(generators.begin(), generators.end(),
                                    [](ParticleGenerator& gen){return !gen.isActive();}), generators.end());
    ball_parti->update(dt_sec);

    //2 hp bar
    hp_bar->update(dt_sec);

    //3 animations
    for(auto& anim: animations)
        anim.update(dt_sec);

    animations.erase(std::remove_if(animations.begin(), animations.end(),
                                    [](AnimEntity& anim){
                         if(anim.anim.getState() == State::end)
                         return true;
                         return false;
                     }), animations.end());

    //4 ball && paddle
    ball.update(dt_sec);
    if(!start)
        ball.velocity += glm::normalize(ball.velocity) * 0.5f * dt_sec;
    paddle.update(dt_sec);

    //5 bricks

    bricks.erase(std::remove_if(bricks.begin(), bricks.end(),
                                [](Brick& brick){
                     if(brick.b_type == Brick_type::gone)
                     return true;
                     return false;
                 }), bricks.end());

    //6 texts
    for(auto& text: t_entities)
    {
        text->update(dt_sec);
    }
    t_entities.erase(std::remove_if( t_entities.begin(),  t_entities.end(),
                                     [](std::unique_ptr<Text_Entity>& t){
                         if(t->life <= 0.f)
                         return true;
                         return false;
                     }),  t_entities.end());

    //prev_last: collisions
    doCollisions();

    //combo_counter
    if(combo_counter.second >= 0.f)
        combo_counter.second -= dt_sec;
    if(combo_counter.second < 0.f)
    {
        combo_counter.second = 0.f;
        if(combo_counter.first > 1)
        {
            t_entities.push_back(std::make_unique<Combo_t>(paddle.position + paddle.size / 2.f - glm::vec2(0.f, 60.f), font, combo_counter.first));
        }
        combo_counter.first = 0;
    }

    //shake time
    if(shake_time > 0.f)
        shake_time -= dt_sec;

    //blackout
    if(black_out_t > 0.f)
        black_out_t -= dt_sec;

    //last: win or loose
    if(hp_bar->isDead())
    {
        isDead = true;
        new_menu = MenuName::LoseScreen;
    }

    bool lvl_done = true;
    for(auto& brick: bricks)
    {
        if(brick.b_type != Brick_type::solid && brick.b_type != Brick_type::wall)
        {
            lvl_done = false;
            break;
        }
    }
    if(lvl_done)
    {
        isDead = true;
        new_menu = MenuName::WinScreen;
    }
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
    renderer.render(*ball_parti);
    for(auto& anim: animations)
        renderer.render(anim.getSprite());
    hp_bar->render(renderer);
    for(auto& text: t_entities)
        renderer.render(text->text);
}

void Game::doCollisions()
{
    bool ball_was_coll_br = false;
    // ball pen vec
    glm::vec2 ball_pen(0.f, 0.f);

    // ball bottom line
    if(ball.position.y > bottom_line)
    {
        animations.emplace_back(ball.position, tex_hit_bott, ball_drop_anim);
        hp_bar->loseLife(*this);

        Min_hp_t t(font, projSize);
        t_entities.push_back(std::make_unique<Min_hp_t>(t));

        ball.reset();

        // it should be this value.
        // it's coupled with shader and cos fun.
        black_out_t = 1.f;
    }
    // ball paddle
    {
        auto coll = ballRectCollision(ball, paddle);
        if(coll.isCollision)
        {
            if(!ball.isImmune_to_paddle())
            {
                ball.isImmune_to_paddle_ = 0.3f;
                paddle.reflectVEL(coll.penetration);

                if(glm::abs(coll.pene_vec.x) > glm::abs(ball_pen.x))
                    ball_pen.x = coll.pene_vec.x;
                if(glm::abs(coll.pene_vec.y) > glm::abs(ball_pen.y))
                    ball_pen.y = coll.pene_vec.y;
            }
        }
    }
    // ball bricks
    {
        for(auto& brick: bricks)
        {
            auto coll = ballRectCollision(ball, brick);
            if(coll.isCollision)
            {
                reflectVel(ball, coll.penetration);
                if(brick.b_type == Brick_type::one_hit)
                {
                    animations.emplace_back(coll.point, tex_paddle_hit, ball_hit_paddle_anim);
                    brick.b_type = Brick_type::gone;

                    // combo counter
                    ball_was_coll_br = true;
                    ++combo_counter.first;
                }
                else if(brick.b_type == Brick_type::solid)
                {
                    shake_time = 0.3f;
                    {
                        PData pdata{false, glm::vec2(5, 8), glm::vec4(0.f, 0.f, 0.f, 1.f), glm::vec4(50.f, 50.f, 50.f, 1.f),
                                    glm::vec2(0.5f, 2.5f), &tex_gen_ball, glm::vec4(20.f, 20.f, -20.f, -20.f),
                                    glm::vec4(0.f, 80.f, 0.f, 80.f), nullptr};

                        generators.emplace_back(0.001f, brick.position + glm::vec2(20.f, brick.size.y), 0.3f,
                                                glm::vec4(0.f, 0.f, brick.size.x - 40.f, 0.f),
                                                false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, pdata);
                    }
                }
                else if(brick.b_type == Brick_type::two_hit_effect)
                {
                    animations.emplace_back(coll.point, explo21, brick_explo21);
                    brick.b_type = Brick_type::gone;

                    // combo counter
                    ball_was_coll_br = true;
                    ++combo_counter.first;
                }

                if(glm::abs(coll.pene_vec.x) > glm::abs(ball_pen.x))
                    ball_pen.x = coll.pene_vec.x;
                if(glm::abs(coll.pene_vec.y) > glm::abs(ball_pen.y))
                    ball_pen.y = coll.pene_vec.y;
                {
                    PData pdata{true, glm::vec2(3, 8), glm::vec4(255.f, 0.f, 0.f, 1.f), glm::vec4(255.f, 100.f, 0.f, 1.f),
                                glm::vec2(0.5f, 1.f), &tex_gen_ball, glm::vec4(60.f, 60.f, -60.f, -60.f),
                                glm::vec4(-20.f, 80.f, 20.f, 80.f), nullptr};

                    generators.emplace_back(0.001f, coll.point, 0.1f,
                                            glm::vec4(-5.f, -5.f, 10.f, 10.f),
                                            true, GL_SRC_ALPHA, GL_ONE, pdata);
                }
            }
        }
    }
    // paddle brick index 1 and 2
    {
        for(std::size_t i = 1; i < 3; ++i)
        {
            auto coll = rectRectCollision(paddle, bricks[i]);
            if(coll.isCollision)
            {
                paddle.velocity = glm::vec2(0.f, 0.f);
                if(paddle.ball->isStuck_)
                    ball.position += paddle.prevPos - paddle.position;

                paddle.position = paddle.prevPos;
            }
        }
    }
    // next collisions

    // now correct ball position
    ball.position -= ball_pen;

    if(ball_was_coll_br)
        combo_counter.second += 0.6f;
}

void Game::update(float frameTime, PostProcessor&)
{
    if(frameTime > max_frame_time)
        frameTime = max_frame_time;
    time_passed += frameTime;
    //accumulator += frameTime;
    //while(accumulator >= dt_update)
    //{
    //update_logic(dt_update);
    //accumulator -= dt_update;
    //}
    update_logic(frameTime);
    pp.update(frameTime);
    if(shake_time > 0.f)
        pp.setShake(true);
    else
        pp.setShake(false);

    pp.setBlackOut(black_out_t);
}
