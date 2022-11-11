// 
// Copyright 2022 Clemens Cords
// Created on 11/10/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class SaveAsDialog
    {
        public:
            SaveAsDialog(const std::string& window_title);

            template<typename Function_t, typename Arg_t>
            void set_on_ok_pressed(Function_t, Arg_t);

            template<typename Function_t, typename Arg_t>
            void set_on_cancel_pressed(Function_t, Arg_t);

            FileChooser& get_file_chooser();
            Dialog& get_dialog();

            std::string get_current_name();
            std::string get_current_path();

        private:
            Window _window;
            Dialog _dialog = Dialog(&_window, "Save as...");

            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            Box _name_entry_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Entry _name_entry;
            Label _name_entry_label = Label("Name: ");

            std::function<void(SaveAsDialog*)> _on_ok_pressed;
            std::function<void(SaveAsDialog*)> _on_cancel_pressed;

            FileChooser _file_chooser = FileChooser();
            std::string _previously_selected_path = "";

            KeyEventController _key_event_controller;

            Button _ok_button;
            Label _ok_button_label = Label("OK");

            Button _cancel_button;
            Label _cancel_button_label = Label("Cancel");
    };
}

//

namespace mousetrap
{
    SaveAsDialog::SaveAsDialog(const std::string& window_title)
    {
        _window.set_title(window_title);

        _name_entry.connect_signal_activate([&](Widget*, SaveAsDialog* instance){
            if (instance->_on_ok_pressed)
                instance->_on_ok_pressed(instance);
        }, this);
        _name_entry.set_hexpand(true);

        _name_entry_label.set_margin_start(state::margin_unit);
        _name_entry.set_margin_start(state::margin_unit);
        _name_entry.set_margin_end(2 * state::margin_unit);
        _name_entry.set_margin_bottom(state::margin_unit);

        _name_entry_box.push_back(&_name_entry_label);
        _name_entry_box.push_back(&_name_entry);
        _name_entry_box.set_vexpand(false);

        _file_chooser.set_expand(true);
        _file_chooser.set_focus_on_click(true);
        _name_entry.set_focus_on_click(true);

        _main.push_back(&_name_entry_box);
        _main.push_back(&_file_chooser);
        _main.set_margin_vertical(state::margin_unit);

        _dialog.get_content_area().push_back(&_main);

        _ok_button_label.set_margin_horizontal(state::margin_unit);
        _cancel_button_label.set_margin_horizontal(state::margin_unit);

        _ok_button.set_child(&_ok_button_label);
        _cancel_button.set_child(&_cancel_button_label);

        auto size = std::max(
            _ok_button_label.get_preferred_size().natural_size.x,
            _cancel_button_label.get_preferred_size().natural_size.x
        );

        _ok_button.set_size_request({size, 0});
        _cancel_button.set_size_request({size, 0});

        _dialog.add_action_widget(&_cancel_button, [](SaveAsDialog* instance){
            if (instance->_on_cancel_pressed)
                instance->_on_cancel_pressed(instance);
        }, this);

        _dialog.add_action_widget(&_ok_button, [](SaveAsDialog* instance){
            if (instance->_on_ok_pressed)
                instance->_on_ok_pressed(instance);
        }, this);

        auto* temp = gtk_widget_get_parent(_cancel_button.operator GtkWidget*());
        gtk_widget_set_margin_bottom(temp, state::margin_unit);
        gtk_widget_set_margin_end(temp, state::margin_unit);
        gtk_box_set_spacing(GTK_BOX(temp), state::margin_unit);

        // TODO: Workaround until https://discourse.gnome.org/t/gtk4-filechooser-selection-changed-signal-alternative/12180 is answered
        _file_chooser.add_tick_callback([](FrameClock clock, SaveAsDialog* instance) -> bool {

            auto selected = instance->_file_chooser.get_selected();
            if (not instance->_name_entry.get_has_focus() and
                not selected.empty() and
                selected.at(0).is_file() and
                instance->_previously_selected_path != selected.at(0).get_name())
            {
                auto name = selected.at(0).get_name();
                instance->_name_entry.set_text(name);
                instance->_previously_selected_path = name;
            }

            return true;
        }, this);


        _key_event_controller.connect_signal_key_pressed([](KeyEventController*, guint keyval, guint keycode, GdkModifierType state, SaveAsDialog*) -> bool {

            //if (keyval == GDK_KEY_Return)
                std::cout << "activate" << std::endl;

            return false;
        }, this);
        _main.add_controller(&_key_event_controller);
    }

    template<typename Function_t, typename Arg_t>
    void SaveAsDialog::set_on_ok_pressed(Function_t f_in, Arg_t arg_in)
    {
        _on_ok_pressed = [f = f_in, arg = arg_in, this](SaveAsDialog* instance){
            f(instance, arg);
        };
    }

    template<typename Function_t, typename Arg_t>
    void SaveAsDialog::set_on_cancel_pressed(Function_t f_in, Arg_t arg_in)
    {
        _on_cancel_pressed = [f = f_in, arg = arg_in, this](SaveAsDialog* instance){
            f(instance, arg);
        };
    }

    FileChooser& SaveAsDialog::get_file_chooser()
    {
        return _file_chooser;
    }

    std::string SaveAsDialog::get_current_name()
    {
        return _name_entry.get_text();
    }

    std::string SaveAsDialog::get_current_path()
    {
        return _file_chooser.get_current_folder().get_name() + "/" + get_current_name();
    }

    Dialog& SaveAsDialog::get_dialog()
    {
        return _dialog;
    }
}