// 
// Copyright 2022 Clemens Cords
// Created on 10/23/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/file.hpp>

namespace mousetrap
{
    enum class FileChooserAction
    {
        SELECT_FILE = GTK_FILE_CHOOSER_ACTION_OPEN,
        SELECT_FOLDER = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER
    };

    class FileFilter
    {
        public:
            FileFilter(const std::string& name);
            operator GtkFileFilter*();

            void add_allowed_pattern(const std::string& glob);
            void add_allow_all_supported_image_formats();

            /// \brief no "." in suffix
            void add_allowed_suffix(const std::string& suffix);
            void add_allowed_mime_type(const std::string& mime_type_id);

        private:
            GtkFileFilter* _native;
    };

    class FileChooser : public WidgetImplementation<GtkFileChooserWidget>
    {
        public:
            FileChooser(FileChooserAction);

            void set_can_select_multiple(bool);
            bool get_can_select_multiple();

            void set_can_create_folders(bool);
            bool get_can_create_fodlers();

            std::vector<FileDescriptor> get_selected();

            FileDescriptor get_current_folder();
            void set_current_folder(FileDescriptor*);

            void set_current_name(const std::string&);
            std::string get_current_name();

            void add_filter(FileFilter);

            using ChoiceID = std::string;
            using OptionID = std::string;

            /// \param default_choice: "" to select first choice in options
            void add_choice(
                ChoiceID id,
                const std::string& label,
                std::vector<OptionID> options,
                std::vector<std::string> option_labels,
                OptionID default_choice = ""
            );

            void add_boolean_choice(ChoiceID id, const std::string& label, bool default_value = false);

            // returns "false", "true" for boolean choice
            OptionID get_choice(ChoiceID);
            bool get_boolean_choice(ChoiceID);

        private:
            using choice_option = struct {std::vector<const char*> option_ids; std::vector<const char*> labels;};
           std::unordered_map<ChoiceID, choice_option> _choice_options;
    };
}

#include <src/file_chooser.inl>
