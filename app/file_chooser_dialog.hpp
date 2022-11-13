// 
// Copyright 2022 Clemens Cords
// Created on 11/12/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/global_state.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
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

    enum class FileChooserDialogMode
    {
        OPEN,
        SAVE_AS,
        CHOOSE_FOLDER
    };
    
    template<FileChooserDialogMode Mode>
    class FileChooserDialog
    {
        public:
            FileChooserDialog(const std::string& window_title);

            template<typename Function_t, typename Arg_t>
            void set_on_accept_pressed(Function_t, Arg_t);

            template<typename Function_t, typename Arg_t>
            void set_on_cancel_pressed(Function_t, Arg_t);

            FileChooser& get_file_chooser();
            std::string get_current_name();

            void show();
            void close();
            
        private:
            Window _window;
            Dialog _dialog;

            Button _accept_button;
            Label _accept_button_label = Label(Mode == FileChooserDialogMode::SAVE_AS ? "OK" : "Open");

            Button _cancel_button;
            Label _cancel_button_label = Label("Cancel");

            MenuButton _help_button;
            Popover _help_popover;
            Box _help_popover_content = Box(GTK_ORIENTATION_HORIZONTAL);

            SeparatorLine _button_separator;

            FilePreview _file_preview;
            Frame _preview_frame;
            Label _preview_label = Label("<span weight=\"bold\" color=\"#AAAAAA\">Preview</span>");
            
            Frame _file_chooser_frame;
            FileChooser _file_chooser = FileChooser(
                Mode == FileChooserDialogMode::CHOOSE_FOLDER ? FileChooserAction::SELECT_FOLDER : FileChooserAction::SELECT_FILE
            );

            Box _name_entry_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Entry _name_entry;
            Label _name_entry_label = Label("Name: ");
            
            Box _content_area = Box(GTK_ORIENTATION_VERTICAL);
            Box _file_chooser_preview_area = Box(GTK_ORIENTATION_HORIZONTAL);

            std::string _previously_selected_path = "";

            std::function<void(FileChooserDialog*)> _on_accept_pressed;
            std::function<void(FileChooserDialog*)> _on_cancel_pressed;
            
            // warn if overriding file dialog:

            Dialog _warn_on_override_dialog = Dialog(&_window, "Warning");

            Viewport  _warn_on_override_content_box;
            Label _warn_on_override_content;

            Button _warn_on_override_continue_button;
            Label  _warn_on_override_continue_label = Label("Continue");
            Button _warn_on_override_cancel_button;
            Label  _warn_on_override_cancel_label = Label("Cancel");
    };
    
    using SaveAsFileDialog = FileChooserDialog<FileChooserDialogMode::SAVE_AS>;
    using OpenFileDialog = FileChooserDialog<FileChooserDialogMode::OPEN>;
    using ChooseFolderDialog = FileChooserDialog<FileChooserDialogMode::CHOOSE_FOLDER>;
}

///

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
        _file_size_label.set_ellipsize_mode(EllipsizeMode::NONE);

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
            auto* icon = g_content_type_get_icon(_file->query_info(G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE).c_str());

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

        _file_size_label.set_text(_file->is_folder() ? g_format_size(size_byte) : g_format_size(size_byte));
    }
}

