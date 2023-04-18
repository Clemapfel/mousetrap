//
// Created by clem on 3/26/23.
//

#include <include/music.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap
{
    Music::Music()
    {}

    bool Music::create_from_file(const std::string& path)
    {
        return _native.openFromFile(path);
    }

    void Music::play() 
    {
        _native.play();
    }

    void Music::pause() 
    {
        _native.pause();
    }

    void Music::stop() 
    {
        _native.stop();
    }

    bool Music::get_should_loop() const 
    {
        return _native.getLoop();
    }

    void Music::set_should_loop(bool b) 
    {
        _native.setLoop(b);
    }

    Time Music::get_duration() const 
    {
        return microseconds(_native.getDuration().asMicroseconds());
    }

    void Music::set_playback_position(float fraction)
    {
        fraction = glm::clamp<float>(fraction, 0, 1);
        _native.setPlayingOffset(sf::microseconds(fraction * get_duration().as_microseconds()));
    }

    float Music::get_playback_position() const
    {
        auto offset = _native.getPlayingOffset().asMicroseconds();
        return offset / get_duration().as_microseconds();
    }

    void Music::set_pitch(float pitch)
    {
        _native.setPitch(pitch);
    }

    float Music::get_pitch() const
    {
        return _native.getPitch();
    }

    void Music::set_volume(float volume)
    {
        _native.setVolume(volume);
    }

    float Music::get_volume() const
    {
        return _native.getVolume();
    }

    void Music::set_spacial_position(float x, float y, float z)
    {
        if (_native.getChannelCount() != 1)
            log::critical("In Music::set_spacial_position: Only 1-Channel (Mono) sounds can be spatialized", MOUSETRAP_DOMAIN);

        _native.setPosition(x, y, z);
    }

    Vector3f Music::get_spacial_position() const
    {
        auto vec = _native.getPosition();
        return Vector3f(vec.x, vec.y, vec.z);
    }

    void Music::set_loop_area(Time start_time, Time end_time)
    {
        float start_mys = start_time.as_microseconds();
        float end_mys = start_time.as_microseconds();

        _native.setLoopPoints(sf::Music::TimeSpan(sf::microseconds(start_mys), sf::seconds(end_mys - start_mys)));
    }
}
