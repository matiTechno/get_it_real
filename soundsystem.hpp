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
        paddle_ball
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
        switch_sound = std::make_unique<sf::Sound>();
        intro_music = std::make_unique<sf::Music>();

        std::string file1("res/button04.mp3.flac"),
                file2("res/Clic07.mp3.flac"),
                file3("res/wind1.wav");

        if(!click_buffer->loadFromFile(file1))
            throw std::runtime_error("[could not load: " + file1 + ']');
        if(!switch_buffer->loadFromFile(file2))
            throw std::runtime_error("[could not load: " + file2 + ']');
        if(!intro_music->openFromFile(file3))
            throw std::runtime_error("[could not load: " + file3 + ']');

        click_sound->setBuffer(*click_buffer);
        switch_sound->setBuffer(*switch_buffer);
        click_sound->setVolume(35.f);
        switch_sound->setVolume(35.f);

        // load game sounds
        load_sound_to_buffer("res/g_sound/Arkanoid SFX (1).wav", G_sound::paddle_ball, 50.f);
    }

    static void playIntroMusic(bool play)
    {
        if(play)
            intro_music->play();
        else
            intro_music->stop();
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
        // couldnt manage to play multiple sounds from same buffer due to
        // status checking bugs and other stuff, sfml...
        // in fututre i will certainly move to other sound apis
        // maybe portauidio??? rtaudio??
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
    static std::unique_ptr<sf::Music> intro_music;

    static void load_sound_to_buffer(const std::string filename, G_sound::Sound sound_type, float volume = 100.f)
    {
        if(!game_s_buffers[sound_type].loadFromFile(filename))
            throw std::runtime_error("[could not load: " + filename + ']');

        game_sounds[sound_type].setBuffer(game_s_buffers[sound_type]);
        game_sounds[sound_type].setVolume(volume);
    }
};

#endif // SOUNDSYSTEM_HPP
