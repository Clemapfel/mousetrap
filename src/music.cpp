//
// Created by clem on 3/26/23.
//

#include <include/music.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(MusicInternal, music_internal, MUSIC_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(MusicInternal, music_internal, MUSIC_INTERNAL)

        static void music_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_MUSIC_INTERNAL(object);
            G_OBJECT_CLASS(music_internal_parent_class)->finalize(object);
            delete self->native;
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(MusicInternal, music_internal, MUSIC_INTERNAL)

        static MusicInternal* music_internal_new()
        {
            auto* self = (MusicInternal*) g_object_new(music_internal_get_type(), nullptr);
            music_internal_init(self);
            self->native = new sf::Music();

            NEW_SIGNAL_FOR_TYPE("play", music_internal);
            return self;
        }
    }

    Music::Music()
        : CTOR_SIGNAL(Music, play)
    {
        _internal = detail::music_internal_new();
    }
    
    Music::~Music()
    {
        g_object_unref(_internal);
    }
    
    Music::operator GObject*() const
    {
        return G_OBJECT(_internal);   
    }

    bool Music::create_from_file(const std::string& path)
    {
        return _internal->native->openFromFile(path);
    }

    void Music::play() 
    {
        _internal->native->play();
    }

    void Music::pause() 
    {
        _internal->native->pause();
    }

    void Music::stop() 
    {
        _internal->native->stop();
    }

    bool Music::get_should_loop() const 
    {
        return _internal->native->getLoop();
    }

    void Music::set_should_loop(bool b) 
    {
        _internal->native->setLoop(b);
    }

    Time Music::get_duration() const 
    {
        return microseconds(_internal->native->getDuration().asMicroseconds());
    }

    void Music::set_playback_position(float fraction)
    {
        fraction = glm::clamp<float>(fraction, 0, 1);
        _internal->native->setPlayingOffset(sf::microseconds(fraction * get_duration().as_microseconds()));
    }

    float Music::get_playback_position() const
    {
        auto offset = _internal->native->getPlayingOffset().asMicroseconds();
        return offset / get_duration().as_microseconds();
    }

    void Music::set_pitch(float pitch)
    {
        _internal->native->setPitch(pitch);
    }

    float Music::get_pitch() const
    {
        return _internal->native->getPitch();
    }

    void Music::set_volume(float volume)
    {
        _internal->native->setVolume(volume);
    }

    float Music::get_volume() const
    {
        return _internal->native->getVolume();
    }

    void Music::set_spacial_position(float x, float y, float z)
    {
        if (_internal->native->getChannelCount() != 1)
            log::critical("In Music::set_spacial_position: Only 1-Channel (Mono) sounds can be spatialized", MOUSETRAP_DOMAIN);

        _internal->native->setPosition(x, y, z);
    }

    Vector3f Music::get_spacial_position() const
    {
        auto vec = _internal->native->getPosition();
        return Vector3f(vec.x, vec.y, vec.z);
    }

    void Music::set_loop_region(Time start_time, Time end_time)
    {
        float start_mys = start_time.as_microseconds();
        float end_mys = start_time.as_microseconds();

        _internal->native->setLoopPoints(sf::Music::TimeSpan(sf::microseconds(start_mys), sf::seconds(end_mys - start_mys)));
    }
}
