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
                public:
                    Frame(Vector2i size);

                    Image* get_image();
                    const Image* get_image() const;
                    const Texture* get_texture() const;
                    void update_texture();

                    bool get_is_keyframe() const;
                    void set_is_keyframe(bool);

                private:
                    Image _image;
                    Texture _texture;
                    bool _is_keyframe = true;
            };

            Layer(const std::string& name, Vector2i size, size_t n_frames);

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
            std::deque<Frame> _frames = std::deque<Frame>();

            std::string _name;

            bool _is_locked = false;
            bool _is_visible = true;
            float _opacity = 1;
            BlendMode _blend_mode = NORMAL;
    };
}
