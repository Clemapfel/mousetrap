// 
// Copyright 2022 Clemens Cords
// Created on 9/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/texture.hpp>

namespace mousetrap
{
    enum BlendMode
    {
        NORMAL,     // alpha blending
        ADD,        // src + dest
        SUBTRACT,   // src - dest
        MULTIPLY,   // src * dest
        MIN,        // min(src, dest)
        MAX         // max(src, dest)
    };

    void set_blend_mode(BlendMode);

    struct Layer
    {
        static inline size_t _current_id = 0;
        Layer(const std::string& name = "");

        std::string name;
        Image image;
        TextureObject* texture;
        bool is_locked = false;
        bool is_visible = true;
        float opacity = 1;
        BlendMode blend_mode = NORMAL;
    };
    
    namespace state
    {
        std::deque<Layer*> layers;
        Vector2ui layer_resolution;

        Layer* new_layer(RGBA);
        Layer* new_layer(const Image&);
        Layer* new_layer(Layer*); // copies contents

        void insert_layer_at(Layer*, size_t position = 0);
        void insert_layer_after(Layer* to_insert, Layer* after);
        void insert_layer_before(Layer* to_insert, Layer* before);

        void delete_layer(Layer*);

        void move_layer_to(Layer*, size_t new_position);
        void move_layer_to_after(Layer* to_move, Layer* after);
        void mvoe_layer_to_before(Layer* to_move, Layer* before);
    }

}

/// ###

namespace mousetrap
{
    Layer::Layer(const std::string& name_in)
    {
        if (name_in.empty())
            name = "Layer_#" + std::to_string(_current_id++);

        texture = new Texture();
    }

    void set_blend_mode(BlendMode mode)
    {
        glEnable(GL_BLEND);

        if (mode == NORMAL)
        {
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        }
        else if (mode == SUBTRACT)
        {
            glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT);
            glBlendFuncSeparate(GL_SRC_COLOR, GL_DST_COLOR, GL_SRC_ALPHA, GL_DST_ALPHA);
        }
        else if (mode == ADD)
        {
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_COLOR, GL_DST_COLOR, GL_SRC_ALPHA, GL_DST_ALPHA);
        }
        else if (mode == MULTIPLY)
        {
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_DST_ALPHA, GL_ZERO);
        }
        else if (mode == MIN)
        {
            glBlendEquationSeparate(GL_MIN, GL_MIN);
            glBlendFuncSeparate(GL_SRC_COLOR, GL_DST_COLOR, GL_SRC_ALPHA, GL_DST_ALPHA);
        }
        else if (mode == MAX)
        {
            glBlendEquationSeparate(GL_MAX, GL_MAX);
            glBlendFuncSeparate(GL_SRC_COLOR, GL_DST_COLOR, GL_SRC_ALPHA, GL_DST_ALPHA);
        }
    }

    Layer* state::new_layer(RGBA color)
    {
        auto image = Image();
        image.create(state::layer_resolution.x, state::layer_resolution.y, color);
        return new_layer(image);
    }

    Layer* state::new_layer(const Image& image)
    {
        auto out = new Layer();

        float width = state::layer_resolution.x;
        float height = state::layer_resolution.y;

        auto image_size = Vector2ui(image.get_size().x, image.get_size().y);

        if (image_size == state::layer_resolution)
            ((Texture*) out->texture)->create_from_image(image);
        else
        {
            auto image_padded = Image();
            image_padded.create(state::layer_resolution.x, state::layer_resolution.y, RGBA(0, 0, 0, 0));

            for (size_t x = 0; x < std::min<size_t>(state::layer_resolution.x, image.get_size().x); ++x)
                for (size_t y = 0; y < std::min<size_t>(state::layer_resolution.y, image.get_size().y); ++y)
                    image_padded.set_pixel(x, y, image.get_pixel(x, y));

            ((Texture*) out->texture)->create_from_image(image_padded);
        }
    }

    Layer* state::new_layer(Layer* layer)
    {

    }


}