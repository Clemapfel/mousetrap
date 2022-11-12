// 
// Copyright 2022 Clemens Cords
// Created on 11/10/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    // TODO: select with preview: https://stackoverflow.com/questions/2817763/finding-file-icon-given-a-mime-type-using-gtk

    class SaveAsDialog
    {
        public:
            SaveAsDialog(const std::string& window_title);

            template<typename Function_t, typename Arg_t>
            void set_on_ok_pressed(Function_t, Arg_t);

            template<typename Function_t, typename Arg_t>
            void set_on_cancel_pressed(Function_t, Arg_t);

            FileChooser& get_file_chooser();

            std::string get_current_name();
            std::string get_current_path();

            void show();
            void close();

        private:
            Window _window;
            Dialog _dialog;

            Frame _main_frame;
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            Box _name_entry_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Entry _name_entry;
            Label _name_entry_label = Label("Name: ");

            std::function<void(SaveAsDialog*)> _on_ok_pressed;
            std::function<void(SaveAsDialog*)> _on_cancel_pressed;

            FileChooser _file_chooser;
            std::string _previously_selected_path = "";

            Button _ok_button;
            Label _ok_button_label = Label("OK");

            Button _cancel_button;
            Label _cancel_button_label = Label("Cancel");

            Dialog _warn_on_override_dialog = Dialog(&_window, "Warning");

            ScrolledWindow  _warn_on_override_content_box;
            Label _warn_on_override_content;

            Button _warn_on_override_continue_button;
            Label  _warn_on_override_continue_label = Label("Continue");
            Button _warn_on_override_cancel_button;
            Label  _warn_on_override_cancel_label = Label("Cancel");
    };

    class FilePreview : public AppComponent
    {
        public:
            FilePreview();

            operator Widget*() override;
            void update();
            void update_from(FileDescriptor*);

        private:
            size_t preview_icon_pixel_size_factor = state::settings_file->get_value_as<float>("file_chooser_dialog.file_preview", "preview_icon_pixel_size_factor");

            FileDescriptor* _file;

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
            ScrolledWindow _window;

            GtkImage* _icon_image;

            Box _icon_image_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Label _file_name_label;
            Label _file_type_label;
            Label _file_size_label;
    };

    class OpenDialog
    {
        public:
            OpenDialog(const std::string& window_title);

            template<typename Function_t, typename Arg_t>
            void set_on_open_pressed(Function_t, Arg_t);

            template<typename Function_t, typename Arg_t>
            void set_on_cancel_pressed(Function_t, Arg_t);

            FileChooser& get_file_chooser();
            Dialog& get_dialog();

        private:
            Window _window;
            Dialog _dialog;

            FilePreview _file_preview;
            Frame _preview_frame;
            Label _preview_label = Label("<span weight=\"bold\" color=\"#AAAAAA\">Preview</span>");

            std::string _previously_selected_path = "";

            Box _main = Box(GTK_ORIENTATION_HORIZONTAL, 0.5 * state::margin_unit);

            std::function<void(OpenDialog*)> _on_open_pressed;
            std::function<void(OpenDialog*)> _on_cancel_pressed;

            Frame _file_chooser_frame;
            FileChooser _file_chooser;

            Button _open_button;
            Label _open_button_label = Label("Open");

            Button _cancel_button;
            Label _cancel_button_label = Label("Cancel");
    };
}

//

