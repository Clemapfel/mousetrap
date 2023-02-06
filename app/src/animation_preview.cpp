//
// Created by clem on 1/23/23.
//

#include <app/animation_preview.hpp>

namespace mousetrap
{
    AnimationPreview::AnimationPreview()
    {
        _layer_area.connect_signal_realize(on_layer_area_realize, this);
        _layer_area.connect_signal_render(on_layer_area_render, this);
        _layer_area.connect_signal_resize(on_layer_area_resize, this);
        _layer_area.set_size_request({0, 0});

        _transparency_area.connect_signal_realize(on_transparency_area_realize, this);
        _transparency_area.connect_signal_realize(on_transparency_area_realize, this);

        _overlay.set_child(&_transparency_area);
        _overlay.add_overlay(&_layer_area);

        _menu_button.set_child(&_menu_button_label);

        // actions

        using namespace state::actions;

        animation_preview_increase_scale_factor.set_function([](){
           state::animation_preview->set_scale_factor(state::animation_preview->_scale_factor + 1);
        });

        animation_preview_increase_scale_factor.set_function([](){
            auto current = state::animation_preview->_scale_factor;
            state::animation_preview->set_scale_factor(current > 0 ? current - 1 : current);
        });

        animation_preview_toggle_playback_active.set_stateful_function([](bool){
           auto next = not state::animation_preview->_playback_active;
           state::animation_preview->set_playback_active(next);
           return next;
        });

        animation_preview_toggle_background_visible.set_function([](){
            auto next = not state::animation_preview->_background_visible;
            state::animation_preview->set_background_visible(next);
            return next;
        });

        for (auto* action : {&animation_preview_toggle_playback_active, &animation_preview_decrease_scale_factor, &animation_preview_increase_scale_factor, &animation_preview_toggle_background_visible})
            state::add_shortcut_action(*action);

        auto playback_section = MenuModel();
        playback_section.add_stateful_action("Start / Stop Playback", animation_preview_toggle_playback_active.get_id(), _playback_active);
        _menu.add_section("Playback", &playback_section);

        auto settings_section = MenuModel();

        _fps_spin_button.set_value(_fps);
        _fps_spin_button.connect_signal_value_changed([](SpinButton* scale, AnimationPreview* instance){
            state::animation_preview->set_fps(scale->get_value());
        }, this);
        _fps_box.push_back(&_fps_label);
        _fps_spacer.set_opacity(0);
        _fps_box.push_back(&_fps_spacer);
        _fps_box.push_back(&_fps_spin_button);
        _fps_box.set_tooltip_text(state::tooltips_file->get_value("animation_preview", "fps_setting"));

        auto fps_submenu = MenuModel();
        fps_submenu.add_widget(&_fps_box);
        settings_section.add_submenu("Fps...", &fps_submenu);

        _scale_factor_spin_button.set_value(_scale_factor);
        _scale_factor_spin_button.connect_signal_value_changed([](SpinButton* scale, AnimationPreview* instance){
            state::animation_preview->set_scale_factor(scale->get_value());
        }, this);
        _scale_factor_box.push_back(&_scale_factor_label);
        _scale_factor_spacer.set_opacity(0);
        _scale_factor_box.push_back(&_scale_factor_spacer);
        _scale_factor_box.push_back(&_scale_factor_spin_button);
        _scale_factor_box.set_tooltip_text(state::tooltips_file->get_value("animation_preview", "scale_factor_setting"));

        auto scale_factor_submenu = MenuModel();
        scale_factor_submenu.add_widget(&_scale_factor_box);
        settings_section.add_submenu("Scale Factor...", &scale_factor_submenu);

        _menu.add_section("Settings", &settings_section);

        _popover_menu.refresh_widgets();
        _menu_button.set_popover(&_popover_menu);
        _menu_button.set_popover_position(GTK_POS_LEFT);

        _frame.set_child(&_overlay);
        _frame.set_expand(true);
        _frame.set_label_widget(nullptr);

        _box.push_back(&_menu_button);
        _box.push_back(&_frame);

        _tooltip.create_from("animation_preview", state::tooltips_file, state::keybindings_file);
        _box.set_tooltip_widget(_tooltip);

        _click_controller.connect_signal_click_pressed(on_click_pressed, this);
        _frame.add_controller(&_click_controller);

        _box.add_tick_callback([](FrameClock clock, AnimationPreview* instance) -> bool {
            instance->on_tick_callback(clock);
            return true;
        }, this);

        _main.push_back(&_box);
    }

    AnimationPreview::operator Widget*()
    {
        return &_main;
    }

