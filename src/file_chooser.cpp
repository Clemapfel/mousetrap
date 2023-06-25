
//
// Created by clem on 3/31/23.
//

#include <mousetrap/file_chooser.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    FileFilter::FileFilter(const std::string& name)
        : _native(gtk_file_filter_new())
    {
        gtk_file_filter_set_name(_native, name.c_str());
        g_object_ref(_native);
    }

    FileFilter::~FileFilter()
    {
        g_object_unref(_native);
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
        if (suffix[0] == '.')
        {
            std::string no_dot = "";
            for (auto c : suffix)
                no_dot.push_back(c);

            log::warning("In FileFilter::add_allowed_suffix: Suffix `" + suffix + "` starts with a `.` which will not match any file formats; did you mean to specify `" + no_dot + "`?");
        }

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
}

namespace mousetrap::detail 
{
    DECLARE_NEW_TYPE(FileChooserInternal, file_chooser_internal, FILE_CHOOSER_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_INIT(FileChooserInternal, file_chooser_internal, FILE_CHOOSER_INTERNAL)

    static void file_chooser_internal_finalize(GObject* object)
    {
        auto* self = MOUSETRAP_FILE_CHOOSER_INTERNAL(object);

        for (auto* filter : *self->filters)
            g_object_unref(filter);

        #if USE_NATIVE_FILE_CHOOSER
            g_object_unref(self->native);
        #endif

        delete self->on_accept;
        delete self->on_cancel;
        delete self->filters;

        G_OBJECT_CLASS(file_chooser_internal_parent_class)->finalize(object);
    }

    DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(FileChooserInternal, file_chooser_internal, FILE_CHOOSER_INTERNAL)

    static FileChooserInternal* file_chooser_internal_new(GObject* native, FileChooserAction action)
    {
        auto* self = (FileChooserInternal*) g_object_new(file_chooser_internal_get_type(), nullptr);
        file_chooser_internal_init(self);

        #if USE_NATIVE_FILE_CHOOSER
            self->native = (GtkFileChooserNative*) native;
        #else
            self->native = (GtkFileDialog*) native;
        #endif

        self->action = action;
        self->filters = new std::vector<GtkFileFilter*>();

        self->initial_file = nullptr;
        self->initial_folder = nullptr;
        self->initial_name = nullptr;

        self->initial_filter = nullptr;
        self->currently_shown = false;

        self->on_accept = nullptr;
        self->on_cancel = nullptr;

        return self;
    }
}

namespace mousetrap
{
    #if USE_NATIVE_FILE_CHOOSER
        FileChooser::FileChooser(FileChooserAction action, const std::string& title)
            : _internal(
                detail::file_chooser_internal_new(
                    G_OBJECT(gtk_file_chooser_native_new(
                        title.c_str(),
                        GTK_WINDOW(gtk_window_new()),
                        [](FileChooserAction action) -> GtkFileChooserAction
                        {
                            if (action == FileChooserAction::OPEN_MULTIPLE_FILES)
                                return GTK_FILE_CHOOSER_ACTION_OPEN;
                            else if (action == FileChooserAction::SELECT_MULTIPLE_FOLDERS)
                                return GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
                            else
                                return (GtkFileChooserAction) action;
                        }(action),
                        nullptr,
                        nullptr
                    )),
                    action
                )
            )
        {
            g_object_ref(_internal);
            g_signal_connect(_internal->native, "response", G_CALLBACK(on_native_dialog_response), _internal);
        }
    #else
        FileChooser::FileChooser(FileChooserAction action, const std::string& title)
            : _internal(detail::file_chooser_internal_new(G_OBJECT(gtk_file_dialog_new()), action))
        {
            g_object_ref(_internal);
            gtk_file_dialog_set_title(_internal->native, title.c_str());
        }
    #endif

    FileChooser::FileChooser(detail::FileChooserInternal* internal)
    {
        _internal = g_object_ref(internal);
    }

    FileChooser::~FileChooser()
    {
        g_object_unref(_internal);
    }

