namespace mousetrap
{
    void LayerView::LayerFrameDisplay::on_gl_area_realize(Widget*, LayerFrameDisplay* instance)
    {
        instance->_gl_area.make_current();

        if (transparency_tiling_shader == nullptr)
        {
            transparency_tiling_shader = new Shader();
            transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_layer_shape = new Shape();

        if (selection_indicator_texture == nullptr)
        {
            selection_indicator_texture = new Texture();
            selection_indicator_texture->create_from_file(get_resource_path() + "frame/frame_vertical.png");
        }

        instance->update();
    }

    void LayerView::LayerFrameDisplay::on_gl_area_resize(GLArea*, int w, int h, LayerFrameDisplay* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_gl_area.queue_render();
    }

    LayerView::LayerFrameDisplay::LayerFrameDisplay(size_t layer, size_t frame, LayerView* owner)
            : _layer(layer),
              _frame(frame),
              _owner(owner),
              _aspect_frame(state::layer_resolution.x / float(state::layer_resolution.y)),
              _frame_widget_label((frame < 10 ? "00" : (frame < 100 ? "0" : "")) + std::to_string(_frame))
    {
        _gl_area.connect_signal_realize(on_gl_area_realize, this);
        _gl_area.connect_signal_resize(on_gl_area_resize, this);
        _gl_area.set_size_request({thumbnail_height, (thumbnail_height / float(state::layer_resolution.x)) * state::layer_resolution.y});
        _gl_area.set_expand(false);

        _aspect_frame.set_child(&_gl_area);
        _frame_widget.set_child(&_aspect_frame);
        _frame_widget.set_label_widget(&_frame_widget_label);
        _frame_widget_label.set_visible(false);

        _overlay.set_child(&_frame_widget);
        _layer_frame_active_icon.set_align(GTK_ALIGN_END);
        _layer_frame_active_icon.set_visible(false);
        _layer_frame_active_icon.set_size_request(_layer_frame_active_icon.get_size());
        _overlay.add_overlay(&_layer_frame_active_icon);

        _main.push_back(&_overlay);
        _frame_widget.set_cursor(GtkCursorType::POINTER);
    }

    LayerView::LayerFrameDisplay::~LayerFrameDisplay()
    {
        delete _transparency_tiling_shape;
        delete _layer_shape;
    }

    LayerView::LayerFrameDisplay::operator Widget*()
    {
        return &_main;
    }

    void LayerView::LayerFrameDisplay::update()
    {
        _frame_widget_label.set_text((_frame < 10 ? "00" : (_frame < 100 ? "0" : "")) + std::to_string(_frame));

        auto& layer = *state::layers.at(_layer);
        auto& frame = layer.frames.at(_frame);
        _gl_area.set_opacity(frame.is_tween_repeat ? 0.3 : 1);
        _main.set_tooltip_text("Layer: " + std::to_string(_layer) + " | Frame: " + std::to_string(_frame));

        if (_gl_area.get_is_realized())
        {
            _transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});
            _layer_shape->as_rectangle({0, 0}, {1, 1});

            size_t frame_i = _frame;

            while (frame_i > 0 and layer.frames.at(frame_i).is_tween_repeat)
                frame_i -= 1;

            _layer_shape->set_texture(layer.frames.at(frame_i).texture);

            auto task = RenderTask(_transparency_tiling_shape, transparency_tiling_shader);
            task.register_vec2("_canvas_size", &_canvas_size);

            _gl_area.add_render_task(task);
            _gl_area.add_render_task(_layer_shape);
            _gl_area.queue_render();

            update_selection();
        }
    }

    void LayerView::LayerFrameDisplay::update_selection()
    {
        _frame_widget_label.set_visible(_layer == _owner->_selected_layer);

        if (not _gl_area.get_is_realized())
            return;

        float k;
        if (_frame == _owner->_selected_frame and _layer == _owner->_selected_layer)
            k = 1;
        if (_frame == _owner->_selected_frame or _layer == _owner->_selected_layer)
            k = 1;
        else
            k = 0.6;

        float opacity = _transparency_tiling_shape->get_vertex_color(0).a;
        _transparency_tiling_shape->set_color(RGBA(k, k, k, opacity));

        opacity = _layer_shape->get_vertex_color(0).a;
        _layer_shape->set_color(RGBA(k, k, k, opacity));

        _gl_area.queue_render();
    }
}