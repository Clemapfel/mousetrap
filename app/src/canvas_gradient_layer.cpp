//
// Created by clem on 3/9/23.
//

#include <app/canvas.hpp>

#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    Canvas::GradientLayer::GradientLayer(Canvas* owner)
        : _owner(owner)
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
        _area.connect_signal_render(on_area_render, this);

        set_offset(_owner->_offset);
        set_scale(_owner->_scale);

        state::actions::canvas_apply_gradient.set_function([]()
        {
            auto* self = state::canvas->_gradient_layer;
            if (not self->_area.get_is_realized())
                return;

            auto image = self->_render_texture->download();
            auto draw_data = ProjectState::DrawData();

            for (size_t x = 0; x < image.get_size().x; ++x)
                for (size_t y = 0; y < image.get_size().y; ++y)
                    draw_data.insert({{x, y}, image.get_pixel(x, y).operator HSVA()});
            active_state->draw_to_cell(active_state->get_current_cell_position(), draw_data);

        });
        state::add_shortcut_action(state::actions::canvas_apply_gradient);

        // TODO
        auto* debug_action = new Action("global.debug_action");
        debug_action->set_function([](){
           state::canvas->_gradient_layer->recompile_shader();
        });
        state::add_shortcut_action(*debug_action);
        // TODO
    }

    Canvas::GradientLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::GradientLayer::set_scale(float scale)
    {
        _scale = scale;
        reformat();
    }

    void Canvas::GradientLayer::set_offset(Vector2f offset)
    {
        _offset = offset;
        reformat();
    }

    void Canvas::GradientLayer::on_layer_resolution_changed()
    {
        if (not _area.get_is_realized())
            return;

        auto size = active_state->get_layer_resolution();
        _render_texture->create(size.x, size.y);
        _area.queue_render();
    }

    void Canvas::GradientLayer::on_area_realize(Widget* widget, GradientLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (instance->_shader == nullptr)
        {
            instance->_shader = new Shader();
            instance->_shader->create_from_file(get_resource_path() + "shaders/canvas_gradient.frag", ShaderType::FRAGMENT);
        }

        instance->_shader_shape = new Shape();
        instance->_shader_shape->as_rectangle({0, 0}, {1, 1});

        instance->_render_shape = new Shape();
        instance->_render_texture = new RenderTexture();

        instance->on_layer_resolution_changed();
        instance->reformat();

        instance->_render_shape->set_texture(instance->_render_texture);
        instance->_render_task = new RenderTask(instance->_render_shape);

        instance->_shader_task = new RenderTask(instance->_shader_shape, instance->_shader);
        instance->_shader_task->register_color("_origin_color_rgba", instance->_origin_color_rgba);
        instance->_shader_task->register_color("_destination_color_rgba", instance->_destination_color_rgba);
        instance->_shader_task->register_int("_dither_mode", instance->_dither_mode);
        instance->_shader_task->register_vec2("_origin_point", instance->_origin_point);
        instance->_shader_task->register_vec2("_destination_point", instance->_destination_point);
        instance->_shader_task->register_int("_is_circular", instance->_is_circular);
    }

    void Canvas::GradientLayer::on_area_resize(GLArea*, int w, int h, GradientLayer* instance)
    {
        instance->_canvas_size = {w, h};
        instance->reformat();
    }

    bool Canvas::GradientLayer::on_area_render(GLArea* area, GdkGLContext* context, GradientLayer* instance)
    {
        area->make_current();
        gdk_gl_context_make_current(context);

        static GLNativeHandle before = [](){
            GLint before = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &before);
            return before;
        }();

        glEnable(GL_BLEND);
        set_current_blend_mode(BlendMode::NORMAL);

        {
            instance->_render_texture->bind_as_rendertarget();

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            instance->_shader_task->render();
            glFlush();
        }

        {
            glBindFramebuffer(GL_FRAMEBUFFER, before);

            gdk_gl_context_make_current(context);

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            instance->_render_task->render();
            glFlush();
        }

        return true;
    }

    void Canvas::GradientLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        auto layer_resolution = active_state->get_layer_resolution();

        float width = layer_resolution.x / _canvas_size.x;
        float height = layer_resolution.y / _canvas_size.y;

        width *= _scale;
        height *= _scale;

        Vector2f center = {0.5, 0.5};
        center += _offset;

        Vector2f top_left = center - Vector2f{0.5 * width, 0.5 * height};
        _render_shape->as_rectangle(top_left, {width, height});

        _area.queue_render();
    }

    // TODO
    void Canvas::GradientLayer::recompile_shader()
    {
        if (not _area.get_is_realized())
            return;

        std::cout << "recompiling..." << std::endl;
        _shader->create_from_file(get_resource_path() + "shaders/canvas_gradient.frag", ShaderType::FRAGMENT);
        _area.queue_render();
    }
    // TODO
}
