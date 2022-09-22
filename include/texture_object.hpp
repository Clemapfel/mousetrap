// 
// Copyright 2022 Clemens Cords
// Created on 9/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    struct TextureObject
    {
        virtual void bind() const = 0;
        virtual void unbind() const = 0;
    };

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
}

//#include <src/texture_object.hpp>

namespace mousetrap
{
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
}