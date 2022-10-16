// 
// Copyright 2022 Clemens Cords
// Created on 10/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/get_resource_path.hpp>

namespace mousetrap
{
    class TexturedSpacer : public GLArea
    {
        public:
            TexturedSpacer();
            ~TexturedSpacer();

        private:
            static inline Texture* _left_end_texture = nullptr;
            static inline Texture* _body_texture = nullptr;
            static inline Texture* _right_end_texture = nullptr;

            Shape* _left_end_shape = nullptr;
            Shape* _right_end_shape = nullptr;
            Shape* _body_shape = nullptr;

            Vector2f _canvas_size;

            static void on_realize(Widget*, TexturedSpacer* instance);
            static void on_resize(GLArea*, int, int, TexturedSpacer* instance);
    };
}

//

namespace mousetrap
{
    TexturedSpacer::TexturedSpacer()
    {
        connect_signal_realize(on_realize, this);
    }

    TexturedSpacer::~TexturedSpacer() noexcept
    {
        delete _left_end_shape;
        delete _body_shape;
        delete _right_end_shape;
    }

    void TexturedSpacer::on_realize(Widget*, TexturedSpacer* instance)
    {
        if (_left_end_texture == nullptr)
        {
            _left_end_texture = new Texture();
            _left_end_texture->create_from_file(get_resource_path() + "textures/textured_spacer_left_end.png");
        }

        if (_body_texture == nullptr)
        {
            _body_texture = new Texture();
            _body_texture->create_from_file(get_resource_path() + "textures/textured_spacer_body.png");
        }

        if (_right_end_texture == nullptr)
        {
            _right_end_texture = new Texture();
            _right_end_texture->create_from_file(get_resource_path() + "textures/textured_spacer_right_end.png");
        }
    }
}