namespace mousetrap
{
    template<FileChooserDialogMode M>
    FileChooserDialog<M>::FileChooserDialog(const std::string& window_title)
        : _dialog(&_window, [&]() -> std::string
        {
            if (not window_title.empty())
                return window_title;

            if (M == FileChooserDialogMode::OPEN)
                return "Open File...";
            else if (M == FileChooserDialogMode::SAVE_AS)
                return "Save As...";
            else if (M == FileChooserDialogMode::CHOOSE_FOLDER)
                return "Select Folder...";
        }())
    {
        _window.set_titlebar_layout("close");
        
        _file_chooser_frame.set_child(&_file_chooser);
        _file_chooser_frame.set_label_widget(nullptr);
        _file_chooser_frame.set_margin_end(state::margin_unit * 0.5);

        _file_chooser.set_expand(true);
        _file_chooser.set_focus_on_click(true);
        _file_chooser.set_can_create_folders(M != FileChooserDialogMode::OPEN);

        if (M == FileChooserDialogMode::SAVE_AS)
            _file_chooser.add_boolean_choice("WARN_ON_OVERRIDE", "Warn if file already exists", true);

        _preview_frame.set_child(_file_preview.operator Widget*());
        _preview_frame.set_margin_start(state::margin_unit * 0.5);
        _preview_frame.set_label_widget(&_preview_label);

        _file_chooser_preview_area.push_back(&_file_chooser_frame);
        _file_chooser_preview_area.push_back(&_preview_frame);
        _file_chooser_preview_area.set_margin_top(state::margin_unit);

        _name_entry.connect_signal_activate([&](Widget*, FileChooserDialog* instance){
            if (instance->_on_accept_pressed)
                instance->_on_accept_pressed(instance);
        }, this);
        _name_entry.set_hexpand(true);
        _name_entry.set_vexpand(false);

        _name_entry.set_margin_start(state::margin_unit);
        _name_entry.set_focus_on_click(true);

        _name_entry_box.push_back(&_name_entry_label);
        _name_entry_box.push_back(&_name_entry);
        _name_entry_box.set_vexpand(false);
        _name_entry_box.set_hexpand(true);
        _name_entry_box.set_margin_start(state::margin_unit);

        if (M == FileChooserDialogMode::SAVE_AS)
            _content_area.push_back(&_name_entry_box);
        
        _content_area.push_back(&_file_chooser_preview_area);
        _content_area.set_margin(state::margin_unit);

        _dialog.get_content_area().push_back(&_content_area);

        _warn_on_override_content.set_wrap_mode(LabelWrapMode::ONLY_ON_WORD);
        _warn_on_override_content.set_justify_mode(JustifyMode::LEFT);
        _warn_on_override_content.set_expand(false);
        _warn_on_override_content.set_halign(GTK_ALIGN_START);

        _warn_on_override_content_box.set_margin(state::margin_unit);
        _warn_on_override_content_box.set_child(&_warn_on_override_content);
        _warn_on_override_dialog.get_content_area().push_back(&_warn_on_override_content_box);
        _warn_on_override_dialog.get_content_area().set_expand(true);
        _warn_on_override_content_box.set_margin(2* state::margin_unit);

        _accept_button_label.set_margin_horizontal(state::margin_unit);
        _cancel_button_label.set_margin_horizontal(state::margin_unit);
        _warn_on_override_continue_label.set_margin_horizontal(state::margin_unit);
        _warn_on_override_cancel_label.set_margin_horizontal(state::margin_unit);

        _accept_button.set_child(&_accept_button_label);
        _cancel_button.set_child(&_cancel_button_label);
        _warn_on_override_continue_button.set_child(&_warn_on_override_continue_label);
        _warn_on_override_cancel_button.set_child(&_warn_on_override_cancel_label);

        auto size = std::max(
            _accept_button_label.get_preferred_size().natural_size.x,
            _cancel_button_label.get_preferred_size().natural_size.x
        );

        _accept_button_label.set_size_request({size, 0});
        _cancel_button_label.set_size_request({size, 0});

        size = std::max(
            _warn_on_override_continue_label.get_preferred_size().natural_size.x,
            _warn_on_override_cancel_label.get_preferred_size().natural_size.x
        );

        _warn_on_override_continue_label.set_size_request({size, 0});
        _warn_on_override_cancel_label.set_size_request({size, 0});

        _warn_on_override_dialog.add_action_widget(&_warn_on_override_cancel_button, [](FileChooserDialog* instance){
            instance->_warn_on_override_dialog.close();
        }, this);

        _warn_on_override_dialog.add_action_widget(&_warn_on_override_continue_button, [](FileChooserDialog* instance){
            if (instance->_on_accept_pressed)
                instance->_on_accept_pressed(instance);

            instance->_warn_on_override_dialog.close();
        }, this);

        _dialog.add_action_widget(&_cancel_button, [](FileChooserDialog* instance){
            if (instance->_on_cancel_pressed)
                instance->_on_cancel_pressed(instance);
        }, this);

        _dialog.add_action_widget(&_accept_button, [](FileChooserDialog* instance){

            if (M == FileChooserDialogMode::SAVE_AS and instance->_file_chooser.get_boolean_choice("WARN_ON_OVERRIDE"))
            {
                instance->_warn_on_override_content.set_text("<b>A file named `" + instance->_name_entry.get_text() + "` already exists. Do you want to replace it?</b>\n\nThis will override the files contents. This operation cannot be undone, continue anyway?");
                instance->_warn_on_override_dialog.present();
            }
            else
            {
                if (instance->_on_accept_pressed)
                    instance->_on_accept_pressed(instance);
            }
        }, this);

        _help_popover.set_child(&_help_popover_content);
        _help_button.set_popover(&_help_popover);

        auto* button_area = gtk_widget_get_parent(_cancel_button.operator GtkWidget*());

        gtk_widget_set_margin_bottom(button_area, state::margin_unit);
        gtk_widget_set_margin_end(button_area, state::margin_unit);
        gtk_widget_set_margin_start(button_area, state::margin_unit);
        gtk_box_set_spacing(GTK_BOX(button_area), state::margin_unit);

        gtk_box_prepend(GTK_BOX(button_area), _button_separator);
        gtk_box_prepend(GTK_BOX(button_area), _help_button);

        _help_button.set_hexpand(false);
        _cancel_button.set_hexpand(false);
        _accept_button.set_hexpand(false);
        _button_separator.set_hexpand(true);

        _help_button.set_halign(GTK_ALIGN_START);
        _cancel_button.set_has_frame(GTK_ALIGN_END);
        _accept_button.set_has_frame(GTK_ALIGN_END);

        button_area = gtk_widget_get_parent(_warn_on_override_cancel_button.operator GtkWidget*());
        gtk_widget_set_margin_bottom(button_area, state::margin_unit);
        gtk_widget_set_margin_end(button_area, state::margin_unit);
        gtk_box_set_spacing(GTK_BOX(button_area), state::margin_unit);

        _file_chooser.add_tick_callback([](FrameClock clock, FileChooserDialog* instance) -> bool {

            auto selected = instance->_file_chooser.get_selected();
            if (not instance->_name_entry.get_has_focus() and
                not selected.empty() and
                instance->_previously_selected_path != selected.at(0).get_name())
            {
                auto file = selected.at(0);
                instance->_file_preview.update_from(&file);
                instance->_name_entry.set_text(file.get_name());
                instance->_previously_selected_path = file.get_path();
            }

            if (M == FileChooserDialogMode::SAVE_AS)
                instance->_accept_button.set_can_respond_to_input(not instance->_name_entry.get_text().empty());
            else if (M == FileChooserDialogMode::CHOOSE_FOLDER)
                instance->_accept_button.set_can_respond_to_input(not selected.empty() and selected.at(0).is_folder());
            else
                instance->_accept_button.set_can_respond_to_input(not selected.empty());

            return true;
        }, this);
    }

