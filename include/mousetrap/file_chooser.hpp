//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/24/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/window.hpp>
#include <mousetrap/signal_component.hpp>
#include <mousetrap/file_descriptor.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief type of action for file chooser dialog, determines layout and choice type
    enum class FileChooserAction
    {
        /// @brief open single file
        OPEN_FILE = GTK_FILE_CHOOSER_ACTION_OPEN,

        /// @brief open multiple fies
        OPEN_MULTIPLE_FILES = GTK_FILE_CHOOSER_ACTION_OPEN + 128,

        /// @brief save to a path, adds path entry
        SAVE = GTK_FILE_CHOOSER_ACTION_SAVE,

        /// @brief select single folder
        SELECT_FOLDER = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,

        /// @brief select multiple folders
        SELECT_MULTIPLE_FOLDERS = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER + 128
    };

    /// @brief filter, can be selected by the user, only files that pass the filter will be displayed in file chooser
    class FileFilter
    {
        public:
            /// @brief construct
            /// @param name name of the filter
            FileFilter(const std::string& name);

            /// @brief destructor
            ~FileFilter();

            /// @brief get filter name
            /// @return name
            std::string get_name() const;

            /// @brief set filter name
            /// @param name
            void set_name(const std::string&);

            /// @brief expose as GtkFileFilter \for_internal_use_only
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

    #ifndef DOXYGEN
    class FileChooser;
    namespace detail
    {
        #define USE_NATIVE_FILE_CHOOSER false //GTK_MINOR_VERSION < 10

        struct _FileChooserInternal
        {
            GObject parent;

            #if USE_NATIVE_FILE_CHOOSER
                GtkFileChooserNative* native;
            #else
                GtkFileDialog* native;
            #endif

            FileChooserAction action;

            GFile* initial_file;
            GFile* initial_folder;

            std::vector<GtkFileFilter*>* filters;
            GtkFileFilter* initial_filter;

            std::string* initial_name;
            bool currently_shown;

            std::function<void(FileChooser&, const std::vector<FileDescriptor>&)>* on_accept;
            std::function<void(FileChooser&)>* on_cancel;
        };
        using FileChooserInternal = _FileChooserInternal;
        DEFINE_INTERNAL_MAPPING(FileChooser);
    }
    #endif

    /// @brief native file chooser dialog
    /// \note The graphical element of this widget changes depending on the installed GTK4 version. For GTK4.10+ GtkFileDialog is used, for older version, the now deprecated GtkFileChooserNative is used
    class FileChooser : public SignalEmitter
    {
        public:
            /// @brief construct
            /// @param action type of action, determines formatting of the file chooser
            /// @param title title of the window, may be empty
            FileChooser(FileChooserAction action, const std::string& title = "");

            /// @brief construct from internal
            /// @param internal
            FileChooser(detail::FileChooserInternal* internal);

            /// @brief destructor
            ~FileChooser();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief expose native
            operator NativeObject() const override;

            /// @brief register callback to be called when user has made a selection
            /// @param function function with signature `(FileChooser&, const std::vector<FileDescriptor>&, Data_t) -> void`
            /// @param data arbitrary data
            template<typename Function_t, typename Data_t>
            void on_accept(Function_t function, Data_t data);

            /// @brief register callback to be called when user has made a selection
            /// @param function function with signature `(FileChooser&, const std::vector<FileDescriptor>&) -> void`
            template<typename Function_t>
            void on_accept(Function_t function);

            /// @brief register callback to be called when user has made a selection
            /// @param function function with signature `(FileChooser&; Data_t) -> void`
            /// @param data arbitrary data
            template<typename Function_t, typename Data_t>
            void on_cancel(Function_t function, Data_t data);

            /// @brief register callback to be called when user has made a selection
            /// @param function function with signature `(FileChooser&) -> void`
            template<typename Function_t>
            void on_cancel(Function_t function);

            /// @brief set label for button that confirms users choice
            /// @param label
            void set_accept_label(const std::string& label);

            /// @brief get label for button that confirms users choice
            /// @return label
            std::string get_accept_label() const;

            /// @brief present the dialog to the user
            void present();

            /// @brief abort the current process, the cancel callback will be invoked
            void cancel();

            /// @brief set whether the dialogs window is modal
            /// @param b true if it should be modal, false otherwise
            void set_is_modal(bool b);

            /// @brief get whether the dialogs window is modal
            /// @return true if modal, false otherwise
            bool get_is_modal() const;

            /// @brief set the type of action, determines formatting of the file chooser
            /// @param action
            void set_file_chooser_action(FileChooserAction action);

            /// @brief get the type of action
            /// @return action
            FileChooserAction get_file_chooser_action() const;

            /// @brief add a filter to the choice of filters
            /// @param file_filter
            void add_filter(const FileFilter& file_filter);

            /// @brief clear the list of file filters
            void clear_filters();

            /// @brief set the currently active filter. add_filter will be called if the filter is not yet added
            /// @param file_filter
            void set_initial_filter(const FileFilter& file_filter);

            /// @brief set the initially selected file
            /// @param file
            void set_initial_file(const FileDescriptor& file);

            /// @brief set the initially selected folder
            /// @param folder
            void set_initial_folder(const FileDescriptor& folder);

            /// @brief set initial name, only applies for FileChooserAction::SAVE
            void set_initial_name(const std::string&);

        private:
            detail::FileChooserInternal* _internal = nullptr;

            #if USE_NATIVE_FILE_CHOOSER
                static void on_native_dialog_response(GtkNativeDialog*, gint id, detail::FileChooserInternal* internal);
            #else
                static void on_file_dialog_ready_callback(GtkFileDialog*, GAsyncResult* result, detail::FileChooserInternal* internal);
            #endif
    };
}

#include "inline/file_chooser.inl"