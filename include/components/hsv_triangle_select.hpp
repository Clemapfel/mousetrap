// 
// Copyright 2022 Clemens Cords
// Created on 8/2/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/get_resource_path.hpp>

#include <include/components/global_state.hpp>

namespace mousetrap
{
    class HsvTriangleSelect;
    static inline HsvTriangleSelect* hsv_triangle_select = nullptr;

    struct HsvTriangleSelect : public Widget
    {
        public:
            HsvTriangleSelect(float width);
            ~HsvTriangleSelect();

            GtkWidget* get_native();

        //private:
            struct TriangleShaderArea : public GLArea
            {
                TriangleShaderArea();
                virtual ~TriangleShaderArea();

                void on_realize(GtkGLArea*) override;
                void on_resize(GtkGLArea*, int, int) override;

                Shape* _shape;
                Shader* _shader = nullptr;
                Vector2f* _canvas_size;
            };

            TriangleShaderArea* _triangle_area;
    };
}

// ###

namespace mousetrap
{
    GtkWidget* HsvTriangleSelect::get_native()
    {
        return _triangle_area->get_native();
    }

    HsvTriangleSelect::HsvTriangleSelect(float width)
    {
        _triangle_area = new TriangleShaderArea();
        _triangle_area->set_size_request(Vector2f{width, width});
    }

    HsvTriangleSelect::~HsvTriangleSelect()
    {
        delete _triangle_area;
    }

    HsvTriangleSelect::TriangleShaderArea::TriangleShaderArea()
    {
        _shader = new Shader();
        _shader->create_from_file(get_resource_path() + "shaders/hsv_triangle_select.frag", ShaderType::FRAGMENT);
    }

    HsvTriangleSelect::TriangleShaderArea::~TriangleShaderArea()
    {
        delete _shader;
        delete _shape;
    }

    void HsvTriangleSelect::TriangleShaderArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        _shape = new Shape();
        _shape->as_rectangle({0, 0}, {1, 1});

        auto size = get_size();
        _canvas_size = new Vector2f();
        on_resize(area, size.x, size.y);

        auto task = RenderTask(_shape, _shader);
        task.register_vec2("_canvas_size", _canvas_size);
        task.register_color("_current_color_hsv", &current_color);

        add_render_task(task);
    }

    void HsvTriangleSelect::TriangleShaderArea::on_resize(GtkGLArea* area, int w, int h)
    {
        _canvas_size->x = w;
        _canvas_size->y = h;

        gtk_gl_area_queue_render(area);
    }
}