    void AnimationPreview::set_scale_factor(size_t x)
    {
        _scale_factor = x;

        _layer_area.make_current();

        auto pixel_size = Vector2f{
                1 / float(_canvas_size.x),
                1 / float(_canvas_size.y)
        };

        auto centroid = Vector2f{0.5, 0.5};
        auto size = Vector2f(
                pixel_size.x * active_state->get_layer_resolution().x * _scale_factor,
                pixel_size.y * active_state->get_layer_resolution().y * _scale_factor
        );

        for (auto* shape : _layer_shapes)
        {
            shape->as_rectangle(
                {centroid.x - size.x, centroid.y - size.y},
                {centroid.x + size.x, centroid.y - size.y},
                {centroid.x + size.x, centroid.y + size.y},
                {centroid.x - size.x, centroid.y + size.y}
            );
        }

        _transparency_area.queue_render();
        _layer_area.queue_render();
    }

    void AnimationPreview::set_fps(float value)
    {
        _fps = value;
    }

    void AnimationPreview::set_background_visible(bool b)
    {
        _background_visible = b;

        if (_transparency_tiling_shape != nullptr)
            _transparency_tiling_shape->set_visible(b);

        _transparency_area.queue_render();
    }

    void AnimationPreview::on_tick_callback(FrameClock& clock)
    {
        if (not _playback_active or _fps == 0)
        {
            if (_current_frame == active_state->get_current_frame_index())
                return;

            _current_frame = active_state->get_current_frame_index();
        }
        else
        {
            auto frame_duration = seconds(1.f / _fps);
            _elapsed += clock.get_time_since_last_frame();

            while (_elapsed > frame_duration)
            {
                _elapsed -= frame_duration;
                _current_frame = _current_frame + 1 >= active_state->get_n_frames() ? 0 : _current_frame + 1;
            }
        }

        for (size_t layer_i = 0; _layer_shapes.size() == active_state->get_n_layers() and layer_i < active_state->get_n_layers(); ++layer_i)
        {
            auto* layer = active_state->get_layer(layer_i);
            _layer_shapes.at(layer_i)->set_texture(layer->get_frame(_current_frame)->get_texture());
        }

        _transparency_area.queue_render();
        _layer_area.queue_render();
    }

    void AnimationPreview::set_playback_active(bool b)
    {
        _playback_active = b;

        if (not _playback_active)
        {
            _current_frame = active_state->get_current_frame_index();
            for (size_t layer_i = 0; _layer_shapes.size() == active_state->get_n_layers() and layer_i < active_state->get_n_layers(); ++layer_i)
            {
                auto* layer = active_state->get_layer(layer_i);
                _layer_shapes.at(layer_i)->set_texture(layer->get_frame(_current_frame)->get_texture());
            }
            _layer_area.queue_render();
        }
    }

    void AnimationPreview::on_click_pressed(ClickEventController*, gint n_press, double x, double y, AnimationPreview* instance)
    {
        for (size_t i = 0; i < n_press; ++i)
            state::animation_preview->set_playback_active(not state::animation_preview->_playback_active);
    }

    void AnimationPreview::queue_render_tasks()
    {
        _transparency_area.clear_render_tasks();

        auto transparency_task = RenderTask(_transparency_tiling_shape, _transparency_tiling_shader);
        transparency_task.register_vec2("_canvas_size", &_canvas_size);

        _transparency_area.add_render_task(transparency_task);
        _transparency_area.queue_render();

        // layers

        _layer_area_render_tasks.clear();

        for (size_t layer_i = 0; layer_i < active_state->get_n_layers(); ++layer_i)
        {
            auto task = RenderTask(_layer_shapes.at(layer_i), nullptr, nullptr, active_state->get_layer(layer_i)->get_blend_mode());
            _layer_area_render_tasks.push_back(task);
        }

        // post fx

        _post_fx_shape_render_task = RenderTask(_post_fx_shape, _post_fx_shader);

        auto& offset = active_state->get_color_offset();
        _post_fx_shape_render_task.register_float("_h_offset", _h_offset);
        _post_fx_shape_render_task.register_float("_s_offset", _s_offset);
        _post_fx_shape_render_task.register_float("_v_offset", _v_offset);
        _post_fx_shape_render_task.register_float("_r_offset", _r_offset);
        _post_fx_shape_render_task.register_float("_g_offset", _g_offset);
        _post_fx_shape_render_task.register_float("_b_offset", _b_offset);
        _post_fx_shape_render_task.register_float("_a_offset", _a_offset);

        _layer_area.queue_render();
    }

    void AnimationPreview::on_layer_area_realize(Widget* widget, AnimationPreview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_post_fx_shader = new Shader();
        instance->_post_fx_shader->create_from_file(get_resource_path() + "shaders/project_post_fx.frag", ShaderType::FRAGMENT);

        instance->_post_fx_texture = new RenderTexture();
        instance->_post_fx_texture->create(1, 1);

        instance->_post_fx_shape = new Shape();
        instance->_post_fx_shape->as_rectangle({0, 0}, {1, 1});
        instance->_post_fx_shape->set_texture(instance->_post_fx_texture);

        instance->on_layer_count_changed();

        instance->set_scale_factor(instance->_scale_factor);
        instance->set_fps(instance->_fps);
        instance->set_background_visible(instance->_background_visible);

        area->queue_render();
    }

