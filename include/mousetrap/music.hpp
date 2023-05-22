//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/26/23
//

#pragma once

#include <SFML/Audio/Music.hpp>

#include <mousetrap/vector.hpp>
#include <mousetrap/time.hpp>
#include <mousetrap/signal_component.hpp>
#include <mousetrap/signal_emitter.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Music;
    namespace detail
    {
        struct _MusicInternal
        {
            GObject parent;
            sf::Music* native;
        };
        using MusicInternal = _MusicInternal;
        DEFINE_INTERNAL_MAPPING(Music);
    }
    #endif

    /// @brief longform sound, streamed directly from disk if possible
    /// \signals
    /// \signal_play{Music}
    class Music : public SignalEmitter,
        HAS_SIGNAL(Music, play)
    {
        public:
            /// @brief construct
            Music();

            /// @brief construct from internal
            Music(detail::MusicInternal*);

            /// @brief destruct
            ~Music();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief expose gobject \for_internal_use_only
            operator NativeObject() const override;

            /// @brief create from file
            /// @param path absolute path
            bool create_from_file(const std::string& path);

            /// @brief play the sound, loops if mousetrap::Sound::set_should_loop was set to true, otherwise plays exactly once
            void play();

            /// @brief pause the playback, position is remembered
            void pause();

            /// @brief pause the playback, position is reset to the beginning
            void stop();

            /// @brief set which area of the file should loop
            /// @param start_time start of loop area
            /// @param end_time end of loop area
            void set_loop_region(Time start_time, Time end_time);

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
            detail::MusicInternal* _internal = nullptr;
    };
}
