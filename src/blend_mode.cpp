//
// Copyright 2022 Clemens Cords
// Created on 9/26/22 by clem (mail@clemens-cords.com)
//

#include <include/blend_mode.hpp>
#include <include/gl_common.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    void set_current_blend_mode(BlendMode mode, bool allow_alpha_blend)
    {
        // source: [1] https://github.com/SFML/SFML/blob/master/src/SFML/Graphics/BlendMode.cpp#L36

        //
        // S: Source (rgba), upper layer
        // D: Destination (rgba), lower layer
        // O: Output, merged layer
        //
        // sigma_rgb: S.rgb factor
        // sigma_a: S.a factor
        // delta_rgb: D.rgb factor
        // delta_a: D.a factor
        //
        // f_rgb: rgb function
        // f_a: alpha functino
        //
        // O.rgb = f_rgb(sigma_rgb * S.rgb, delta_rgb * D.rgb)
        // O.a = f_a(sigma_a * S.a, delta_a * D.a)
        //
        //

        glEnable(GL_BLEND);

        if (mode == NORMAL)
        {
            // O.rgb = S.a * S.rgb + (1 - S.a) * D.rgb
            // O.a = 1 * S.a + (1 - S.a) * D.a

            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }
        else if (mode == ADD)
        {
            // O.rgb = S.a * S.rgb + 1 * D.rgb
            // O.a = 1 * S.a + 1 * D.a

            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
        }
        else if (mode == MULTIPLY)
        {
            // O.rgb = D.rgb * S.rgb + 0 * D.rgb
            // O.a = D.a * S.a + 0 * D.a

            if (allow_alpha_blend)
            {
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_DST_ALPHA, GL_ZERO);
            }
            else
            {
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO,  GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }
        }
        else if (mode == MIN)
        {
            // O.rgb = min(1 * S.rgb, 1 * D.rgb)
            // O.a = min(1 * S.a, 1 * D.a)

            if (allow_alpha_blend)
            {
                glBlendEquationSeparate(GL_MIN, GL_MIN);
                glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
            }
            else
            {
                glBlendEquationSeparate(GL_MIN, GL_ADD);
                glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }
        }
        else if (mode == MAX)
        {
            // O.rgb = max(1 * S.rgb, 1 * D.rgb)
            // O.a = max(1 * S.a, 1 * D.a)

            if (allow_alpha_blend)
            {
                glBlendEquationSeparate(GL_MAX, GL_MAX);
                glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
            }
            else
            {
                glBlendEquationSeparate(GL_MAX, GL_ADD);
                glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }

        }
        else if (mode == REVERSE_SUBTRACT)
        {
            // O.rgb = S.a * S.rgb - 1 * D.rgb
            // O.a = 1 * S.a - 1 * D.a

            if (allow_alpha_blend)
            {
                glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_SUBTRACT); // sic
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
            }
            else
            {
                glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_ADD); // sic
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }
        }
        else if (mode == SUBTRACT)
        {
            // O.rgb = 1 * D.rgb - S.a * S.rgb
            // O.a = 1 * D.a - 1 * S.a;

            if (allow_alpha_blend)
            {
                glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT); // sic
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
            }
            else
            {
                glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_ADD); // sic
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
            }
        }
        else
            glDisable(GL_BLEND);
    }

    std::string blend_mode_to_string(BlendMode mode)
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

    BlendMode blend_mode_from_string(const std::string& str)
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
        {
            log::critical("In blend_mode_from_string: Unrecognized blend mode `" + str + "`");
            return NONE;
        }
    }
}