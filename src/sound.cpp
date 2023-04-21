//
// Created by clem on 3/16/23.
//

#include <mousetrap/sound.hpp>
#include <mousetrap/log.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(SoundInternal, sound_internal, SOUND_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(SoundInternal, sound_internal, SOUND_INTERNAL)

        static void sound_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_SOUND_INTERNAL(object);
            G_OBJECT_CLASS(sound_internal_parent_class)->finalize(object);
            delete self->native;
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(SoundInternal, sound_internal, SOUND_INTERNAL)

        static SoundInternal* sound_internal_new()
        {
            auto* self = (SoundInternal*) g_object_new(sound_internal_get_type(), nullptr);
            sound_internal_init(self);
            self->native = new sf::Sound();
            NEW_SIGNAL("play", self);
            return self;
        }
    }
    
    Sound::Sound()
        : CTOR_SIGNAL(Sound, play)
    {
        _internal = detail::sound_internal_new();
    }

    Sound::~Sound()
    {
        g_object_unref(_internal);
    }

    Sound::operator GObject*() const
    {
        return G_OBJECT(_internal);
    }

    void Sound::create_from_buffer(const SoundBuffer& buffer)
    {
        auto* sf_buffer = buffer.operator sf::SoundBuffer*();
        if (sf_buffer == nullptr)
        {
            log::critical("In Sound::create_from_buffer: Buffer uninitialized", MOUSETRAP_DOMAIN);
            return;
        }

        _internal->native->setBuffer(*buffer.operator sf::SoundBuffer*());
    }

    void Sound::play()
    {
        _internal->native->play();
        emit_signal_play();
    }

    void Sound::pause()
    {
        _internal->native->pause();
    }

    void Sound::stop()
    {
        _internal->native->stop();
    }

    SoundStatus Sound::get_status() const
    {
        return (SoundStatus) _internal->native->getStatus();
    }

    bool Sound::get_should_loop() const
    {
        return _internal->native->getLoop();
    }

    void Sound::set_should_loop(bool b)
    {
        _internal->native->setLoop(b);
    }

    Time Sound::get_duration() const
    {
        auto duration = _internal->native->getBuffer()->getDuration();
        return microseconds(duration.asMicroseconds());
    }

    void Sound::set_playback_position(float fraction)
    {
        fraction = glm::clamp<float>(fraction, 0, 1);
        _internal->native->setPlayingOffset(sf::microseconds(fraction * get_duration().as_microseconds()));
    }

    float Sound::get_playback_position() const
    {
        auto offset = _internal->native->getPlayingOffset().asMicroseconds();
        return offset / get_duration().as_microseconds();
    }

    void Sound::set_pitch(float pitch)
    {
        _internal->native->setPitch(pitch);
    }

    float Sound::get_pitch() const
    {
        return _internal->native->getPitch();
    }

    void Sound::set_volume(float volume)
    {
        _internal->native->setVolume(volume);
    }

    float Sound::get_volume() const
    {
        return _internal->native->getVolume();
    }

    void Sound::set_spacial_position(float x, float y, float z)
    {
        if (_internal->native->getBuffer()->getChannelCount() != 1)
            std::cerr << "[WARNING] In Sound::set_spacial_position: Only 1-Channel (Mono) sounds can be spatialized" << std::endl;

        _internal->native->setPosition(x, y, z);
    }

    Vector3f Sound::get_spacial_position() const
    {
        auto vec = _internal->native->getPosition();
        return Vector3f(vec.x, vec.y, vec.z);
    }
}