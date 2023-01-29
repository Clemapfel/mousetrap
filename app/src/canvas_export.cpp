//
// Created by clem on 1/23/23.
//

#include "app/canvas_export.hpp"
#include "app/project_state.hpp"

namespace mousetrap
{
    CanvasExport::CanvasExport()
        : _frame(active_state->get_layer_resolution().x / float(active_state->get_layer_resolution().y))
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
        _area.connect_signal_render(on_area_render, this);
        _area.set_size_request(active_state->get_layer_resolution());
        _area.set_align(GTK_ALIGN_CENTER);

        _frame.set_child(&_area);
        _frame.set_expand(false);
        _box.push_back(&_frame);
        _box.set_expand(true);
    }

    CanvasExport::operator Widget*()
    {
        return &_box;
    }
    
    void CanvasExport::queue_render_tasks()
    {
        _area.clear_render_tasks();

        for (size_t layer_i = 0; layer_i < active_state->get_n_layers(); ++layer_i)
            _render_tasks.emplace_back(_shapes.at(layer_i), nullptr, nullptr, active_state->get_layer(layer_i)->get_blend_mode());
    }

    void CanvasExport::on_area_realize(Widget* widget, CanvasExport* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->on_layer_count_changed();
    }
    
    void CanvasExport::on_area_resize(GLArea*, int w, int h, CanvasExport* instance)
    {
        instance->_canvas_size = {w, h};
    }

    gboolean CanvasExport::on_area_render(GLArea* area, GdkGLContext* context, CanvasExport* instance)
    {
       // noop
    }

    Image CanvasExport::merge_layers(const std::set<size_t>& layer_is)
    {
        std::vector<RenderTask> tasks;
        tasks.reserve(layer_is.size());

        for (size_t i : layer_is)
            tasks.push_back(_render_tasks.at(i));

        return merge(tasks);
    }

    Image CanvasExport::merge(const std::vector<RenderTask>& tasks)
    {
        // update render

        _area.make_current();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        set_current_blend_mode(BlendMode::NORMAL);

        for (auto task : tasks)
           task.render();

        glFlush();

        // read buffer

        auto out = Image();
        out.create(active_state->get_layer_resolution().x, active_state->get_layer_resolution().y);

        std::vector<float> buffer;
        auto size = active_state->get_layer_resolution();
        buffer.resize(size.x * size.y * 4);

        glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_FLOAT, &buffer[0]);

        for (size_t y = 0, i = 0; y < size.y; ++y)
        {
            for (size_t x = 0; x < size.x; ++x, i += 4)
            {
                float r = buffer.at(i+0);
                float g = buffer.at(i+1);
                float b = buffer.at(i+2);
                float a = buffer.at(i+3);

                out.set_pixel(x, size.y - (y+1), RGBA(r, g, b, a));
            }
        }

        return out;
    }
    
    void CanvasExport::on_layer_count_changed()
    {
        _area.make_current();

        for (auto* shape : _shapes)
            delete shape;

        _shapes.clear();

        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
        {
            if (_shapes.size() <= i)
                _shapes.emplace_back(new Shape());

            auto* layer = _shapes.at(i);
            layer->as_rectangle({0, 0}, {1, 1});
            layer->set_texture(active_state->get_frame(i, active_state->get_current_frame_index())->get_texture());
            layer->set_visible(active_state->get_layer(i)->get_is_visible());
            layer->set_color(RGBA(1, 1, 1, active_state->get_layer(i)->get_is_visible()));
        }

        queue_render_tasks();
    }

    void CanvasExport::on_layer_properties_changed()
    {
        if (not _area.get_is_realized())
            return;

        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
        {
            const auto* layer = active_state->get_layer(i);
            _shapes.at(i)->set_texture(layer->get_frame(active_state->get_current_frame_index())->get_texture());
            _shapes.at(i)->set_visible(layer->get_is_visible());
            _shapes.at(i)->set_color(RGBA(1, 1, 1, layer->get_opacity()));
        }
    }

    void CanvasExport::on_layer_image_updated()
    {
        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
            _shapes.at(i)->set_texture(active_state->get_frame(i, active_state->get_current_frame_index())->get_texture());
    }

    void CanvasExport::on_layer_resolution_changed()
    {
        on_layer_image_updated();
        _area.set_size_request(active_state->get_layer_resolution());
    }
}