    NativeObject FileChooser::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    FileChooser::operator NativeObject() const
    {
        return G_OBJECT(_internal->native);
    }

    void FileChooser::present()
    {
        #if USE_NATIVE_FILE_CHOOSER

            auto action = _internal->action;

            if (action == FileChooserAction::OPEN_MULTIPLE_FILES)
                gtk_file_chooser_set_action(GTK_FILE_CHOOSER(_internal->native), GTK_FILE_CHOOSER_ACTION_OPEN);
            else if (action == FileChooserAction::SELECT_MULTIPLE_FOLDERS)
                gtk_file_chooser_set_action(GTK_FILE_CHOOSER(_internal->native), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
            else
                gtk_file_chooser_set_action(GTK_FILE_CHOOSER(_internal->native), (GtkFileChooserAction) action);

            auto select_multiple = action == FileChooserAction::OPEN_MULTIPLE_FILES or action == FileChooserAction::SELECT_MULTIPLE_FOLDERS;
            gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(_internal->native), select_multiple);

            GError* error = nullptr;
            if (action == FileChooserAction::SELECT_FOLDER or action == FileChooserAction::SELECT_MULTIPLE_FOLDERS)
                gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(_internal->native), _internal->initial_folder, &error);
            else if (action == FileChooserAction::OPEN_FILE or action == FileChooserAction::OPEN_MULTIPLE_FILES)
                gtk_file_chooser_set_file(GTK_FILE_CHOOSER(_internal->native), _internal->initial_file, &error);
            else if (action == FileChooserAction::SAVE)
                gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(_internal->native), _internal->initial_name->c_str());

            if (error != nullptr)
            {
                log::critical(std::string("In FileChooser::present: ") + error->message, MOUSETRAP_DOMAIN);
                g_error_free(error);
            }

