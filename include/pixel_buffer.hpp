// 
// Copyright 2022 Clemens Cords
// Created on 8/10/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_common.hpp>

namespace mousetrap
{
    class PixelBuffer
    {
        public:
            PixelBuffer(size_t width, size_t height);
            ~PixelBuffer();

            void bind();
            void unbind();

            /// \brief expose data
            float* map();

            /// \brief trigger data upload
            void unmap();

        private:
            void* _data = nullptr;

            Vector2ui _size;
            GLNativeHandle _native_handle;
    };
}

// ###

namespace mousetrap
{
    PixelBuffer::PixelBuffer(size_t width, size_t height)
        : _size(width, height)
    {
        glGenBuffers(1, &_native_handle);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _native_handle);
        glBufferData(GL_PIXEL_UNPACK_BUFFER,
             width * height * sizeof(float),
             nullptr,
             GL_STREAM_DRAW
        );
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }

    void PixelBuffer::bind()
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _native_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _size.x, _size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    void PixelBuffer::unbind()
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }

    float* PixelBuffer::map()
    {
        bind();
        return (float*) glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    }
}