namespace mousetrap
{
    SaveAsDialog::SaveAsDialog(const std::string& window_title)
        : _dialog(&_window, window_title)
    {
        _window.set_titlebar_layout("close");
        _name_entry.connect_signal_activate([&](Widget*, SaveAsDialog* instance){
            if (instance->_on_ok_pressed)
                instance->_on_ok_pressed(instance);
        }, this);
        _name_entry.set_hexpand(true);
        _name_entry.set_vexpand(false);

        _name_entry_label.set_margin_start(state::margin_unit);

        _name_entry.set_margin_start(state::margin_unit);
        _name_entry.set_margin_end(2 * state::margin_unit);
        _name_entry.set_margin_bottom(state::margin_unit);

        _name_entry_box.push_back(&_name_entry_label);
        _name_entry_box.push_back(&_name_entry);
        _name_entry_box.set_vexpand(false);
        _name_entry_box.set_hexpand(true);

        _file_chooser.set_expand(true);
        _file_chooser.set_focus_on_click(true);
        _file_chooser.add_boolean_choice("WARN_ON_OVERRIDE", "Warn if file already exists", true);
        _name_entry.set_focus_on_click(true);

        _main_frame.set_label_widget(nullptr);
        _main_frame.set_child(&_file_chooser);
        _main_frame.set_margin_horizontal(state::margin_unit);

        _main.push_back(&_name_entry_box);
        _main.push_back(&_main_frame);
        _main.set_margin_vertical(state::margin_unit);

        _dialog.get_content_area().push_back(&_main);

        _warn_on_override_content_box.set_policy(GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        _warn_on_override_content_box.set_propagate_natural_height(true);
        _warn_on_override_content_box.set_propagate_natural_width(true);
        _warn_on_override_content_box.set_expand(true);
        _warn_on_override_content.set_wrap_mode(LabelWrapMode::ONLY_ON_WORD);
        _warn_on_override_content.set_justify_mode(JustifyMode::LEFT);
        _warn_on_override_content.set_expand(false);

        _warn_on_override_content_box.set_child(&_warn_on_override_content);
        _warn_on_override_dialog.get_content_area().push_back(&_warn_on_override_content_box);
        _warn_on_override_dialog.get_content_area().set_expand(true);
        _warn_on_override_content_box.set_margin(2* state::margin_unit);

        _ok_button_label.set_margin_horizontal(state::margin_unit);
        _cancel_button_label.set_margin_horizontal(state::margin_unit);
        _warn_on_override_continue_label.set_margin_horizontal(state::margin_unit);
        _warn_on_override_cancel_label.set_margin_horizontal(state::margin_unit);

        _ok_button.set_child(&_ok_button_label);
        _cancel_button.set_child(&_cancel_button_label);
        _warn_on_override_continue_button.set_child(&_warn_on_override_continue_label);
        _warn_on_override_cancel_button.set_child(&_warn_on_override_cancel_label);

        auto size = std::max(
            _ok_button_label.get_preferred_size().natural_size.x,
            _cancel_button_label.get_preferred_size().natural_size.x
        );

        _ok_button_label.set_size_request({size, 0});
        _cancel_button_label.set_size_request({size, 0});

        size = std::max(
            _warn_on_override_continue_label.get_preferred_size().natural_size.x,
            _warn_on_override_cancel_label.get_preferred_size().natural_size.x
        );

        _warn_on_override_continue_label.set_size_request({size, 0});
        _warn_on_override_cancel_label.set_size_request({size, 0});

        _warn_on_override_dialog.add_action_widget(&_warn_on_override_cancel_button, [](SaveAsDialog* instance){
            instance->_warn_on_override_dialog.close();
        }, this);

        _warn_on_override_dialog.add_action_widget(&_warn_on_override_continue_button, [](SaveAsDialog* instance){
            if (instance->_on_ok_pressed)
                instance->_on_ok_pressed(instance);

            instance->_warn_on_override_dialog.close();
        }, this);

        _dialog.add_action_widget(&_cancel_button, [](SaveAsDialog* instance){
            if (instance->_on_cancel_pressed)
                instance->_on_cancel_pressed(instance);
        }, this);

        _dialog.add_action_widget(&_ok_button, [](SaveAsDialog* instance){

            if (instance->_file_chooser.get_boolean_choice("WARN_ON_OVERRIDE"))
            {
                instance->_warn_on_override_content.set_text("<b>A file named `" + instance->_name_entry.get_text() + "` already exists. Do you want to replace it?</b>\n\nThis will override the files contents. This operation cannot be undone, continue anyway?");
                instance->_warn_on_override_dialog.present();
            }
            else
            {
                if (instance->_on_ok_pressed)
                    instance->_on_ok_pressed(instance);
            }
        }, this);

        auto* temp = gtk_widget_get_parent(_cancel_button.operator GtkWidget*());
        gtk_widget_set_margin_bottom(temp, state::margin_unit);
        gtk_widget_set_margin_end(temp, state::margin_unit);
        gtk_box_set_spacing(GTK_BOX(temp), state::margin_unit);

        temp = gtk_widget_get_parent(_warn_on_override_cancel_button.operator GtkWidget*());
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

            instance->_ok_button.set_can_respond_to_input(not instance->_name_entry.get_text().empty());

            return true;
        }, this);
    }

    void SaveAsDialog::show()
    {
        _dialog.show();
    }

    void SaveAsDialog::close()
    {
        _warn_on_override_dialog.close();
        _dialog.close();
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
}

