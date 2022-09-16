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
    }
}