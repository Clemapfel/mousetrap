//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), 1/5/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/global_state.hpp>

namespace mousetrap
{
    class AnimationPreview : public AppComponent
    {
        public:
            AnimationPreview();

            operator Widget*() override;
            void update() override;

        private:
            // render
            GLArea _area;

            std::vector<Shape*> _layer_shapes;

            static inline Shader* _transparency_tiling_shader = nullptr;
            Vector2f _canvas_size = Vector2f(1, 1);
            Shape* _transparency_tiling_shape = nullptr;

            void queue_render_tasks();
            static void on_realize(Widget*, AnimationPreview*);
            static void on_resize(GLArea*, int w, int h, AnimationPreview*);

            // scale factor

            size_t _scale_factor = state::settings_file->get_value_as<size_t>("animation_preview", "default_scale_factor");
            size_t _max_scale_factor = state::settings_file->get_value_as<size_t>("animation_preview", "max_scale_factor");

            SpinButton _scale_factor_spin_button = SpinButton(1, _max_scale_factor, 1);
            SeparatorLine _scale_factor_spacer;
            Label _scale_factor_label = Label("Scale Factor: ");
            Box _scale_factor_box = Box(GTK_ORIENTATION_HORIZONTAL);

            void set_scale_factor(size_t);
            static void on_scale_factor_spin_button_value_changed(SpinButton* spin_button, AnimationPreview*);

            // fps

            float _fps = state::settings_file->get_value_as<float>("animation_preview", "default_fps");
            float max_fps = state::settings_file->get_value_as<float>("animation_preview", "max_fps");

            SpinButton _fps_spin_button = SpinButton(0.1, max_fps, 0.1);
            SeparatorLine _fps_spacer;
            Label _fps_label = Label("Scale Factor: ");
            Box _fps_box = Box(GTK_ORIENTATION_HORIZONTAL);

            void set_fps(float);
            static void on_fps_spin_button_value_changed(SpinButton* spin_button, AnimationPreview*);
            void on_tick_callback(FrameClock&);

            // background

            size_t _background_visible = state::settings_file->get_value_as<bool>("animation_preview", "background_visible");

            Switch _background_visible_switch;
            Label _background_visible_label = Label("Show Background: ");
            SeparatorLine _background_visible_spacer;
            Box _background_visible_box = Box(GTK_ORIENTATION_HORIZONTAL);

            void set_background_visible(bool);
            static void on_background_visible_switch_state_set(Switch*, bool, AnimationPreview*);

            // playback

            bool _playback_active = true;
            size_t _current_frame = 0;
            Time _elapsed = seconds(0);

            // menu

            MenuButton _menu_button;
            Label _menu_button_label = Label("Preview");
            MenuModel _menu;
            PopoverMenu _popover_menu = PopoverMenu(&_menu);

            // layout

            Frame _frame;
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            // actions

            Action _toggle_playback_active_action = Action("animation_preview.toggle_playback_active");
            Action _increase_scale_factor_action = Action("animation_preview.increase_scale_factor");
            Action _decrease_scale_factor_action = Action("animation_preview.decrease_scale_factor");
            Action _toggle_background_visible_action = Action("animation_preview.toggle_background_visible");

            void on_playback_toggled();

            Tooltip _tooltip;
            ShortcutController _shortcut_controller = ShortcutController(state::app);
            ClickEventController _click_controller;
            static void on_click_pressed(ClickEventController*, gint n_press, double x, double y, AnimationPreview*);
    };
}

// ###

