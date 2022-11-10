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

            FileChooser _file_chooser = FileChooser(FileChooserAction::TODO);

            //static void on_file_chooser_file_selection_changed(FileChooser*, SaveAsDialog*);
            static void on_file_chooser_file_activated(FileChooser*, SaveAsDialog*);
    };
}

//

namespace mousetrap
{
    SaveAsDialog::SaveAsDialog(const std::string& window_title)
    {
        _window.set_title(window_title);

        _name_entry.connect_signal_activate([&](Widget*, SaveAsDialog* instance){
            _name_entry.set_text(instance->get_file_chooser().get_current_name());
            instance->_on_ok_pressed(instance);
        }, this);
        _name_entry.set_hexpand(true);

        _name_entry_label.set_margin_start(state::margin_unit);
        _name_entry.set_margin_start(state::margin_unit);
        _name_entry.set_margin_end(2 * state::margin_unit);

        _name_entry_box.push_back(&_name_entry_label);
        _name_entry_box.push_back(&_name_entry);
        _name_entry_box.set_vexpand(false);

        _file_chooser.set_expand(true);
        _file_chooser.set_focus_on_click(true);
        _name_entry.set_focus_on_click(true);

        _main.push_back(&_name_entry_box);
        _main.push_back(&_file_chooser);

        _dialog.get_content_area().push_back(&_main);

        _dialog.add_action_button("OK", [](SaveAsDialog* instance){
            if (instance->_on_ok_pressed)
                instance->_on_ok_pressed(instance);
        }, this);

        _dialog.add_action_button("Cancel", [](SaveAsDialog* instance){
            if (instance->_on_cancel_pressed)
                instance->_on_cancel_pressed(instance);
        }, this);

        _file_chooser.add_tick_callback([](FrameClock clock, SaveAsDialog* instance) -> bool {

            auto selected = instance->_file_chooser.get_selected();
            if (not selected.empty() and selected.at(0).is_file())
                instance->_name_entry.set_text(selected.at(0).get_name());

            return true;
        }, this);
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

    /*
    void SaveAsDialog::on_file_chooser_file_selection_changed(FileChooser* file_chooser, SaveAsDialog* instance)
    {
        auto selected = file_chooser->get_selected();

        if (not selected.empty() and selected.at(0).is_file())
            instance->_name_entry.set_text(selected.at(0).get_name());
    }
     */

    void SaveAsDialog::on_file_chooser_file_activated(FileChooser* file_chooser, SaveAsDialog* instance)
    {
        if (instance->_on_ok_pressed)
            instance->_on_ok_pressed(instance);
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