    template<FileChooserDialogMode M>
    template<typename Function_t, typename Arg_t>
    void FileChooserDialog<M>::set_on_accept_pressed(Function_t f_in, Arg_t arg_in)
    {
        _on_accept_pressed = [f = f_in, arg = arg_in, this](FileChooserDialog* instance){
            f(instance, arg);
        };
    }

    template<FileChooserDialogMode M>
    template<typename Function_t, typename Arg_t>
    void FileChooserDialog<M>::set_on_cancel_pressed(Function_t f_in, Arg_t arg_in)
    {
        _on_cancel_pressed = [f = f_in, arg = arg_in, this](FileChooserDialog* instance){
            f(instance, arg);
        };
    }

    template<FileChooserDialogMode M>
    FileChooser& FileChooserDialog<M>::get_file_chooser()
    {
        return _file_chooser;
    }

    template<FileChooserDialogMode M>
    std::string FileChooserDialog<M>::get_current_name()
    {
        if (M == FileChooserDialogMode::SAVE_AS)
            return _file_chooser.get_current_folder().get_path() + "/" + _name_entry.get_text();
        else
        {
            auto selected = _file_chooser.get_selected();
            if (not selected.empty())
                return selected.at(0).get_name();
            else
                return "";
        }
    }
    
    template<FileChooserDialogMode M>
    void FileChooserDialog<M>::show()
    {
        _dialog.show();
    }

    template<FileChooserDialogMode M>
    void FileChooserDialog<M>::close()
    {
        _dialog.close();
        
        if (M == FileChooserDialogMode::SAVE_AS)
            _warn_on_override_dialog.close();
    }
}