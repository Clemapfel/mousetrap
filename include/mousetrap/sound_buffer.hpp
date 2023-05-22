//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/16/23
//

#pragma once

#include <mousetrap/signal_emitter.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class SoundBuffer;
    namespace detail
    {
        struct _SoundBufferInternal
        {
            GObject parent;
            sf::SoundBuffer* native;
        };
        using SoundBufferInternal = _SoundBufferInternal;
        DEFINE_INTERNAL_MAPPING(SoundBuffer);
    }
    #endif

    /// @brief buffer holding sound data in ram
    class SoundBuffer : public SignalEmitter
    {
        public:
            /// @brief bit depths of the data
            using Sample_t = int16_t;

            /// @brief construct as empty buffer
            SoundBuffer();

            /// @brief construct from internal
            SoundBuffer(detail::SoundBufferInternal*);

            /// @brief destruct, frees data
            virtual ~SoundBuffer();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief expose a gobject
            operator NativeObject() const override;

            /// @brief create as silence
            /// @param sample_count total number of samples
            /// @param channel_count number of channels
            /// @param sample_rate in Hertz
            [[deprecated("unused as of v0.1.0")]] void create(size_t sample_count, size_t channel_count = 1, size_t sample_rate = 44100);

            /// @brief create from file, supports WAV, OGG/Vorbis and FLAC
            /// @param path absolute path
            /// @return true if loaded succesfully, false otherwise
            bool create_from_file(const std::string&);

            /// @brief save buffer to file
            /// @param path output path, does not create a new folder
            /// @return true if saved succesfully, false otherwise
            bool save_to_file(const std::string&);

            /// @brief get total number of samples of the data
            /// @return size_t
            size_t get_n_samples() const;

            /// @brief get pointer to sample data, immutable
            /// @return pointer to sample data, stored linearly in memory
            const Sample_t* get_samples() const;

            /// @brief expose as sf::SoundBuffer \for_internal_use_only
            operator sf::SoundBuffer*() const;

        private:
            detail::SoundBufferInternal* _internal = nullptr;
    };
}
