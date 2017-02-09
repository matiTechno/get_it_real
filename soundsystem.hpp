#ifndef SOUNDSYSTEM_HPP
#define SOUNDSYSTEM_HPP

#include <SFML/Audio.hpp>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <string>

struct G_sound
{
    enum Sound
    {
        paddle_ball,
        heart,
        gem,
        coin,
        rain,
        hp_lose,
        powerup_destro,
        eyeball,
        paddle_ball_pene,
        candy,
        solid_brick_hit,
        wall_hit,
        br_destro_1,
        br_destro_2,
        br_destro_solid
    };
};

class SoundSystem
{
public:
    static void init()
    {
        click_buffer = std::make_unique<sf::SoundBuffer>();
        switch_buffer = std::make_unique<sf::SoundBuffer>();

        click_sound = std::make_unique<sf::Sound>();
        click_sound->setVolume(30.f);
        switch_sound = std::make_unique<sf::Sound>();
        switch_sound->setVolume(30.f);

        std::string file1("res/button04.mp3.ogg"), file2("res/Clic07.mp3.ogg");

        if(!click_buffer->loadFromFile(file1))
            throw std::runtime_error("[could not load: " + file1 + ']');
        if(!switch_buffer->loadFromFile(file2))
            throw std::runtime_error("[could not load: " + file2 + ']');

        click_sound->setBuffer(*click_buffer);
        switch_sound->setBuffer(*switch_buffer);

        load_sound_to_buffer("res/g_sound/Arkanoid SFX (1).ogg", G_sound::paddle_ball, 5.f);
        load_sound_to_buffer("res/g_sound/heartbeat.mp3_.ogg", G_sound::heart);
        load_sound_to_buffer("res/g_sound/133008__cosmicd__annulet-of-absorption(1).ogg", G_sound::gem, 30.f);
        load_sound_to_buffer("res/g_sound/Picked Coin Echo.ogg", G_sound::coin, 50.f);
        load_sound_to_buffer("res/g_sound/rock_breaking.ogg", G_sound::rain, 25.f);
        load_sound_to_buffer("res/g_sound/qubodupImpactMeat02.ogg", G_sound::hp_lose, 30.f);
        load_sound_to_buffer("res/g_sound/foom_0.ogg", G_sound::powerup_destro, 10.f);
        load_sound_to_buffer("res/g_sound/Arkanoid SFX (10).ogg", G_sound::eyeball, 40.f);
        load_sound_to_buffer("res/g_sound/flagdrop.ogg", G_sound::paddle_ball_pene, 25.f);
        load_sound_to_buffer("res/g_sound/qubodupImpactStone.ogg", G_sound::solid_brick_hit, 15.f);
        load_sound_to_buffer("res/g_sound/Randomize3.ogg", G_sound::candy, 20.f);
        load_sound_to_buffer("res/g_sound/skull_hit_1.ogg", G_sound::wall_hit, 20.f);
        load_sound_to_buffer("res/g_sound/sfx_exp_shortest_soft8.ogg", G_sound::br_destro_2, 30.f);
        load_sound_to_buffer("res/g_sound/sfx_exp_short_hard2.ogg", G_sound::br_destro_1, 20.f);
        load_sound_to_buffer("res/g_sound/sfx_exp_medium3.ogg", G_sound::br_destro_solid, 30);
    }

    static void playClick()
    {
        click_sound->play();
    }
    static void playSwitch()
    {
        switch_sound->play();
    }

    static void play_sound(G_sound::Sound sound)
    {
        game_sounds[sound].stop();
        game_sounds[sound].play();
    }

    static void pause_all_game_sounds()
    {
        for(auto& sound: game_sounds)
        {
            sound.second.pause();
        }
    }

    static void stop_all_game_sounds()
    {
        for(auto& sound: game_sounds)
        {
            sound.second.stop();
        }
    }

    static void resume_game_sounds()
    {
        for(auto& sound: game_sounds)
        {
            if(sound.second.getStatus() == sf::Sound::Status::Paused)
                sound.second.play();
        }
    }

    static std::unordered_map<G_sound::Sound, sf::SoundBuffer, std::hash<int>> game_s_buffers;
    static std::unordered_map<G_sound::Sound, sf::Sound, std::hash<int>> game_sounds;

private:
    static std::unique_ptr<sf::SoundBuffer> click_buffer, switch_buffer;
    static std::unique_ptr<sf::Sound> click_sound, switch_sound;

    static void load_sound_to_buffer(const std::string filename, G_sound::Sound sound_type, float volume = 100.f, bool loop = false)
    {
        if(!game_s_buffers[sound_type].loadFromFile(filename))
            throw std::runtime_error("[could not load: " + filename + ']');

        game_sounds[sound_type].setBuffer(game_s_buffers[sound_type]);
        game_sounds[sound_type].setVolume(volume);
        game_sounds[sound_type].setLoop(loop);
    }
};

#endif // SOUNDSYSTEM_HPP
