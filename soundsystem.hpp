#ifndef SOUNDSYSTEM_HPP
#define SOUNDSYSTEM_HPP

#include <SFML/Audio.hpp>
#include <memory>

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

private:
    static std::unique_ptr<sf::SoundBuffer> click_buffer, switch_buffer;
    static std::unique_ptr<sf::Sound> click_sound, switch_sound;
    static std::unique_ptr<sf::Music> intro_music;
};

#endif // SOUNDSYSTEM_HPP
