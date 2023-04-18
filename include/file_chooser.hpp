//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/24/23
//

#pragma once

#include <include/widget.hpp>
#include <include/window.hpp>
#include <include/signal_component.hpp>
#include <include/file_descriptor.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief type of action for file chooser dialog, determines layout and choice type
    enum class FileChooserAction
    {
        /// @brief open file, can accept with one or more files selected
        OPEN = GTK_FILE_CHOOSER_ACTION_OPEN,

        /// @brief save to a path, adds path entry
        SAVE = GTK_FILE_CHOOSER_ACTION_SAVE,

        /// @brief select folder, similar to file but only folders are shown
        SELECT_FOLDER = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER
    };

    /// @brief filter, can be selected by the user, only files that pass the filter will be displayed in file chooser
    class FileFilter
    {
        public:
            /// @brief construct
            /// @param name name of the filter
            FileFilter(const std::string& name);

            /// @brief get filter name
            /// @return name
            std::string get_name() const;

            /// @brief set filter name
            /// @param name
            void set_name(const std::string&);

            /// @brief expose as GtkFileFilter \internal
            operator GtkFileFilter*() const;

            /// @brief add allowed file pattern, as a shell-style glob
            /// @param glob string, for example `*.txt`
            void add_allowed_pattern(const std::string& glob);

            /// @brief add all file formats that can be loaded to a mousetrap::Image
            void add_allow_all_supported_image_formats();

            /// @brief add allowed file extensions
            /// @param suffix extension, does not contain `.`, for example `txt`
            void add_allowed_suffix(const std::string& suffix);

            /// @brief add a mime type to the filter
            /// @param mime_type_id
            void add_allowed_mime_type(const std::string& mime_type_id);

        private:
            GtkFileFilter* _native;
    };

    /// @brief native file chooser dialog
    /// @todo replace with GtkFileDialog once GTK4.10 stable releases
    class FileChooser : public WidgetImplementation<GtkFileChooserNative>,
        HAS_SIGNAL(FileChooser, response)
    {
        public:
            /// @brief construct
            /// @param action type of action, determines formatting of the file chooser
            /// @param title title of the window, may be empty
            /// @param accept_label label for the button that confirms the users choice
            /// @param cancel_label label for the button that aborts the users choice
            FileChooser(FileChooserAction, const std::string& title, const std::string& accept_label = "Accept", const std::string& cancel_label = "Cancel");

            /// @brief set label for button that confirms users choice
            /// @param label
            void set_accept_label(const std::string&);

            /// @brief get label for button that confirms users choice
            /// @return label
            std::string get_accept_label() const;

            /// @brief set label for button that aborts dialog
            /// @param label
            void set_cancel_label(const std::string&);

            /// @brief get label for button that aborts dialog
            /// @return label
            std::string get_cancel_label() const;

            /// @brief present the dialog to the user
            void show();

            /// @brief hide the dialog, this does not cause the "response" signal to be emitted
            void hide();

            /// @brief get whether dialog is currently presented to the user
            bool get_is_shown() const;

            /// @brief set whether the dialogs window is modal
            /// @param b true if it should be modal, false otherwise
            void set_is_modal(bool);

            /// @brief get whether the dialogs window is modal
            /// @return true if modal, false otherwise
            bool get_is_modal() const;

            /// @brief set whether dialog should be transient for another window. If transient, dialog will always be shown on top
            void set_transient_for(Window*);

            /// @brief set whether the user can select more than one file or directory at the same time
            /// @param b true if multiple selection should be allowed, false otherwise
            void set_can_select_multiple(bool);

            /// @brief get whether the user can select more than one file or directory at the same time
            /// @return true if multiple selection is allowed, false otherwise
            bool get_can_select_multiple() const;

            /// @brief set the type of action, determines formatting of the file chooser
            /// @param action
            void set_file_chooser_action(FileChooserAction);

            /// @brief get the type of action
            /// @return action
            FileChooserAction get_file_chooser_action() const;

            /// @brief get currently selected file(s)
            /// @return vector of files, may be empty
            [[nodiscard]] std::vector<FileDescriptor> get_selected_files() const;

            /// @brief change the current directory such that given file is now selected
            /// @param file
            void set_selected_file(const FileDescriptor&) const;

            /// @brief get current directory
            /// @return directory
            [[nodiscard]] FileDescriptor get_current_folder() const;

            /// @brief set current directory
            /// @param directory
            void set_current_folder(const FileDescriptor&);

            /// @brief add a filter to the choice of filters
            /// @param file_filter
            void add_filter_choice(const FileFilter&);

            /// @brief set the currently selected filter, does not have to be added via mousetrap::FileChooser::add_filter_choice first
            /// @param file_filter
            void set_filter(const FileFilter&);

            /// @brief set name in the file selector, as if entered by the user
            /// @param name
            void set_current_name(const std::string&);

            /// @brief get currently entered name in the file selector
            /// @return name, may be empty
            std::string get_current_name() const;
    };
}