namespace mousetrap
{
    FilePreview::FilePreview()
    {
        _icon_image_box.set_halign(GTK_ALIGN_CENTER);

        _main.push_back(&_icon_image_box);
        _main.push_back(&_file_name_label);
        _main.push_back(&_file_type_label);
        _main.push_back(&_file_size_label);

        _main.set_margin_horizontal(state::margin_unit);

        _file_name_label.set_halign(GTK_ALIGN_CENTER);
        _file_name_label.set_ellipsize_mode(EllipsizeMode::END);
        _file_type_label.set_ellipsize_mode(EllipsizeMode::START);

        for (auto* label : {&_file_name_label, &_file_type_label, &_file_size_label})
        {
            label->set_wrap_mode(LabelWrapMode::ONLY_ON_CHAR);
            label->set_max_width_chars(8);
            auto* glabel = label->operator GtkLabel*();
        }

        _file_name_label.set_margin_bottom(state::margin_unit);
        _main.set_hexpand(false);
        _main.set_vexpand(true);

        _window.set_size_request({preview_icon_pixel_size_factor * state::margin_unit, 0});
        _window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        _window.set_propagate_natural_height(true);
        _window.set_child(&_main);
    }

    FilePreview::operator Widget*()
    {
        return &_window;
    }

    void FilePreview::update()
    {
        update_from(_file);
    }

    void FilePreview::update_from(FileDescriptor* file)
    {
        _file = file;

        if (_file == nullptr)
            return;

        auto* pixbuf_maybe = gdk_pixbuf_new_from_file(_file->get_path().c_str(), nullptr);

        _icon_image_box.clear();
        if (pixbuf_maybe != nullptr)
        {
            auto target_width = preview_icon_pixel_size_factor * state::margin_unit;
            auto target_height = (gdk_pixbuf_get_height(pixbuf_maybe) / float(gdk_pixbuf_get_width(pixbuf_maybe))) * target_width;

            auto* pixbuf_scaled = gdk_pixbuf_scale_simple(pixbuf_maybe, target_width, target_height, GDK_INTERP_NEAREST);
            _icon_image = GTK_IMAGE(gtk_image_new_from_pixbuf(pixbuf_scaled));

            gtk_widget_set_size_request(GTK_WIDGET(_icon_image), target_width, target_height);
            gtk_widget_set_halign(GTK_WIDGET(_icon_image), GTK_ALIGN_CENTER);
            gtk_box_append(_icon_image_box.operator GtkBox*(), GTK_WIDGET(_icon_image));

            g_object_unref(pixbuf_maybe);
            g_object_unref(pixbuf_scaled);
        }
        else
        {
            auto* info = g_file_query_info(_file->operator GFile*(), G_FILE_ATTRIBUTE_PREVIEW_ICON, G_FILE_QUERY_INFO_NONE, nullptr, nullptr);
            auto* icon = g_content_type_get_icon(_file->get_content_type().c_str());

            if (icon != nullptr)
            {
                _icon_image = GTK_IMAGE(gtk_image_new_from_gicon(G_ICON(icon)));
                gtk_widget_set_halign(GTK_WIDGET(_icon_image), GTK_ALIGN_CENTER);
                gtk_image_set_pixel_size(_icon_image, preview_icon_pixel_size_factor * state::margin_unit);
                gtk_box_append(_icon_image_box.operator GtkBox*(), GTK_WIDGET(_icon_image));
            }
        }

        std::stringstream file_name_text;
        file_name_text << "<span weight=\"bold\" size=\"100%\">"
                       << _file->query_info(G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME)
                       << "</span>";

        _file_name_label.set_text(file_name_text.str());
        _file_type_label.set_text(_file->query_info(G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE));

        size_t size_byte = 0;

        try {
            size_byte = std::stoi(_file->query_info(G_FILE_ATTRIBUTE_STANDARD_ALLOCATED_SIZE));
        }
        catch (...)
        {}

        _file_size_label.set_text(_file->is_folder() ? "" : g_format_size(size_byte));
    }
}