            for (auto* filter : *_internal->filters)
                gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(_internal->native), filter);

            if (_internal->initial_filter != nullptr)
                gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(_internal->native), _internal->initial_filter);

            gtk_native_dialog_show(GTK_NATIVE_DIALOG(_internal->native));
            _internal->currently_shown = true;
        #else

            auto* list = g_list_store_new(GTK_TYPE_FILE_FILTER);
            for (auto* filter : *_internal->filters)
                g_list_store_append(list, filter);

            gtk_file_dialog_set_filters(_internal->native, G_LIST_MODEL(list));

            auto action = _internal->action;
            if (action == FileChooserAction::OPEN_FILE)
            {
                gtk_file_dialog_open(
                    _internal->native,
                    nullptr,
                    nullptr,
                    (GAsyncReadyCallback) on_file_dialog_ready_callback,
                    _internal
                );
            }
            else if (action == FileChooserAction::OPEN_MULTIPLE_FILES)
            {
                gtk_file_dialog_open_multiple(
                    _internal->native,
                    nullptr,
                    nullptr,
                    (GAsyncReadyCallback) on_file_dialog_ready_callback,
                    _internal
                );
            }
            else if (action == FileChooserAction::SAVE)
            {
                gtk_file_dialog_save(
                _internal->native,
                nullptr,
                nullptr,
                (GAsyncReadyCallback) on_file_dialog_ready_callback,
                _internal
                );
            }
            else if (action == FileChooserAction::SELECT_FOLDER)
            {
                gtk_file_dialog_select_folder(
                    _internal->native,
                    nullptr,
                    nullptr,
                    (GAsyncReadyCallback) on_file_dialog_ready_callback,
                    _internal
                );
            }
            else if (action == FileChooserAction::SELECT_MULTIPLE_FOLDERS)
            {
                gtk_file_dialog_select_multiple_folders(
                    _internal->native,
                    nullptr,
                    nullptr,
                    (GAsyncReadyCallback) on_file_dialog_ready_callback,
                    _internal
                );
            }

        #endif
    }

    void FileChooser::cancel()
    {
        #if USE_NATIVE_FILE_CHOOSER
            gtk_native_dialog_hide(GTK_NATIVE_DIALOG(_internal->native));
        #else

        #endif
    }

    void FileChooser::set_accept_label(const std::string& label)
    {
        #if USE_NATIVE_FILE_CHOOSER
            gtk_file_chooser_native_set_accept_label(_internal->native, label.c_str());
        #else
            gtk_file_dialog_set_accept_label(_internal->native, label.c_str());
        #endif
    }

    std::string FileChooser::get_accept_label() const
    {
        #if USE_NATIVE_FILE_CHOOSER
            auto* ptr = gtk_file_chooser_native_get_accept_label(_internal->native);
            return ptr != nullptr ? std::string(ptr) : "";
        #else
            auto* ptr = gtk_file_dialog_get_accept_label(_internal->native);
            return ptr != nullptr ? std::string(ptr) : "";
        #endif
    }

    void FileChooser::set_is_modal(bool b)
    {
        #if USE_NATIVE_FILE_CHOOSER
            gtk_native_dialog_set_modal(GTK_NATIVE_DIALOG(_internal->native), b);
        #else
            gtk_file_dialog_set_modal(_internal->native, b);
        #endif
    }

    bool FileChooser::get_is_modal() const
    {
        #if USE_NATIVE_FILE_CHOOSER
            return gtk_native_dialog_get_modal(GTK_NATIVE_DIALOG(_internal->native));
        #else
            return gtk_file_dialog_get_modal(_internal->native);
        #endif
    }

    void FileChooser::set_file_chooser_action(FileChooserAction action)
    {
        if (_internal->currently_shown)
            log::warning("In FileChooser::set_file_chooser_action: Dialog is currently open, changes will not be applied until the next time FileChooser::present is called");

        _internal->action = action;
    }

    FileChooserAction FileChooser::get_file_chooser_action() const
    {
            return _internal->action;
    }

    void FileChooser::add_filter(const FileFilter& file_filter)
    {
        auto* gtk_filter = file_filter.operator GtkFileFilter*();
        _internal->filters->push_back(g_object_ref(gtk_filter));

        if (_internal->initial_filter == nullptr)
            _internal->initial_filter = gtk_filter;
    }

    void FileChooser::clear_filters()
    {
        for (auto* filter : *_internal->filters)
            g_object_unref(filter);

        _internal->filters->clear();
    }

    void FileChooser::set_initial_filter(const FileFilter& file_filter)
    {
        auto* as_gtk_filter = file_filter.operator GtkFileFilter*();

        _internal->initial_filter = as_gtk_filter;
        for (auto* filter : *_internal->filters)
            if (filter == as_gtk_filter)
                return;

        add_filter(file_filter);
    }

    void FileChooser::set_initial_file(const FileDescriptor& file)
    {
        if (_internal->initial_file != nullptr)
            g_object_unref(_internal->initial_file);

        _internal->initial_file = g_object_ref(file.operator GFile*());
    }

    void FileChooser::set_initial_folder(const FileDescriptor& file)
    {
        if (_internal->initial_folder != nullptr)
            g_object_unref(_internal->initial_folder);

        _internal->initial_folder = g_object_ref(file.operator GFile*());
    }

    void FileChooser::set_initial_name(const std::string& name)
    {
        if (_internal->initial_name != nullptr)
            delete _internal->initial_name;

        _internal->initial_name = new std::string(name);
    }

    #if USE_NATIVE_FILE_CHOOSER
    void FileChooser::on_native_dialog_response(GtkNativeDialog*, gint id, detail::FileChooserInternal* internal)
    {
        if ((id == GTK_RESPONSE_ACCEPT or id == GTK_RESPONSE_YES or id == GTK_RESPONSE_APPLY) and (internal->on_accept != nullptr))
        {
            std::vector<FileDescriptor> files;
            auto* list = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(internal->native));
            for (size_t i = 0; i < g_list_model_get_n_items(list); ++i)
                files.emplace_back(G_FILE(g_list_model_get_item(list, i)));

            if (*internal->on_accept)
                (*internal->on_accept)(files);
        }
        else if (internal->on_cancel != nullptr)
        {
            if (*internal->on_cancel)
                (*internal->on_cancel)();
        }
    }
    #else
    void FileChooser::on_file_dialog_ready_callback(GtkFileDialog* self, GAsyncResult* result, detail::FileChooserInternal* internal)
    {
        GError* error = nullptr;
        std::vector<FileDescriptor> files;
        if (internal->action == FileChooserAction::OPEN_FILE)
        {
            files.emplace_back(gtk_file_dialog_open_finish(self, result, &error));
        }
        else if (internal->action == FileChooserAction::OPEN_MULTIPLE_FILES)
        {
            auto* list = gtk_file_dialog_open_multiple_finish(self, result, &error);
            for (size_t i = 0; G_IS_LIST_MODEL(list) and i < g_list_model_get_n_items(list); ++i)
                files.emplace_back(G_FILE(g_list_model_get_item(list, i)));
        }
        else if (internal->action == FileChooserAction::SAVE)
        {
            files.emplace_back(gtk_file_dialog_save_finish(self, result, &error));
        }
        else if (internal->action == FileChooserAction::SELECT_FOLDER)
        {
            files.emplace_back(gtk_file_dialog_select_folder_finish(self, result, &error));
        }
        else if (internal->action == FileChooserAction::SELECT_MULTIPLE_FOLDERS)
        {
            auto* list = gtk_file_dialog_select_multiple_folders_finish(self, result, &error);
            for (size_t i = 0; G_IS_LIST_MODEL(list) and i < g_list_model_get_n_items(list); ++i)
                files.emplace_back(G_FILE(g_list_model_get_item(list, i)));
        }

        if (error != nullptr)
        {
            if (error->code == 2) // dismissed by user
            {
                if (internal->on_cancel != nullptr and (*internal->on_cancel))
                {
                    auto self = FileChooser(internal);
                    (*internal->on_cancel)(self);
                }
            }
            else
                log::critical(std::string("In FileChooser::on_file_dialog_ready_callback: ") + error->message, MOUSETRAP_DOMAIN);

            g_error_free(error);
            return;
        }

        if (internal->on_accept != nullptr and (*internal->on_accept))
        {
            auto self = FileChooser(internal);
            (*internal->on_accept)(self, files);
        }
    }
    #endif
}

