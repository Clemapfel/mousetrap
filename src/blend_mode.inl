// 
// Copyright 2022 Clemens Cords
// Created on 9/26/22 by clem (mail@clemens-cords.com)
//


namespace mousetrap
{
    inline void set_current_blend_mode(BlendMode mode)
    {
        if (mode == NONE)
        {
            glDisable(GL_BLEND);
            return;
        }

        glEnable(GL_BLEND);

        if (mode == NORMAL)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        }
        else if (mode == SUBTRACT)
        {
            glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT);
            glBlendFuncSeparate(GL_SRC_COLOR, GL_DST_COLOR, GL_SRC_ALPHA, GL_DST_ALPHA);
        }
        else if (mode == REVERSE_SUBTRACT)
        {
            glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_SUBTRACT);
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
        else
            glDisable(GL_BLEND);
    }

    inline std::string blend_mode_to_string(BlendMode mode)
    {
        if (mode == NONE)
            return "NONE";
        else if (mode == NORMAL)
            return "NORMAL";
        else if (mode == ADD)
            return "ADD";
        else if (mode == SUBTRACT)
            return "SUBTRACT";
        else if (mode == REVERSE_SUBTRACT)
            return "REVERSE_SUBTRACT";
        else if (mode == MULTIPLY)
            return "MULTIPLY";
        else if (mode == MIN)
            return "MIN";
        else if (mode == MAX)
            return "MAX";
        else
            return "NONE";
    }

    inline BlendMode blend_mode_from_string(const std::string& str)
    {
        if (str == "NORMAL" or str == "normal")
            return NORMAL;
        else if (str == "ADD" or str == "add")
            return ADD;
        else if (str == "SUBTRACT" or str == "subtract")
            return SUBTRACT;
        else if (str == "REVERSE_SUBTRACT" or str == "reverse_subtract")
            return REVERSE_SUBTRACT;
        else if (str == "MULTIPLY" or str == "multiply")
            return MULTIPLY;
        else if (str == "MIN" or str == "min")
            return MIN;
        else if (str == "MAX" or str == "max")
            return MAX;
        else
            return NONE;
    }
}