namespace mousetrap
{
    OpenDialog::OpenDialog(const std::string& window_title)
        : _dialog(&_window, window_title)
    {
        _window.set_title(window_title);

        _file_chooser.set_expand(true);
        _file_chooser_frame.set_child(&_file_chooser);
        _file_chooser_frame.set_label_widget(nullptr);
        _file_chooser_frame.set_margin_end(state::margin_unit * 0.5);

        _preview_frame.set_child(_file_preview.operator Widget*());
        _preview_frame.set_margin_start(state::margin_unit * 0.5);
        _preview_frame.set_label_widget(&_preview_label);

        _main.set_margin_horizontal(state::margin_unit);
        _main.push_back(&_file_chooser_frame);
        _main.push_back(&_preview_frame);

        _main.set_margin_vertical(state::margin_unit);

        _dialog.get_content_area().push_back(&_main);

        _open_button_label.set_margin_horizontal(state::margin_unit);
        _cancel_button_label.set_margin_horizontal(state::margin_unit);

        _open_button.set_child(&_open_button_label);
        _cancel_button.set_child(&_cancel_button_label);

        auto size = std::max(
            _open_button_label.get_preferred_size().natural_size.x,
            _cancel_button_label.get_preferred_size().natural_size.x
        );

        _open_button.set_size_request({size, 0});
        _cancel_button.set_size_request({size, 0});

        _dialog.add_action_widget(&_cancel_button, [](OpenDialog* instance){
            if (instance->_on_cancel_pressed)
                instance->_on_cancel_pressed(instance);
        }, this);

        _dialog.add_action_widget(&_open_button, [](OpenDialog* instance){
            if (instance->_on_open_pressed)
                instance->_on_open_pressed(instance);
        }, this);

        auto* temp = gtk_widget_get_parent(_cancel_button.operator GtkWidget*());
        gtk_widget_set_margin_bottom(temp, state::margin_unit);
        gtk_widget_set_margin_end(temp, state::margin_unit);
        gtk_box_set_spacing(GTK_BOX(temp), state::margin_unit);

        _file_chooser.add_tick_callback([](FrameClock clock, OpenDialog* instance) -> bool {

            auto selected = instance->_file_chooser.get_selected();
            instance->_open_button.set_can_respond_to_input(not selected.empty());

            if (not selected.empty() and selected.at(0).get_name() != instance->_previously_selected_path)
            {
                auto file = selected.at(0);
                instance->_previously_selected_path = file.get_name();
                instance->_file_preview.update_from(&file);
            }

            return true;
        }, this);
    }

    template<typename Function_t, typename Arg_t>
    void OpenDialog::set_on_open_pressed(Function_t f_in, Arg_t arg_in)
    {
        _on_open_pressed = [f = f_in, arg = arg_in, this](OpenDialog* instance){
            f(instance, arg);
        };
    }

    template<typename Function_t, typename Arg_t>
    void OpenDialog::set_on_cancel_pressed(Function_t f_in, Arg_t arg_in)
    {
        _on_cancel_pressed = [f = f_in, arg = arg_in, this](OpenDialog* instance){
            f(instance, arg);
        };
    }

    FileChooser& OpenDialog::get_file_chooser()
    {
        return _file_chooser;
    }

    Dialog& OpenDialog::get_dialog()
    {
        return _dialog;
    }
}