
//
// Created by clem on 3/31/23.
//

#include <include/file_chooser.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    FileFilter::FileFilter(const std::string& name)
        : _native(gtk_file_filter_new())
    {
        gtk_file_filter_set_name(_native, name.c_str());
    }

    FileFilter::operator GtkFileFilter*() const
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

    std::string FileFilter::get_name() const
    {
        auto* name = gtk_file_filter_get_name(_native);
        return name != nullptr ? name : "";
    }

    void FileFilter::set_name(const std::string& name)
    {
        gtk_file_filter_set_name(_native, name.c_str());
    }

    FileChooser::FileChooser(FileChooserAction action, const std::string& title, const std::string& accept_label, const std::string& cancel_label)
        : WidgetImplementation<GtkFileChooserNative>(gtk_file_chooser_native_new(title.c_str(), nullptr, (GtkFileChooserAction) action, accept_label.c_str(), cancel_label.c_str())),
          CTOR_SIGNAL(FileChooser, response)
    {}

    void FileChooser::set_accept_label(const std::string& label)
    {
        gtk_file_chooser_native_set_accept_label(get_native(), label.c_str());
    }

    std::string FileChooser::get_accept_label() const
    {
        auto* label = gtk_file_chooser_native_get_accept_label(get_native());
        return label == nullptr ? "" : label;
    }

    void FileChooser::set_cancel_label(const std::string& label)
    {
        gtk_file_chooser_native_set_cancel_label(get_native(), label.c_str());
    }

    std::string FileChooser::get_cancel_label() const
    {
        auto* label = gtk_file_chooser_native_get_cancel_label(get_native());
        return label == nullptr ? "" : label;
    }

    void FileChooser::show()
    {
        gtk_native_dialog_show(GTK_NATIVE_DIALOG(get_native()));
    }

    void FileChooser::hide()
    {
        gtk_native_dialog_hide(GTK_NATIVE_DIALOG(get_native()));
    }

    bool FileChooser::get_is_shown() const
    {
        return gtk_native_dialog_get_visible(GTK_NATIVE_DIALOG(get_native()));
    }

    void FileChooser::set_is_modal(bool b)
    {
        gtk_native_dialog_set_modal(GTK_NATIVE_DIALOG(get_native()), b);
    }

    bool FileChooser::get_is_modal() const
    {
        return gtk_native_dialog_get_modal(GTK_NATIVE_DIALOG(get_native()));
    }

    void FileChooser::set_transient_for(Window* window)
    {
        gtk_native_dialog_set_transient_for(GTK_NATIVE_DIALOG(get_native()), window != nullptr ? window->operator GtkWindow*() : nullptr);
    }

    void FileChooser::set_can_select_multiple(bool b)
    {
        gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(get_native()), b);
    }

    bool FileChooser::get_can_select_multiple() const
    {
        return gtk_file_chooser_get_select_multiple(GTK_FILE_CHOOSER(get_native()));
    }

    void FileChooser::set_file_chooser_action(FileChooserAction action)
    {
        gtk_file_chooser_set_action(GTK_FILE_CHOOSER(get_native()), (GtkFileChooserAction) action);
    }

    FileChooserAction FileChooser::get_file_chooser_action() const
    {
        return (FileChooserAction) gtk_file_chooser_get_action(GTK_FILE_CHOOSER(get_native()));
    }

    std::vector<FileDescriptor> FileChooser::get_selected_files() const
    {
        std::vector<FileDescriptor> out;
        auto* list = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(get_native()));
        for (size_t i = 0; i < g_list_model_get_n_items(list); ++i)
            out.emplace_back(G_FILE(g_list_model_get_item(list, i)));

        return out;
    }

    void FileChooser::set_selected_file(const FileDescriptor& file) const
    {
        GError* error = nullptr;
        gtk_file_chooser_set_file(GTK_FILE_CHOOSER(get_native()), file.operator GFile*(), &error);

        if (error != nullptr)
        {
            log::critical("In FileChooser::set_file: Unable to set current file to " + file.get_path() + std::string(": ") + error->message);
            g_error_free(error);
        }
    }

    FileDescriptor FileChooser::get_current_folder() const
    {
        return FileDescriptor(gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(get_native())));
    }

    void FileChooser::set_current_folder(const FileDescriptor& folder)
    {
        GError* error = nullptr;
        gtk_file_chooser_set_file(GTK_FILE_CHOOSER(get_native()), folder.operator GFile*(), &error);

        if (error != nullptr)
        {
            log::critical("In FileChooser::set_current_folder: Unable to set current folder to " + folder.get_path() + std::string(": ") + error->message);
            g_error_free(error);
        }
    }

    void FileChooser::add_filter_choice(const FileFilter& filter)
    {
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(get_native()), filter.operator GtkFileFilter *());
    }

    void FileChooser::set_filter(const FileFilter& filter)
    {
        gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(get_native()), filter.operator GtkFileFilter*());
    }

    void FileChooser::set_current_name(const std::string& name)
    {
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(get_native()), name.c_str());
    }

    std::string FileChooser::get_current_name() const
    {
        auto* out = gtk_file_chooser_get_current_name(GTK_FILE_CHOOSER(get_native()));
        return out != nullptr ? out : "";
    }
}