// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    FileFilter::FileFilter(const std::string& name)
            : _native(gtk_file_filter_new())
    {
        gtk_file_filter_set_name(_native, name.c_str());
    }

    FileFilter::operator GtkFileFilter*()
    {
        return _native;
    }

    void FileFilter::add_allow_all_supported_image_formats()
    {
        gtk_file_filter_add_pixbuf_formats(_native);
    }

    void FileFilter::add_allowed_suffix(const std::string& suffix)
    {
        gtk_file_filter_add_suffix(_native, suffix.c_str());
    }

    void FileFilter::add_allowed_pattern(const std::string& glob)
    {
        gtk_file_filter_add_pattern(_native, glob.c_str());
    }

    void FileFilter::add_allowed_mime_type(const std::string& mime_type_id)
    {
        gtk_file_filter_add_mime_type(_native, mime_type_id.c_str());
    }

    FileChooser::FileChooser(FileChooserAction action)
            : WidgetImplementation<GtkFileChooserWidget>(GTK_FILE_CHOOSER_WIDGET(gtk_file_chooser_widget_new((GtkFileChooserAction) action)))//,
              //HasActivateSignal<FileChooser>(this)
              //HasFileActivatedSignal<FileChooser>(this),
              //HasFileSelectionChangedSignal<FileChooser>(this)
    {
        auto* all_filter = gtk_file_filter_new();
        gtk_file_filter_set_name(all_filter, "(No Filter)");
        gtk_file_filter_add_pattern(all_filter, "*");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(get_native()), all_filter);
    }

    void FileChooser::add_filter(FileFilter filter)
    {
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(get_native()), filter.operator GtkFileFilter*());
    }

    void FileChooser::set_can_select_multiple(bool b)
    {
        gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(get_native()), b);
    }

    bool FileChooser::get_can_select_multiple()
    {
        return gtk_file_chooser_get_select_multiple(GTK_FILE_CHOOSER(get_native()));
    }

    void FileChooser::set_can_create_folders(bool b)
    {
        gtk_file_chooser_set_create_folders(GTK_FILE_CHOOSER(get_native()), b);
    }

    bool FileChooser::get_can_create_fodlers()
    {
        return gtk_file_chooser_get_create_folders(GTK_FILE_CHOOSER(get_native()));
    }

    FileDescriptor FileChooser::get_current_folder()
    {
        return FileDescriptor(gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(get_native())));
    }

    void FileChooser::set_current_folder(FileDescriptor* folder)
    {
        GError* error_maybe = nullptr;
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(get_native()), folder->operator GFile*(), &error_maybe);

        if (error_maybe != nullptr)
            std::cerr << "[WARNING] In FileChooser::set_current_folder: Unable to to set location to " << folder->get_path() << ": " << error_maybe->message << std::endl;
    }

    std::string FileChooser::get_current_name()
    {
        return gtk_file_chooser_get_current_name(GTK_FILE_CHOOSER(get_native()));
    }

    void FileChooser::set_current_name(const std::string& name)
    {
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(get_native()), name.c_str());
    }

    std::vector<FileDescriptor> FileChooser::get_selected()
    {
        auto* list = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(get_native()));
        std::vector<FileDescriptor> out;
        out.reserve(g_list_model_get_n_items(list));

        for (size_t i = 0; i < g_list_model_get_n_items(list); ++i)
            out.emplace_back((GFile*) g_list_model_get_item(list, i));

        return out;
    }

    FileChooser::OptionID FileChooser::get_choice(ChoiceID id)
    {
        auto* res = gtk_file_chooser_get_choice(GTK_FILE_CHOOSER(get_native()), id.c_str());
        if (res == nullptr)
        {
            std::cerr << "[ERROR] In FileChooser::get_choice: No choice with id `" << id
                      << "` registered." << std::endl;
        }
        return res;
    }

    bool FileChooser::get_boolean_choice(ChoiceID id)
    {
        auto* res_ptr = gtk_file_chooser_get_choice(GTK_FILE_CHOOSER(get_native()), id.c_str());
        if (res_ptr == nullptr)
        {
            std::cerr << "[ERROR] In FileChooser::get_boolean_choice: No boolean choice with id `" << id
                      << "` registered." << std::endl;
            return false;
        }

        std::string res = res_ptr;

        if (res == "true")
            return true;
        else if (res == "false")
            return false;

        return false;
    }

    void FileChooser::add_choice(ChoiceID id, const std::string& label, std::vector<OptionID> options,
                                 std::vector<std::string> option_labels, OptionID default_choice)
    {
        auto inserted = _choice_options.insert({id, choice_option{}}).first->second;

        for (auto& option_id : options)
            inserted.option_ids.push_back(option_id.c_str());
        inserted.option_ids.push_back(nullptr);

        for (auto& label : option_labels)
            inserted.labels.push_back(label.c_str());
        inserted.labels.push_back(nullptr);

        gtk_file_chooser_add_choice(GTK_FILE_CHOOSER(get_native()), id.c_str(), label.c_str(), inserted.option_ids.data(), inserted.labels.data());

        if (default_choice != "")
            gtk_file_chooser_set_choice(GTK_FILE_CHOOSER(get_native()), id.c_str(), default_choice.c_str());
    }

    void FileChooser::add_boolean_choice(ChoiceID id, const std::string& label, bool default_choice)
    {
        gtk_file_chooser_add_choice(GTK_FILE_CHOOSER(get_native()), id.c_str(), label.c_str(), nullptr, nullptr);

        if (default_choice)
            gtk_file_chooser_set_choice(GTK_FILE_CHOOSER(get_native()), id.c_str(), "true");
        else
            gtk_file_chooser_set_choice(GTK_FILE_CHOOSER(get_native()), id.c_str(), "false");
    }
}