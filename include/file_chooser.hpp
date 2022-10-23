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
        OPEN = GTK_FILE_CHOOSER_ACTION_OPEN,
        SAVE = GTK_FILE_CHOOSER_ACTION_SAVE,
        SELECT_FOLDER = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER
    };

    class FileChooser : public WidgetImplementation<GtkFileChooserWidget>
    {
        public:
            FileChooser(FileChooserAction);

            void set_can_select_multiple(bool);
            bool get_can_select_multiple();

            void set_can_create_folders(bool);
            bool get_can_create_fodlers();

            std::vector<File> get_selected();

            File get_current_folder();
            void set_current_folder(File*);

            void set_current_name(const std::string&);
            std::string get_current_name();
    };
}

//

namespace mousetrap
{
    FileChooser::FileChooser(FileChooserAction action)
        : WidgetImplementation<GtkFileChooserWidget>(GTK_FILE_CHOOSER_WIDGET(gtk_file_chooser_widget_new((GtkFileChooserAction) action)))
    {}

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

    File FileChooser::get_current_folder()
    {
        return File(gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(get_native())));
    }

    void FileChooser::set_current_folder(File* folder)
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

    std::vector<File> FileChooser::get_selected()
    {
        auto* list = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(get_native()));
        std::vector<File> out;
        out.reserve(g_list_model_get_n_items(list));

        for (size_t i = 0; i < g_list_model_get_n_items(list); ++i)
            out.emplace_back((GFile*) g_list_model_get_item(list, i));

        return out;
    }
}