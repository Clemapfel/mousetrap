//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/16/23
//

#pragma once

#include <mousetrap/sound_buffer.hpp>
#include <mousetrap/time.hpp>
#include <mousetrap/vector.hpp>
#include <mousetrap/signal_emitter.hpp>
#include <mousetrap/signal_component.hpp>

#include <SFML/Audio/Sound.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    namespace detail
    {
        struct _SoundInternal
        {
            GObject parent;
            sf::Sound* native;
        };
        using SoundInternal = _SoundInternal;
    }
    #endif

    /// @brief status of a sound
    enum class SoundStatus
    {
        /// @brief sound is currently stopped
        STOPPED = sf::Sound::Status::Stopped,

        /// @brief sound is currently paused
        PAUSED = sf::Sound::Status::Paused,

        /// @brief sound is currently playing
        PLAYING = sf::Sound::Status::Playing
    };

    /// @brief a sound, streamed from ram
    /// \signals
    /// \signal_play{Sound}
    class Sound : public SignalEmitter,
        HAS_SIGNAL(Sound, play)
    {
        public:
            /// @brief construct
            Sound();

            /// @brief destruct
            ~Sound();

            /// @brief expose internal
            operator GObject*() const override;

            /// @brief construct from buffer
            /// @param buffer
            void create_from_buffer(const SoundBuffer&);

            /// @brief play the sound, loops if mousetrap::Sound::set_should_loop was set to true, otherwise plays exactly once
            void play();

            /// @brief pause the playback, position is remembered
            void pause();

            /// @brief pause the playback, position is reset to the beginning
            void stop();

            /// @brief get sound status
            SoundStatus get_status() const;

            /// @brief get whether the sound should loop indefinitely
            /// @return true if looping, false otherwise
            bool get_should_loop() const;

            /// @brief set whether the sound should loop indefinitely
            /// @param b true if should loop, false otherwise
            void set_should_loop(bool);

            /// @brief get the duration of the original sound buffer
            Time get_duration() const;

            /// @brief set playback position
            /// @param fraction: float in [0, 1]
            void set_playback_position(float fraction);

            /// @brief get playback position
            /// @returns float in [0, 1]
            float get_playback_position() const;

            /// @brief tune the pitch of the sound
            /// @param pitch
            void set_pitch(float);

            /// @brief get the pitch of the sound
            /// @return pitch
            float get_pitch() const;

            /// @brief get playback volume of sound
            /// @param volume may not be negative
            void set_volume(float);

            /// @brief get playback volume of sound
            /// @return volumne
            float get_volume() const;

            /// @brief modify the perceived spatial position of the sound
            /// @param x x-coordinate, relative to origin
            /// @param y y-coordinate, relative to origin
            /// @param z z-coordinate, relative to origin
            void set_spacial_position(float x = 0, float y = 0, float z = 0);

            /// @brief get perceived spatial position of the sound
            /// @return position in 3D space, (0, 0, 0) if at origin
            Vector3f get_spacial_position() const;

        private:
            detail::SoundInternal* _internal = nullptr;
    };
}
