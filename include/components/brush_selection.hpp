// 
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>

namespace mousetrap
{
    class BrushSelection : public Widget
    {
        public:
            BrushSelection();
            ~BrushSelection();

            GtkWidget* get_native() {
                return _render_area->get_native();
            }

        private:
            struct RenderArea : public GLArea
            {
                RenderArea() = default;

                void on_realize(GtkGLArea*);

                Shape* _todo_shape = nullptr;
                Texture* _todo_texture = nullptr;
            };

            RenderArea* _render_area;
    };
}

// ###

namespace mousetrap
{
    BrushSelection::BrushSelection()
    {
        _render_area = new RenderArea();
    }

    BrushSelection::~BrushSelection()
    {
        delete _render_area;
    }

    void BrushSelection::RenderArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        _todo_shape = new Shape();
        _todo_shape->as_rectangle({0, 0}, {1, 1});

        _todo_texture = new Texture();
        _todo_texture->create_from_file("/home/clem/Workspace/mousetrap/mole.png");
        _todo_shape->set_texture(_todo_texture);

        add_render_task(_todo_shape);
    }
}