    gboolean AnimationPreview::on_layer_area_render(GLArea*, GdkGLContext* context, AnimationPreview* instance)
    {
        gdk_gl_context_make_current(context);

        static GLNativeHandle before = [](){
            GLint before = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &before);
            return before;
        }();

        {
            instance->_post_fx_texture->bind_as_rendertarget();

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            glEnable(GL_BLEND);
            set_current_blend_mode(BlendMode::NORMAL);

            for (auto task:instance->_layer_area_render_tasks)
                task.render();

            glFlush();
        }

        {
            glBindFramebuffer(GL_FRAMEBUFFER, before);

            gdk_gl_context_realize(context, nullptr);
            gdk_gl_context_make_current(context);

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            glEnable(GL_BLEND);
            set_current_blend_mode(BlendMode::NORMAL);

            instance->_post_fx_shape_render_task.render();

            glFlush();
        }
    }

    void AnimationPreview::on_transparency_area_realize(Widget* widget, AnimationPreview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (_transparency_tiling_shader == nullptr)
        {
            _transparency_tiling_shader = new Shader();
            _transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        area->queue_render();
    }

    void AnimationPreview::on_layer_area_resize(GLArea*, int w, int h, AnimationPreview* instance)
    {
        instance->_canvas_size = {w, h};
        instance->set_scale_factor(instance->_scale_factor);
        instance->_transparency_area.queue_render();
        instance->_post_fx_texture->create(w, h);
    }

    void AnimationPreview::on_transparency_area_resize(GLArea*, int w, int h, AnimationPreview* instance)
    {
        instance->_canvas_size = {w, h};
        instance->set_scale_factor(instance->_scale_factor);
        instance->_transparency_area.queue_render();
    }

    void AnimationPreview::on_layer_count_changed()
    {
        _layer_area.make_current();

        _current_frame = active_state->get_current_frame_index();

        for (auto* shape : _layer_shapes)
            delete shape;

        _layer_shapes.clear();

        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
        {
            if (_layer_shapes.size() <= i)
                _layer_shapes.emplace_back(new Shape());

            auto* layer = _layer_shapes.at(i);
            layer->as_rectangle({0, 0}, {1, 1});
            layer->set_texture(active_state->get_frame(i, _current_frame)->get_texture());
            layer->set_visible(active_state->get_layer(i)->get_is_visible());
            layer->set_color(RGBA(1, 1, 1, active_state->get_layer(i)->get_is_visible()));
        }

        queue_render_tasks();
        on_transparency_area_resize(&_transparency_area, _canvas_size.x, _canvas_size.y, this);
        on_layer_area_resize(&_layer_area, _canvas_size.x, _canvas_size.y, this);
    }

    void AnimationPreview::on_layer_properties_changed()
    {
        if (not _transparency_area.get_is_realized() or not _layer_area.get_is_realized())
            return;

        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
        {
            const auto* layer = active_state->get_layer(i);
            _layer_shapes.at(i)->set_texture(layer->get_frame(_current_frame)->get_texture());
            _layer_shapes.at(i)->set_visible(layer->get_is_visible());
            _layer_shapes.at(i)->set_color(RGBA(1, 1, 1, layer->get_opacity()));
        }

        queue_render_tasks();
        on_transparency_area_resize(&_transparency_area, _canvas_size.x, _canvas_size.y, this);
        on_layer_area_resize(&_layer_area, _canvas_size.x, _canvas_size.y, this);
    }

    void AnimationPreview::on_layer_image_updated()
    {
        if (not _transparency_area.get_is_realized() or not _layer_area.get_is_realized())
            return;

        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
            _layer_shapes.at(i)->set_texture(active_state->get_frame(i, _current_frame)->get_texture());

        on_transparency_area_resize(&_transparency_area, _canvas_size.x, _canvas_size.y, this);
        on_layer_area_resize(&_layer_area, _canvas_size.x, _canvas_size.y, this);
    }

    void AnimationPreview::on_layer_frame_selection_changed()
    {
        on_layer_image_updated();
    }

    void AnimationPreview::on_playback_toggled()
    {
        set_playback_active(active_state->get_playback_active());
    }

    void AnimationPreview::on_playback_fps_changed()
    {
        set_fps(active_state->get_fps());
    }

    void AnimationPreview::on_layer_resolution_changed()
    {
        on_layer_image_updated();
    }

    void AnimationPreview::on_color_offset_changed()
    {
        const auto& offset = active_state->get_color_offset();

        *_h_offset = offset.at(0);
        *_s_offset = offset.at(1);
        *_v_offset = offset.at(2);
        *_r_offset = offset.at(3);
        *_g_offset = offset.at(4);
        *_b_offset = offset.at(5);
        *_a_offset = offset.at(6);

        _layer_area.queue_render();
    }
}
