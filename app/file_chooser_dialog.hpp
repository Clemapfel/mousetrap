// 
// Copyright 2022 Clemens Cords
// Created on 11/12/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/project_state.hpp>
#include <app/app_component.hpp>
#include <app/shortcut_information.hpp>

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

            template<typename Function_t>
            void set_on_accept_pressed(Function_t);

            template<typename Function_t, typename Arg_t>
            void set_on_cancel_pressed(Function_t, Arg_t);

            template<typename Function_t>
            void set_on_cancel_pressed(Function_t);

            FileChooser& get_file_chooser();

            std::string get_current_name();
            Entry& get_name_entry();

            void show();
            void close();
            
        private:
            Dialog _dialog;

            Button _accept_button;
            Label _accept_button_label = Label(Mode == FileChooserDialogMode::SAVE_AS ? "OK" : "Open");

            Button _cancel_button;
            Label _cancel_button_label = Label("Cancel");

            SeparatorLine _action_button_area_spacer;

            std::string _previously_selected_path = "";

            Label _show_keybindings_button_label = Label(state::settings_file->get_value("global", "show_keybinding_shortcut_label"));
            MenuButton _show_keybindings_button;
            Popover _show_keybindings_popover;
            ShortcutInformation _show_keybindings_content;

            FilePreview _file_preview;
            Frame _preview_frame;
            Label _preview_label = Label("<span weight=\"bold\" color=\"#AAAAAA\">Preview</span>");
            
            Frame _file_chooser_frame;
            FileChooser _file_chooser = FileChooser([]() -> FileChooserAction {
                if (Mode == FileChooserDialogMode::OPEN)
                    return FileChooserAction::SELECT_FILE;
                else if (Mode == FileChooserDialogMode::SAVE_AS)
                    return FileChooserAction::SELECT_FILE;
                else if (Mode == FileChooserDialogMode::CHOOSE_FOLDER)
                    return FileChooserAction::SELECT_FOLDER;
            }());

            Box _name_entry_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Entry _name_entry;
            Label _name_entry_label = Label("Name: ");
            
            Box _content_area = Box(GTK_ORIENTATION_VERTICAL);
            Box _file_chooser_preview_area = Box(GTK_ORIENTATION_HORIZONTAL);

            std::function<void(FileChooserDialog*)> _on_accept_pressed;
            std::function<void(FileChooserDialog*)> _on_cancel_pressed;
            
            // warn if overriding file dialog:

            Dialog _warn_on_override_dialog = Dialog(state::main_window, "Warning");

            Viewport  _warn_on_override_content_box;
            Label _warn_on_override_content;

            Button _warn_on_override_continue_button;
            Label  _warn_on_override_continue_label = Label("Continue");
            Button _warn_on_override_cancel_button;
            Label  _warn_on_override_cancel_label = Label("Cancel");

            ShortcutController _shortcut_controller = ShortcutController(state::app);

            Overlay _show_keybinds_overlay;

            KeyEventController _key_event_controller;
            static bool on_key_pressed(KeyEventController*, guint keyval, guint keycode, GdkModifierType state, FileChooserDialog<Mode>* instance);

            bool _name_entry_focused = false;
            FocusEventController _focus_event_controller;
            static void on_focus_gained(FocusEventController*, double x, double y, FileChooserDialog<Mode>* instance);
            static void on_focus_lost(FocusEventController*, double x, double y, FileChooserDialog<Mode>* instance);
    };
    
    using SaveAsFileDialog = FileChooserDialog<FileChooserDialogMode::SAVE_AS>;
    using OpenFileDialog = FileChooserDialog<FileChooserDialogMode::OPEN>;
    using ChooseFolderDialog = FileChooserDialog<FileChooserDialogMode::CHOOSE_FOLDER>;
}

#include <app/src/file_chooser_dialog.inl>
