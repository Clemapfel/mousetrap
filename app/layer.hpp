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
        
        void add_layer(RGBA);
        void add_layer(const Image&);
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

    void state::add_layer(RGBA color)
    {
        auto image = Image();
        image.create(state::layer_resolution.x, state::layer_resolution.y, color);
        add_layer(image);
    }

    void state::add_layer(const Image& image)
    {
        state::layers.emplace_back(new Layer());

        float width = state::layer_resolution.x;
        float height = state::layer_resolution.y;

        auto image_size = Vector2ui(image.get_size().x, image.get_size().y);

        if (image_size == state::layer_resolution)
            ((Texture*) state::layers.back()->texture)->create_from_image(image);
        else
        {
            auto image_padded = Image();
            image_padded.create(state::layer_resolution.x, state::layer_resolution.y, RGBA(0, 0, 0, 0));

            for (size_t x = 0.5 * (image_size.x - state::layer_resolution.x); x < image_size.x and x < state::layer_resolution.x; ++x)
                for (size_t y = 0.5 * (image_size.y - state::layer_resolution.y); y < image_size.y and x < state::layer_resolution.y; ++y)
                    image_padded.set_pixel(x, y, image.get_pixel(x, y));

            ((Texture*) state::layers.back()->texture)->create_from_image(image);
        }
    }
}