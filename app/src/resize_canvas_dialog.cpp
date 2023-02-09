//
// Created by clem on 2/5/23.
//

#include <app/resize_canvas_dialog.hpp>
#include <app/project_state.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/canvas_export.hpp>

namespace mousetrap
{
    void ResizeCanvasDialog::on_layer_resolution_changed()
    {
        auto size = active_state->get_layer_resolution();
        if (_scale_mode == ABSOLUTE)
        {
            _width_spin_button.set_value(size.x);
            _height_spin_button.set_value(size.y);
        }

        auto ratio = size.x / float(size.y);
        auto ratio_string =  std::to_string(ratio);
        std::string ratio_string_formatted = "";

        if (int(ratio) == ratio)
            ratio_string_formatted = std::to_string(int(ratio));
        else
        {
            bool comma_seen = false;
            size_t post_comma_count = 0;
            for (auto c:ratio_string)
            {
                if (comma_seen and (post_comma_count++ >= 3 or c == '0'))
                    break;

                ratio_string_formatted.push_back(c);

                if (c == '.')
                    comma_seen = true;
            }
        }

        _maintain_aspect_ratio_label.set_text(state::tooltips_file->get_value("scale_canvas_dialog", "maintain_aspect_ratio_label") + " (" + ratio_string_formatted + ")");
    }

    void ResizeCanvasDialog::set_final_size(size_t w, size_t h)
    {
        _final_size_label.set_text("New Size: " + std::to_string(w) + " x " + std::to_string(h) + " px");
    }

    void ResizeCanvasDialog::update_offset_bounds()
    {
        int w_old = active_state->get_layer_resolution().x;
        int h_old = active_state->get_layer_resolution().y;
        int w_next = _width;
        int h_next = _height;

        if (w_next <= w_old)
        {
            _x_offset_button.set_lower_limit(0);
            _x_offset_button.set_upper_limit(abs(w_old - w_next));
        }
        else
        {
            _x_offset_button.set_lower_limit(-1 * abs(w_old - w_next));
            _x_offset_button.set_upper_limit(0);
        }

        if (h_next <= h_old)
        {
            _y_offset_button.set_lower_limit(0);
            _y_offset_button.set_upper_limit(abs(h_old - h_next));
        }
        else
        {
            _y_offset_button.set_lower_limit(-1 * abs(h_old - h_next));
            _y_offset_button.set_upper_limit(0);
        }
    }

    void ResizeCanvasDialog::set_x_offset(int x)
    {
        _x_offset = x;

        _x_offset_button.set_signal_value_changed_blocked(true);
        _x_offset_button.set_value(x);
        _x_offset_button.set_signal_value_changed_blocked(false);
        reformat_area();
    }

    void ResizeCanvasDialog::set_y_offset(int y)
    {
        _y_offset = y;

        _y_offset_button.set_signal_value_changed_blocked(true);
        _y_offset_button.set_value(y);
        _y_offset_button.set_signal_value_changed_blocked(false);
        reformat_area();
    }

    void ResizeCanvasDialog::center_offset()
    {
        std::cerr << "[ERROR] In ResizeCanvasDialog::center: TODO" << std::endl;
    }

    void ResizeCanvasDialog::reset_offset()
    {
        std::cerr << "[ERROR] In ResizeCanvasDialog::center: TODO" << std::endl;
    }

    void ResizeCanvasDialog::set_scale_mode(ScaleMode mode)
    {
        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        auto max_size = state::settings_file->get_value_as<size_t>("global", "maximum_image_size");
        if (mode == ABSOLUTE)
        {
            _width_spin_button.set_upper_limit(max_size);
            _height_spin_button.set_upper_limit(max_size);

            _width_spin_button.set_value(active_state->get_layer_resolution().x);
            _height_spin_button.set_value(active_state->get_layer_resolution().y);
        }
        else
        {
            _width_spin_button.set_upper_limit(int(max_size / float(active_state->get_layer_resolution().x)) * 100);
            _height_spin_button.set_upper_limit(int(max_size / float(active_state->get_layer_resolution().y)) * 100);

            _width_spin_button.set_value(100);
            _height_spin_button.set_value(100);
        }

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);