/*



#if USE_NATIVE_FILE_CHOOSER

namespace mousetrap

    FileChooser::FileChooser(FileChooserAction action, const std::string& title, const std::string& accept_label, const std::string& cancel_label)
        : Widget(GTK_WIDGET(gtk_file_chooser_native_new(title.c_str(), nullptr, (GtkFileChooserAction) action, accept_label.c_str(), cancel_label.c_str()))),
          CTOR_SIGNAL(FileChooser, response)
    {
        _internal = g_object_ref_sink(GTK_FILE_CHOOSER_NATIVE(Widget::operator NativeWidget()));
    }
    
    FileChooser::FileChooser(detail::FileChooserInternal* internal) 
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(FileChooser, response)
    {
        _internal = g_object_ref(internal);
    }
    
    FileChooser::~FileChooser()
    {
        g_object_unref(_internal);
    }

    NativeObject FileChooser::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void FileChooser::set_accept_label(const std::string& label)
    {
        gtk_file_chooser_native_set_accept_label(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget()), label.c_str());
    }

    std::string FileChooser::get_accept_label() const
    {
        auto* label = gtk_file_chooser_native_get_accept_label(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget()));
        return label == nullptr ? "" : label;
    }

    void FileChooser::show()
    {
        gtk_native_dialog_show(GTK_NATIVE_DIALOG(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())));
    }

    void FileChooser::hide()
    {
        gtk_native_dialog_hide(GTK_NATIVE_DIALOG(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())));
    }

    bool FileChooser::get_is_shown() const
    {
        return gtk_native_dialog_get_visible(GTK_NATIVE_DIALOG(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())));
    }

    void FileChooser::set_is_modal(bool b)
    {
        gtk_native_dialog_set_modal(GTK_NATIVE_DIALOG(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())), b);
    }

    bool FileChooser::get_is_modal() const
    {
        return gtk_native_dialog_get_modal(GTK_NATIVE_DIALOG(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())));
    }

    void FileChooser::set_transient_for(Window* window)
    {
        gtk_native_dialog_set_transient_for(GTK_NATIVE_DIALOG(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())), window != nullptr ? GTK_WINDOW(window->get_internal()) : nullptr);
    }

    void FileChooser::set_can_select_multiple(bool b)
    {
        gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())), b);
    }

    bool FileChooser::get_can_select_multiple() const
    {
        return gtk_file_chooser_get_select_multiple(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())));
    }

    void FileChooser::set_file_chooser_action(FileChooserAction action)
    {
        gtk_file_chooser_set_action(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())), (GtkFileChooserAction) action);
    }

    FileChooserAction FileChooser::get_file_chooser_action() const
    {
        return (FileChooserAction) gtk_file_chooser_get_action(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())));
    }

    std::vector<FileDescriptor> FileChooser::get_selected_files() const
    {
        std::vector<FileDescriptor> out;
        auto* list = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())));
        for (size_t i = 0; i < g_list_model_get_n_items(list); ++i)
            out.emplace_back(G_FILE(g_list_model_get_item(list, i)));

        return out;
    }

    void FileChooser::set_selected_file(const FileDescriptor& file) const
    {
        GError* error = nullptr;
        gtk_file_chooser_set_file(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())), file.operator GFile*(), &error);

        if (error != nullptr)
        {
            log::critical("In FileChooser::set_file: Unable to set current file to " + file.get_path() + std::string(": ") + error->message);
            g_error_free(error);
        }
    }

    FileDescriptor FileChooser::get_current_folder() const
    {
        return FileDescriptor(gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget()))));
    }

    void FileChooser::set_current_folder(const FileDescriptor& folder)
    {
        GError* error = nullptr;
        gtk_file_chooser_set_file(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())), folder.operator GFile*(), &error);

        if (error != nullptr)
        {
            log::critical("In FileChooser::set_current_folder: Unable to set current folder to " + folder.get_path() + std::string(": ") + error->message);
            g_error_free(error);
        }
    }

    void FileChooser::add_filter_choice(const FileFilter& filter)
    {
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())), filter.operator GtkFileFilter *());
    }

    void FileChooser::set_filter(const FileFilter& filter)
    {
        gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())), filter.operator GtkFileFilter*());
    }

    void FileChooser::set_current_name(const std::string& name)
    {
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())), name.c_str());
    }

    std::string FileChooser::get_current_name() const
    {
        auto* out = gtk_file_chooser_get_current_name(GTK_FILE_CHOOSER(GTK_FILE_CHOOSER_NATIVE(operator NativeWidget())));
        return out != nullptr ? out : "";
    }
}

#else

namespace mousetrap
{

    FileChooser::FileChooser(FileChooserAction action, const std::string& title, const std::string& accept_label, const std::string& cancel_label)
    : Widget(GTK_WIDGET(gtk_file_dialog_new())),
      CTOR_SIGNAL(FileChooser, response)
    {
        _internal = g_object_ref_sink(GTK_FILE_CHOOSER_NATIVE(Widget::operator NativeWidget()));
    }

    FileChooser::FileChooser(detail::FileChooserInternal* internal)
    : Widget(GTK_WIDGET(internal)),
      CTOR_SIGNAL(FileChooser, response)
    {
        _internal = g_object_ref(internal);
    }

    FileChooser::~FileChooser()
    {
        g_object_unref(_internal);
    }

    NativeObject FileChooser::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void FileChooser::set_accept_label(const std::string& label)
    {
        gtk_file_dialog_set_accept_label(GTK_FILE_DIALOG(operator NativeWidget()), labe.c_str());
    }

    std::string FileChooser::get_accept_label() const
    {
        auto* label = gtk_file_dialog_get_accept_label(GTK_FILE_DIALOG(operator NativeWidget()));
        return label == nullptr ? "" : label;
    }

    void FileChooser::set_cancel_label(const std::string& label)
    {
        gtk_file_dialog_set_cancel_label(GTK_FILE_DIALOG(operator NativeWidget()), label.c_str());
    }

    std::string FileChooser::get_cancel_label() const
    {
        auto* label = gtk_file_chooser_native_get_cancel_label(GTK_FILE_DIALOG(operator NativeWidget()));
        return label == nullptr ? "" : label;
    }

    void FileChooser::show()
    {
        gtk_native_dialog_show(GTK_NATIVE_DIALOG(GTK_FILE_DIALOG(operator NativeWidget())));
    }

    void FileChooser::hide()
    {
        gtk_native_dialog_hide(GTK_NATIVE_DIALOG(GTK_FILE_DIALOG(operator NativeWidget())));
    }

    bool FileChooser::get_is_shown() const
    {
        return gtk_native_dialog_get_visible(GTK_NATIVE_DIALOG(GTK_FILE_DIALOG(operator NativeWidget())));
    }

    void FileChooser::set_is_modal(bool b)
    {
        gtk_native_dialog_set_modal(GTK_NATIVE_DIALOG(GTK_FILE_DIALOG(operator NativeWidget())), b);
    }

    bool FileChooser::get_is_modal() const
    {
        return gtk_native_dialog_get_modal(GTK_NATIVE_DIALOG(GTK_FILE_DIALOG(operator NativeWidget())));
    }

    void FileChooser::set_transient_for(Window* window)
    {
        gtk_native_dialog_set_transient_for(GTK_NATIVE_DIALOG(GTK_FILE_DIALOG(operator NativeWidget())), window != nullptr ? GTK_WINDOW(window->get_internal()) : nullptr);
    }

    void FileChooser::set_can_select_multiple(bool b)
    {
        gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())), b);
    }

    bool FileChooser::get_can_select_multiple() const
    {
        return gtk_file_chooser_get_select_multiple(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())));
    }

    void FileChooser::set_file_chooser_action(FileChooserAction action)
    {
        gtk_file_chooser_set_action(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())), (GtkFileChooserAction) action);
    }

    FileChooserAction FileChooser::get_file_chooser_action() const
    {
        return (FileChooserAction) gtk_file_chooser_get_action(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())));
    }

    std::vector<FileDescriptor> FileChooser::get_selected_files() const
    {
        std::vector<FileDescriptor> out;
        auto* list = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())));
        for (size_t i = 0; i < g_list_model_get_n_items(list); ++i)
            out.emplace_back(G_FILE(g_list_model_get_item(list, i)));

        return out;
    }

    void FileChooser::set_selected_file(const FileDescriptor& file) const
    {
        GError* error = nullptr;
        gtk_file_chooser_set_file(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())), file.operator GFile*(), &error);

        if (error != nullptr)
        {
            log::critical("In FileChooser::set_file: Unable to set current file to " + file.get_path() + std::string(": ") + error->message);
            g_error_free(error);
        }
    }

    FileDescriptor FileChooser::get_current_folder() const
    {
        return FileDescriptor(gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget()))));
    }

    void FileChooser::set_current_folder(const FileDescriptor& folder)
    {
        GError* error = nullptr;
        gtk_file_chooser_set_file(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())), folder.operator GFile*(), &error);

        if (error != nullptr)
        {
            log::critical("In FileChooser::set_current_folder: Unable to set current folder to " + folder.get_path() + std::string(": ") + error->message);
            g_error_free(error);
        }
    }

    void FileChooser::add_filter_choice(const FileFilter& filter)
    {
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())), filter.operator GtkFileFilter *());
    }

    void FileChooser::set_filter(const FileFilter& filter)
    {
        gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())), filter.operator GtkFileFilter*());
    }

    void FileChooser::set_current_name(const std::string& name)
    {
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())), name.c_str());
    }

    std::string FileChooser::get_current_name() const
    {
        auto* out = gtk_file_chooser_get_current_name(GTK_FILE_CHOOSER(GTK_FILE_DIALOG(operator NativeWidget())));
        return out != nullptr ? out : "";
    }
}

#endif

 */