namespace mousetrap
{
    AnimationPreview::AnimationPreview()
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);

        _menu_button.set_child(&_menu_button_label);

        // TODO why does *this* change locations in actions only

        _toggle_playback_active_action.set_do_function([](AnimationPreview*){
            auto* instance = ((AnimationPreview*) state::animation_preview);
            instance->on_playback_toggled();
        }, this);
        _toggle_playback_active_action.add_shortcut(state::keybindings_file->get_value("animation_preview", "toggle_playback_active"));
        state::app->add_action(_toggle_playback_active_action);
        _shortcut_controller.add_action(_toggle_playback_active_action.get_id());

        _increase_scale_factor_action.set_do_function([](AnimationPreview*){
            auto* instance = ((AnimationPreview*) state::animation_preview);
            if (instance->_scale_factor < instance->_max_scale_factor)
                instance->set_scale_factor(instance->_scale_factor + 1);
        }, this);
        _increase_scale_factor_action.add_shortcut(state::keybindings_file->get_value("animation_preview", "increase_scale_factor"));
        state::app->add_action(_increase_scale_factor_action);
        _shortcut_controller.add_action(_increase_scale_factor_action.get_id());

        _decrease_scale_factor_action.set_do_function([](AnimationPreview*){
            auto* instance = ((AnimationPreview*) state::animation_preview);
            if (instance->_scale_factor > 1)
                instance->set_scale_factor(instance->_scale_factor - 1);
        }, this);
        _decrease_scale_factor_action.add_shortcut(state::keybindings_file->get_value("animation_preview", "decrease_scale_factor"));
        state::app->add_action(_decrease_scale_factor_action);
        _shortcut_controller.add_action(_decrease_scale_factor_action.get_id());

        _toggle_background_visible_action.set_do_function([](AnimationPreview*){
            auto* instance = ((AnimationPreview*) state::animation_preview);
            instance->set_background_visible(not instance->_background_visible);
        }, this);
        _toggle_background_visible_action.add_shortcut(state::keybindings_file->get_value("animation_preview", "toggle_background_visible"));
        state::app->add_action(_toggle_background_visible_action);
        _shortcut_controller.add_action(_toggle_background_visible_action.get_id());

        _menu.add_action("Start / Stop Playback", "animation_preview.toggle_playback_active");

        auto settings_section = MenuModel();

        _fps_spin_button.set_value(_fps);
        _fps_spin_button.connect_signal_value_changed(on_fps_spin_button_value_changed, this);
        _fps_box.push_back(&_fps_label);
        _fps_spacer.set_opacity(0);
        _fps_box.push_back(&_fps_spacer);
        _fps_box.push_back(&_fps_spin_button);

        auto fps_submenu = MenuModel();
        fps_submenu.add_widget(&_fps_box);
        settings_section.add_submenu("Fps...", &fps_submenu);

        _scale_factor_spin_button.set_value(_scale_factor);
        _scale_factor_spin_button.connect_signal_value_changed(on_scale_factor_spin_button_value_changed, this);
        _scale_factor_box.push_back(&_scale_factor_label);
        _scale_factor_spacer.set_opacity(0);
        _scale_factor_box.push_back(&_scale_factor_spacer);
        _scale_factor_box.push_back(&_scale_factor_spin_button);

        auto scale_factor_submenu = MenuModel();
        scale_factor_submenu.add_widget(&_scale_factor_box);
        settings_section.add_submenu("Scale Factor...", &scale_factor_submenu);

        _background_visible_switch.set_active(_background_visible);
        _background_visible_switch.connect_signal_state_set(on_background_visible_switch_state_set, this);
        _background_visible_box.push_back(&_background_visible_label);
        _background_visible_spacer.set_opacity(0);
        _background_visible_box.push_back(&_background_visible_spacer);
        _background_visible_box.push_back(&_background_visible_switch);

        auto background_submenu = MenuModel();
        background_submenu.add_widget(&_background_visible_box);
        settings_section.add_submenu("Show / Hide Background...", &background_submenu);

        _menu.add_section("Settings", &settings_section);

        _popover_menu.refresh_widgets();
        _menu_button.set_popover(&_popover_menu);
        _menu_button.set_popover_position(GTK_POS_LEFT);

        for (auto* box : {&_fps_box, &_scale_factor_box, &_background_visible_box})
            box->set_margin_horizontal(state::margin_unit);

        _frame.set_child(&_area);
        _frame.set_expand(true);
        _frame.set_label_widget(nullptr);

        _main.push_back(&_menu_button);
        _main.push_back(&_frame);

        _tooltip.create_from("animation_preview", state::tooltips_file, state::keybindings_file);

        _main.set_tooltip_widget(_tooltip);
        _main.add_controller(&_shortcut_controller);

        _click_controller.connect_signal_click_pressed(on_click_pressed, this);
        _frame.add_controller(&_click_controller);

        _main.add_tick_callback([](FrameClock clock, AnimationPreview* instance) -> bool {
            instance->on_tick_callback(clock);
            return true;
        }, this);
    }

    AnimationPreview::operator Widget*()
    {
        return &_main;
    }

    void AnimationPreview::update()
    {}

    void AnimationPreview::on_realize(Widget* widget, AnimationPreview* instance)
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

        for (auto* shape : instance->_layer_shapes)
            delete shape;

        instance->_layer_shapes.clear();

        for (auto* layer : state::layers)
        {
            auto* shape = instance->_layer_shapes.emplace_back(new Shape());
            shape->as_rectangle({0, 0}, {1, 1});
            shape->set_texture(layer->frames.at(instance->_current_frame).texture);
        }

        instance->set_scale_factor(instance->_scale_factor);
        instance->set_fps(instance->_fps);
        instance->set_background_visible(instance->_background_visible);

        instance->queue_render_tasks();
        area->queue_render();
    }

    void AnimationPreview::queue_render_tasks()
    {
        _area.clear_render_tasks();

        auto transparency_task = RenderTask(_transparency_tiling_shape, _transparency_tiling_shader);
        transparency_task.register_vec2("_canvas_size", &_canvas_size);

        _area.add_render_task(transparency_task);

        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            auto task = RenderTask(_layer_shapes.at(layer_i), nullptr, nullptr, state::layers.at(layer_i)->blend_mode);
            _area.add_render_task(task);
        }

        _area.queue_render();
    }

    void AnimationPreview::on_resize(GLArea*, int w, int h, AnimationPreview* instance)
    {
        instance->_canvas_size = {w, h};
        instance->set_scale_factor(instance->_scale_factor);
        instance->_area.queue_render();
    }

    void AnimationPreview::on_scale_factor_spin_button_value_changed(SpinButton* spin_button, AnimationPreview* instance)
    {
        instance->set_scale_factor(spin_button->get_value());
    }

    void AnimationPreview::set_scale_factor(size_t x)
    {
        _scale_factor = x;

        auto pixel_size = Vector2f{
            1 / float(_canvas_size.x),
            1 / float(_canvas_size.y)
        };

        auto centroid = Vector2f{0.5, 0.5};
        auto size = Vector2f(
            pixel_size.x * state::layer_resolution.x * _scale_factor,
            pixel_size.y * state::layer_resolution.y * _scale_factor
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

        _area.queue_render();
    }

    void AnimationPreview::on_fps_spin_button_value_changed(SpinButton* spin_button, AnimationPreview* instance)
    {
        instance->set_fps(spin_button->get_value());
    }

    void AnimationPreview::set_fps(float value)
    {
        _fps = value;
    }

    void AnimationPreview::on_background_visible_switch_state_set(Switch* s, bool state, AnimationPreview* instance)
    {
        instance->set_background_visible(state);
        s->set_state((Switch::State) state);
    }

    void AnimationPreview::set_background_visible(bool b)
    {
        _background_visible = b;

        if (_transparency_tiling_shape != nullptr)
            _transparency_tiling_shape->set_visible(b);

        _area.queue_render();
    }

    void AnimationPreview::on_tick_callback(FrameClock& clock)
    {
        if (not _playback_active or _fps == 0)
            return;

        auto frame_duration = seconds(1.f / _fps);
        _elapsed += clock.get_time_since_last_frame();

        while (_elapsed > frame_duration)
        {
            _elapsed -= frame_duration;
            _current_frame = _current_frame + 1 == state::n_frames ? 0 : _current_frame + 1;
        }

        for (size_t layer_i = 0; _layer_shapes.size() == state::layers.size() and layer_i < state::layers.size(); ++layer_i)
        {
            auto* layer = state::layers.at(layer_i);
            _layer_shapes.at(layer_i)->set_texture(layer->frames.at(_current_frame).texture);
        }

        _area.queue_render();
    }

    void AnimationPreview::on_playback_toggled()
    {
        _playback_active = not _playback_active;

        if (not _playback_active)
        {
            _current_frame = state::current_frame;
            for (size_t layer_i = 0; _layer_shapes.size() == state::layers.size() and layer_i < state::layers.size(); ++layer_i)
            {
                auto* layer = state::layers.at(layer_i);
                _layer_shapes.at(layer_i)->set_texture(layer->frames.at(_current_frame).texture);
            }
            _area.queue_render();
        }
    }

    void AnimationPreview::on_click_pressed(ClickEventController*, gint n_press, double x, double y, AnimationPreview* instance)
    {
        for (size_t i = 0; i < n_press; ++i)
            instance->_toggle_playback_active_action.activate();
    }
}
