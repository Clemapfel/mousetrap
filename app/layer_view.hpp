// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/list_view.hpp>
#include <include/box.hpp>
#include <include/gl_area.hpp>

namespace mousetrap
{
    class LayerView : public Widget
    {
        public:

        private:

            static inline Shader* _noop_shader;
            static inline Shape* _layer_view_row_canvas_shape;

            struct LayerViewRow
            {
                Box* _main_box;

                GLArea* _texture_area;
            };
    };
}