        _scale_mode = mode;
        set_final_size(active_state->get_layer_resolution().x, active_state->get_layer_resolution().y);
    }

    void ResizeCanvasDialog::set_aspect_ratio_locked(bool b)
    {
        _aspect_ratio_locked = b;
    }

    void ResizeCanvasDialog::set_width(float v)
    {
        _width = v;

        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        size_t final_x = 0;
        size_t final_y = 0;

        if (_scale_mode == ABSOLUTE)
        {
            auto target = v;
            _width_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _height_spin_button.set_value(target * (active_state->get_layer_resolution().y / float(active_state->get_layer_resolution().x)));

            final_x = _width_spin_button.get_value();
            final_y = _height_spin_button.get_value();
        }
        else
        {
            auto target = v;
            _width_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _height_spin_button.set_value(v);

            final_x = _width_spin_button.get_value() / 100 *  active_state->get_layer_resolution().x;
            final_y = _height_spin_button.get_value() / 100 * active_state->get_layer_resolution().y;
        }

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);

        set_final_size(final_x, final_y);
        update_offset_bounds();
        reformat_area();
    }

    void ResizeCanvasDialog::set_height(float v)
    {
        _height = v;

        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        size_t final_x = 0;
        size_t final_y = 0;

        if (_scale_mode == ABSOLUTE)
        {
            auto target = v;
            _height_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _width_spin_button.set_value(target * (active_state->get_layer_resolution().x / float(active_state->get_layer_resolution().y)));

            final_x = _width_spin_button.get_value();
            final_y = _height_spin_button.get_value();
        }
        else
        {
            auto target = v;
            _height_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _width_spin_button.set_value(v);

            final_x = _width_spin_button.get_value() / 100 *  active_state->get_layer_resolution().x;
            final_y = _height_spin_button.get_value() / 100 * active_state->get_layer_resolution().y;
        }

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);

        set_final_size(final_x, final_y);
        update_offset_bounds();
        reformat_area();
    }

    ResizeCanvasDialog::operator Widget*()
    {
        return &_dialog;
    }

    void ResizeCanvasDialog::present()
    {
        set_width(active_state->get_layer_resolution().x);
        set_height(active_state->get_layer_resolution().y);
        set_x_offset(0);
        set_y_offset(0);

        update_current_image_texture();
        _dialog.present();
    }

    ResizeCanvasDialog::ResizeCanvasDialog()
        : _width_spin_button(1, 1, 1),
          _height_spin_button(1, 1, 1),
          _x_offset_button(0, 0, 1),
          _y_offset_button(0, 0, 1)
    {
        set_scale_mode(_scale_mode);
        on_layer_resolution_changed();
        set_final_size(0, 0);

        _width_spin_button.connect_signal_value_changed([](SpinButton* button, ResizeCanvasDialog* instance) {
            instance->set_width(button->get_value());
        }, this);

        _height_spin_button.connect_signal_value_changed([](SpinButton* button, ResizeCanvasDialog* instance) {
            instance->set_height(button->get_value());
        }, this);

        _width_box.push_back(&_width_label);
        _width_box.push_back(&_width_spacer);
        _width_box.push_back(&_width_spin_button);

        _height_box.push_back(&_height_label);
        _height_box.push_back(&_height_spacer);
        _height_box.push_back(&_height_spin_button);

        for (auto* label : {
            &_width_label,
            &_height_label
        })
        {
            label->set_halign(GTK_ALIGN_START);
            label->set_expand(false);
            label->set_margin_end(state::margin_unit);
        }

        for (auto* spacer : {
            &_width_spacer,
            &_height_spacer
        })
        {
            spacer->set_opacity(0);
            spacer->set_expand(true);
            spacer->set_halign(GTK_ALIGN_CENTER);
        }

        for (auto* button : {
            &_width_spin_button,
            &_height_spin_button
        })
        {
            button->set_expand(false);
            button->set_halign(GTK_ALIGN_END);
        }

        _spin_button_box.push_back(&_width_box);
        _spin_button_box.push_back(&_height_box);
        _spin_button_box.set_expand(true);
        _spin_button_box.set_margin_vertical(state::margin_unit);

        _absolute_or_relative_dropdown.push_back(&_absolute_list_label, &_absolute_when_selected_label, [](ResizeCanvasDialog* instance){
            instance->set_scale_mode(ABSOLUTE);
        }, this);

        _absolute_or_relative_dropdown.push_back(&_relative_list_label, &_relative_when_selected_label, [](ResizeCanvasDialog* instance){
            instance->set_scale_mode(RELATIVE);
        }, this);

        _absolute_or_relative_dropdown.set_margin_start(state::margin_unit);
        _absolute_or_relative_dropdown.set_align(GTK_ALIGN_CENTER);
        _absolute_or_relative_dropdown.set_expand(false);

        _absolute_list_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "absolute_dropdown_label"));
        _absolute_when_selected_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "absolute_dropdown_label"));

        _relative_list_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "relative_dropdown_label"));
        _relative_when_selected_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "relative_dropdown_label"));

        _spin_button_and_dropdown_box.push_back(&_spin_button_box);
        _spin_button_and_dropdown_box.push_back(&_absolute_or_relative_dropdown);

        _maintain_aspect_ratio_button.set_active(_aspect_ratio_locked);
        _maintain_aspect_ratio_button.connect_signal_toggled([](CheckButton* button, ResizeCanvasDialog* instance){
            instance->set_aspect_ratio_locked(button->get_active());
        }, this);
        _maintain_aspect_ratio_button.set_margin_horizontal(state::margin_unit);

        _maintain_aspect_ratio_box.push_back(&_maintain_aspect_ratio_button);
        _maintain_aspect_ratio_box.push_back(&_maintain_aspect_ratio_label);

        _final_size_label.set_justify_mode(JustifyMode::LEFT);
        _final_size_label.set_halign(GTK_ALIGN_START);
        _final_size_label.set_margin_top(state::margin_unit);

        _spin_button_and_dropdown_box.set_margin_start(state::margin_unit);
        _maintain_aspect_ratio_box.set_margin_start(state::margin_unit);

        _offset_label.set_halign(GTK_ALIGN_START);

        for (auto* label : {&_x_offset_label, &_y_offset_label})
        {
            label->set_halign(GTK_ALIGN_START);
            label->set_margin_end(state::margin_unit * 2);
        }

        for (auto* box : {&_x_offset_box, &_y_offset_box})
        {
            box->set_margin_horizontal(state::margin_unit * 2);
        }

        _x_offset_button.connect_signal_value_changed([](SpinButton* scale, ResizeCanvasDialog* instance){
            instance->set_x_offset(scale->get_value());
        }, this);

        _y_offset_button.connect_signal_value_changed([](SpinButton* scale, ResizeCanvasDialog* instance){
            instance->set_y_offset(scale->get_value());
        }, this);

        _x_offset_box.push_back(&_x_offset_label);
        _x_offset_box.push_back(&_x_offset_button);
        _y_offset_box.push_back(&_y_offset_label);
        _y_offset_box.push_back(&_y_offset_button);

        _offset_box.push_back(&_x_offset_box);
        _offset_box.push_back(&_y_offset_box);

        _center_button.set_child(&_center_button_label);
        _center_button.connect_signal_clicked([](Button* button, ResizeCanvasDialog* instance){
            instance->center_offset();
        }, this);

        _reset_button.set_child(&_reset_button_label);
        _reset_button.connect_signal_clicked([](Button* button, ResizeCanvasDialog* instance){
            instance->reset_offset();
        }, this);

        _offset_button_box.push_back(&_reset_button);
        _offset_button_box.push_back(&_center_button);

        _offset_button_box.set_margin_top(state::margin_unit);

        for (auto* button : {&_reset_button, &_center_button})
        {
            button->set_margin_horizontal(state::margin_unit);
            button->set_hexpand(true);
        }

        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);

        _area.set_expand(true);
        _aspect_frame.set_child(&_area);

        _window_box.set_homogeneous(false);
        _window_box.push_back(&_instruction_label);
        _window_box.push_back(&_spin_button_and_dropdown_box);
        _window_box.push_back(&_maintain_aspect_ratio_box);
        _window_box.push_back(&_final_size_label);

        _spin_button_and_dropdown_box.set_vexpand(false);

        {
            auto* spacer = new SeparatorLine();
            spacer->set_size_request({0, 3});
            spacer->set_vexpand(false);
            spacer->set_margin_top(state::margin_unit);
            _window_box.push_back(spacer);
        }

        _window_box.push_back(&_offset_label);
        _window_box.push_back(&_offset_box);
        _window_box.push_back(&_offset_button_box);

        {
            auto* spacer = new SeparatorLine();
            spacer->set_size_request({0, 3});
            spacer->set_vexpand(false);
            spacer->set_margin_vertical(state::margin_unit);
            _window_box.push_back(spacer);
        }

        _window_box.push_back(&_preview_label);
        _window_box.push_back(&_aspect_frame);
        _aspect_frame.set_margin(state::margin_unit);

        {
            auto* spacer = new SeparatorLine();
            spacer->set_size_request({0, 3});
            spacer->set_vexpand(false);
            _window_box.push_back(spacer);
        }

        _instruction_label.set_halign(GTK_ALIGN_START);
        _preview_label.set_halign(GTK_ALIGN_START);

        for (auto* label : {&_offset_label})
            label->set_margin_vertical(state::margin_unit);

        _window_box.set_margin(state::margin_unit);

        _accept_button.set_child(&_accept_button_label);
        _cancel_button.set_child(&_cancel_button_label);

        _dialog.get_content_area().push_back(&_window_box);

        _dialog.add_action_widget(&_cancel_button, [](ResizeCanvasDialog* instance){
            instance->_dialog.close();
        }, this);

        _dialog.add_action_widget(&_accept_button,[](ResizeCanvasDialog* instance){

            auto width_value = instance->_width_spin_button.get_value();
            auto height_value = instance->_height_spin_button.get_value();

            auto width = instance->_scale_mode == ABSOLUTE ? width_value  : (width_value / 100) * active_state->get_layer_resolution().x;
            auto height = instance->_scale_mode == ABSOLUTE ? height_value  : (height_value / 100) * active_state->get_layer_resolution().y;

            active_state->resize_canvas(
                Vector2ui(round(width), round(height)),
                Vector2i(instance->_x_offset, instance->_y_offset)
            );

            instance->_dialog.close();
        }, this);

        auto button_size = std::max<float>(_accept_button.get_preferred_size().natural_size.x, _cancel_button.get_preferred_size().natural_size.x);
        for (auto* button : {&_accept_button, &_cancel_button})
        {
            button->set_margin_end(state::margin_unit);
            button->set_size_request({button_size, 0});
        }

        _accept_button.set_margin_end(state::margin_unit);
        _cancel_button.set_margin_end(state::margin_unit);
        GtkWidget* parent = gtk_widget_get_parent(_accept_button.operator GtkWidget*());
        gtk_widget_set_margin_start(parent, state::margin_unit);
        gtk_widget_set_margin_bottom(parent, state::margin_unit);

        state::actions::resize_canvas_dialog_open.set_function([](){
            state::resize_canvas_dialog->present();
        });
        state::add_shortcut_action(state::actions::resize_canvas_dialog_open);
    }

    void ResizeCanvasDialog::update_current_image_texture()
    {
        if (_current_canvas_texture == nullptr)
            return;

        std::set<size_t> layers;
        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
            layers.insert(i);

        auto whole_image = state::canvas_export->merge_layers(layers, active_state->get_current_frame_index());
        _current_canvas_texture->create_from_image(whole_image);

        _aspect_frame.set_ratio(active_state->get_layer_resolution().x / float(active_state->get_layer_resolution().y));
        _area.queue_render();
    }

    void ResizeCanvasDialog::on_area_realize(Widget* widget, ResizeCanvasDialog* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_new_boundary_shape = new Shape();
        instance->_current_canvas_shape = new Shape();
        instance->_area_frame_shape = new Shape();
        instance->_current_canvas_texture = new Texture();

        instance->update_current_image_texture();
        instance->_current_canvas_shape->set_texture(instance->_current_canvas_texture);
        instance->reformat_area();

        instance->_area.clear_render_tasks();
        instance->_area.add_render_task(instance->_area_frame_shape);
        instance->_area.add_render_task(instance->_current_canvas_shape);
        instance->_area.add_render_task(instance->_new_boundary_shape);

        area->queue_render();
    }

    void ResizeCanvasDialog::on_area_resize(GLArea*, int w, int h, ResizeCanvasDialog* instance)
    {
        instance->_area_size = {w, h};
        auto res = active_state->get_layer_resolution();

        instance->reformat_area();
        instance->_area.queue_render();
    }

    void ResizeCanvasDialog::reformat_area()
    {
        if (not _area.get_is_realized())
            return;

        float x_eps = 1.f / _area_size.x;
        float y_eps = 1.f / _area_size.y;

        Vector2f old_top_left = {0, 0};
        Vector2f old_size = {1, 1};
        Vector2f new_top_left = {0, 0};
        Vector2f new_size = {1, 1};

        float new_w = _width;
        float old_w = active_state->get_layer_resolution().x;

        float new_h = _height;
        float old_h = active_state->get_layer_resolution().y;

        std::cout << new_w << " " << old_w << " " << new_h << " " << old_h << std::endl;

        if (new_w < old_w)
        {
            std::cout << "called" << std::endl;

            new_top_left.x = _x_offset / old_w;
            new_size.x = new_w / old_w;

            old_top_left.x = 0;
            old_size.x = 1;
        }
        else if (new_w > old_w)
        {}

        if (new_h < old_h)
        {
            new_top_left.y = _y_offset / old_h;
            new_size.y = new_h / old_h;

            old_top_left.y = 0;
            old_size.y = 1;
        }
        else
        {}

        std::cout << "new: " << new_top_left.x << "  " << new_top_left.y << " | " << new_size.x << " " << new_size.y << std::endl;
        std::cout << "old: " << old_top_left.x << "  " << old_top_left.y << " | " << old_size.x << " " << old_size.y << std::endl;

        _current_canvas_shape->as_rectangle(old_top_left, old_size);
        _current_canvas_shape->set_texture(_current_canvas_texture);

        {
            float x = new_top_left.x + x_eps;
            float y = new_top_left.y + y_eps;
            float w = new_size.x - 2 * x_eps;
            float h = new_size.y - 2 * y_eps;

           _new_boundary_shape->as_lines(
           {
                // main frame
                {{x + 0, y + 0}, {x + w, y + 0}},
                {{x + w, y + 0}, {x + w, y + h}},
                {{x + w, y + h}, {x + 0, y + h}},
                {{x + 0, y + h}, {x + 0, y + 0}},

                // outer outline
                {{x - x_eps, y  - y_eps}, {x + w + x_eps , y - y_eps}},
                {{x + w + x_eps, y - y_eps}, {x + w + x_eps, y + h + y_eps}},
                {{x + w + x_eps, y + h + y_eps}, {x - x_eps, y + h + y_eps}},
                {{x - x_eps, y + h + y_eps}, {x - x_eps, y - y_eps}},

                // inner outline
                {{x + x_eps, y + y_eps}, {x + w - x_eps, y + y_eps}},
                {{x + w - x_eps, y + y_eps}, {x + w - x_eps, y + h - y_eps}},
                {{x + w - x_eps, y + h - y_eps}, {x + x_eps, y + h - y_eps}},
                {{x + x_eps, y + h - y_eps}, {x + x_eps, y + y_eps}},
            });

            const size_t n_vertices = 8;
            for (size_t i = 0; i < n_vertices; i++)
                _new_boundary_shape->set_vertex_color(i, RGBA(1, 1, 1, 1));

            for (size_t i = n_vertices; i < n_vertices * 3; ++i)
                _new_boundary_shape->set_vertex_color(i, RGBA(0, 0, 0, 1));

            _area_frame_shape->as_rectangle(
                 {0 + x_eps, 0 + y_eps},
                 {1 - x_eps, 0 + y_eps},
                 {1 - x_eps, 1 - y_eps},
                 {0 + x_eps, 1 - y_eps}
            );
            _area_frame_shape->set_color(RGBA(0, 0, 0, 0.5));
        }

        _area.queue_render();
    }
}
