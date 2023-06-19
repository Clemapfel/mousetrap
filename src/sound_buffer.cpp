//
// Created by clem on 3/16/23.
//

#ifdef UNDEF

#include <mousetrap/sound_buffer.hpp>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(SoundBufferInternal, sound_buffer_internal, SOUND_BUFFER_INTERNAL)

        static void sound_buffer_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_SOUND_BUFFER_INTERNAL(object);
            G_OBJECT_CLASS(sound_buffer_internal_parent_class)->finalize(object);

            delete self->native;
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(SoundBufferInternal, sound_buffer_internal, SOUND_BUFFER_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(SoundBufferInternal, sound_buffer_internal, SOUND_BUFFER_INTERNAL)

        static SoundBufferInternal* sound_buffer_internal_new()
        {
            auto* self = (SoundBufferInternal*) g_object_new(sound_buffer_internal_get_type(), nullptr);
            sound_buffer_internal_init(self);

            self->native = new sf::SoundBuffer();
            return self;
        }
    }

    SoundBuffer::SoundBuffer()
    {
        _internal = detail::sound_buffer_internal_new();
        g_object_ref(_internal);
    }

    SoundBuffer::SoundBuffer(detail::SoundBufferInternal* internal)
    {
        _internal = g_object_ref(internal);
    }

    SoundBuffer::~SoundBuffer()
    {
        g_object_unref(_internal);
    }

    NativeObject SoundBuffer::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    SoundBuffer::operator NativeObject() const
    {
        return G_OBJECT(_internal);
    }

    bool SoundBuffer::create_from_file(const std::string& path)
    {
        return _internal->native->loadFromFile(path);
    }

    bool SoundBuffer::save_to_file(const std::string& path)
    {
       return _internal->native->saveToFile(path);
    }

    SoundBuffer::operator sf::SoundBuffer*() const
    {
        return _internal->native;
    }

    size_t SoundBuffer::get_n_samples() const
    {
        return _internal->native->getSampleCount();
    }

    const SoundBuffer::Sample_t* SoundBuffer::get_samples() const
    {
        return _internal->native->getSamples();
    }
}

#endif