// 
// Copyright 2022 Clemens Cords
// Created on 8/10/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_common.hpp>

#include <atomic>

namespace mousetrap
{
    class PixelBuffer
    {
        public:
            PixelBuffer(size_t width, size_t height);
            virtual ~PixelBuffer();

            void bind() const;
            void unbind() const;

            /// \brief trigger stream of cache to gpu-side buffer
            void flush() const;
            Vector2ui get_size() const;

            void set_pixel(size_t, size_t, RGBA);
            void set_pixel(size_t linear_index, RGBA);
            RGBA get_pixel(size_t, size_t);

            float* get_data();

        private:
            size_t to_linear_index(size_t x, size_t y);

            float* _cache = nullptr;
            mutable std::atomic<bool> _bound = false;

            Vector2ui _size;
            GLNativeHandle _native_handle;
    };
}

#include <src/pixel_buffer.inl>