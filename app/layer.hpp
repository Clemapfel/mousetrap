// 
// Copyright 2022 Clemens Cords
// Created on 9/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class Layer
    {
        public:
            class Frame
            {
                friend class Layer;

                public:
                    Frame();
                    Frame(Vector2i size);

                    Frame(const Frame&);
                    Frame(Frame&&);
                    Frame& operator=(const Frame&);
                    Frame& operator=(Frame&&);

                    ~Frame();

                    RGBA get_pixel(size_t x, size_t y) const;
                    void set_pixel(size_t x, size_t y, RGBA);

                    void overwrite_image(const Image&);
                    void set_size(Vector2ui);
                    Vector2ui get_size() const;

                    Vector2ui get_image_size() const;

                    const Texture* get_texture() const;
                    void update_texture();

                    bool get_is_keyframe() const;
                    void set_is_keyframe(bool);

                    void set_offset(Vector2i);
                    Vector2i get_offset() const;

                private:
                    Image* _image = nullptr;
                    Texture* _texture = nullptr;
                    bool _is_keyframe = true;

                    Vector2i _offset = {0, 0};
                    Vector2ui _size = {0, 0};
            };

            Layer(const std::string& name, Vector2ui size, size_t n_frames);

            Layer(const Layer&);
            Layer& operator=(const Layer&);

            Layer(Layer&&) = delete;
            Layer& operator=(Layer&&) = delete;

            Layer::Frame* add_frame(Vector2ui resolution, size_t);
            void delete_frame(size_t);

            Layer::Frame* get_frame(size_t index);
            const Layer::Frame* get_frame(size_t index) const;

            size_t get_n_frames() const;

            std::string get_name() const;
            void set_name(const std::string&);

            bool get_is_locked() const;
            bool get_is_visible() const;

            void set_is_locked(bool);
            void set_is_visible(bool);

            float get_opacity() const;
            void set_opacity(float);

            BlendMode get_blend_mode() const;
            void set_blend_mode(BlendMode);

        private:
            std::deque<Frame*> _frames;

            std::string _name;

            bool _is_locked = false;
            bool _is_visible = true;
            float _opacity = 1;
            BlendMode _blend_mode = NORMAL;
    };
}
