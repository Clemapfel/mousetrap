//
// Created by clem on 2/13/23.
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::BrushShapeLayer::BrushShapeLayer(Canvas* owner)
        : _owner(owner)
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
        _area.connect_signal_render(on_area_render, this);
    }

    Canvas::BrushShapeLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::BrushShapeLayer::set_cursor_position(Vector2i pos)
    {
        _position = pos;
        reformat();
        _area.queue_render();
    }

    void Canvas::BrushShapeLayer::set_cursor_in_bounds(bool b)
    {
        _visible = b;

        if (not _area.get_is_realized())
            return;

        _render_shape->set_visible(_visible);
        _area.queue_render();
    }

    void Canvas::BrushShapeLayer::set_brush_outline_visible(bool b)
    {
        *_outline_visible = b;
        _area.queue_render();
    }

    void Canvas::BrushShapeLayer::on_layer_resolution_changed()
    {
        reformat();
        _area.queue_render();
    }

    void Canvas::BrushShapeLayer::set_outline_color(RGBA color)
    {
        *_outline_color = color;
        _area.queue_render();
    }

    void Canvas::BrushShapeLayer::on_color_selection_changed()
    {
        if (not _area.get_is_realized())
            return;

        auto tool = active_state->get_current_tool();
        if (tool == ToolID::BRUSH)
        {
            auto color = active_state->get_primary_color();
            _brush_shape->set_color(HSVA(color.h, color.s, color.v, active_state->get_brush_opacity()));
        }
        else if (tool == ToolID::ERASER)
            _brush_shape->set_color(RGBA(1, 1, 1, active_state->get_brush_opacity()));

        _area.queue_render();
    }

    void Canvas::BrushShapeLayer::on_brush_selection_changed()
    {
        if (not _area.get_is_realized())
            return;

        _brush_texture->create_from_image(active_state->get_current_brush()->get_image());
        reformat();
        _area.queue_render();
    }

    void Canvas::BrushShapeLayer::set_scale(float scale)
    {
        _scale = scale;
        reformat();
        _area.queue_render();
    }

    void Canvas::BrushShapeLayer::set_offset(Vector2f offset)
    {
        if (_offset == offset)
            return;

        _offset = {offset.x, offset.y};
        reformat();
        _area.queue_render();
    }

    void Canvas::BrushShapeLayer::on_active_tool_changed()
    {
        on_color_selection_changed();
    }

    void Canvas::BrushShapeLayer::on_area_realize(Widget* widget, BrushShapeLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_brush_shape = new Shape();
        auto color = active_state->get_primary_color();
        instance->_brush_shape->set_color(HSVA(color.h, color.s, color.v, active_state->get_brush_opacity()));

        instance->_brush_texture = new Texture();
        instance->_brush_texture->create_from_image(active_state->get_current_brush()->get_image());

        instance->_outline_shader = new Shader();
        instance->_outline_shader->create_from_file(get_resource_path() + "shaders/brush_outline.frag", ShaderType::FRAGMENT);

        instance->_render_shape = new Shape();
        instance->_render_texture = new RenderTexture();
        instance->_render_texture->set_scale_mode(TextureScaleMode::LINEAR);

        instance->_render_shape->set_texture(instance->_render_texture);

        instance->reformat();

        instance->_brush_shape_task = new RenderTask(instance->_brush_shape);
        instance->_render_shape_task = new RenderTask(instance->_render_shape, instance->_outline_shader);
        instance->_render_shape_task->register_vec2("_texture_size", instance->_canvas_size);
        instance->_render_shape_task->register_color("_outline_color_rgba", instance->_outline_color);
        instance->_render_shape_task->register_int("_outline_visible", instance->_outline_visible);

        instance->_render_shape->set_visible(instance->_visible);
        area->queue_render();
    }

    void Canvas::BrushShapeLayer::on_area_resize(GLArea* area, int w, int h, BrushShapeLayer* instance)
    {
        *instance->_canvas_size = {w, h};

        if (instance->_render_texture != nullptr)
            instance->_render_texture->create(w, h);

        instance->reformat();
        area->queue_render();
    }

    void Canvas::BrushShapeLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        auto layer_resolution = active_state->get_layer_resolution();

        float canvas_width = layer_resolution.x / _canvas_size->x;
        float canvas_height = layer_resolution.y / _canvas_size->y;

        canvas_width *= _scale;
        canvas_height *= _scale;

        Vector2f center = {0.5, 0.5};
        center += _offset;

        Vector2f top_left = center - Vector2f{0.5 * canvas_width, 0.5 * canvas_height};
        float pixel_w = canvas_width / layer_resolution.x;
        float pixel_h = canvas_height / layer_resolution.y;

        int brush_resolution = active_state->get_brush_size();

        float brush_width = (brush_resolution / float(layer_resolution.x)) * canvas_width;
        float brush_height = (brush_resolution / float(layer_resolution.y)) * canvas_height;

        float brush_pos_x = top_left.x + _position.x * pixel_w - 0.5 * brush_width + (brush_resolution % 2 == 1 ? 0.5 * pixel_w : pixel_w);
        float brush_pos_y = top_left.y + _position.y * pixel_h - 0.5 * brush_height + (brush_resolution % 2 == 1 ? 0.5 * pixel_h : pixel_h);

        _brush_shape->as_rectangle({brush_pos_x, brush_pos_y}, {brush_width, brush_height});
        _brush_shape->set_texture(_brush_texture);

        _render_shape->as_rectangle({0, 0}, {1, 1});
        _render_shape->set_texture(_render_texture);

        on_color_selection_changed();
    }

    bool Canvas::BrushShapeLayer::on_area_render(GLArea* area, GdkGLContext* context, BrushShapeLayer* instance)
    {
        area->make_current();
        gdk_gl_context_make_current(context);

        auto active_tool = active_state->get_current_tool();
        if (not (active_tool == ToolID::BRUSH or active_tool == ToolID::ERASER))
        {
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();
            return true;
        }

        static GLNativeHandle before = [](){
            GLint before = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &before);
            return before;
        }();

        {
            instance->_render_texture->bind_as_rendertarget();

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            glEnable(GL_BLEND);
            set_current_blend_mode(BlendMode::NORMAL);

            instance->_brush_shape_task->render();
            glFlush();
        }

        {
            glBindFramebuffer(GL_FRAMEBUFFER, before);

            gdk_gl_context_make_current(context);

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            glEnable(GL_BLEND);
            set_current_blend_mode(BlendMode::NORMAL);

            instance->_render_shape_task->render();

            glFlush();
        }

        return true;
    }


}
