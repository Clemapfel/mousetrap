//
// Created by clem on 3/16/23.
//

#include <include/sound_buffer.hpp>

namespace mousetrap
{
    SoundBuffer::SoundBuffer()
    {
        _data = new sf::SoundBuffer();
    }

    SoundBuffer::~SoundBuffer()
    {
        delete _data;
    }

    bool SoundBuffer::create_from_file(const std::string& path)
    {
        return _data->loadFromFile(path);
    }

    bool SoundBuffer::save_to_file(const std::string& path)
    {
       return _data->saveToFile(path);
    }

    SoundBuffer::operator sf::SoundBuffer*() const
    {
        return _data;
    }

    size_t SoundBuffer::get_n_samples() const
    {
        return _data->getSampleCount();
    }

    const SoundBuffer::Sample_t* SoundBuffer::get_samples() const
    {
        return _data->getSamples();
    }
}
