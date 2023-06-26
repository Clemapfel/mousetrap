//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 6/26/23
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/signal_emitter.hpp>

#include <map>
#include <functional>

namespace mousetrap
{
    //#if GTK_MINOR_VERSION >= 10

    #ifndef DOXYGEN
    class AlertDialog;
    namespace detail
    {
        struct _AlertDialogInternal
        {
            GObject parent;

            GtkAlertDialog* native;
            std::vector<std::string>* button_labels;
            std::function<void(AlertDialog&, int)>* on_selection;
        };
        using AlertDialogInternal = _AlertDialogInternal;
        DEFINE_INTERNAL_MAPPING(AlertDialog);
    }
    #endif

    class AlertDialog : public detail::notify_if_gtk_uninitialized
    {
        public:
            /// @brief construct
            AlertDialog(const std::vector<std::string>& button_labels, const std::string& message, const std::string& detailed_message = "");

            /// @brief construct from internal
            AlertDialog(detail::AlertDialogInternal*);

            /// @brief dtor
            ~AlertDialog();

            /// @brief expose internal
            NativeObject get_internal() const;

            /// @brief add button at specific position
            /// @param index index after which to insert, -1 for start
            /// @param label
            void add_button(int index, const std::string& label);

            /// @brief remove button at specific position
            void remove_button(size_t index);

            /// @brief set label of button at index
            /// @param index button index
            /// @param label
            void set_button_label(size_t index, const std::string&);

            /// @brief get label of button at index
            /// @return string
            std::string get_button_label(int index) const;

            /// @brief get number of buttons
            /// @return number
            size_t get_n_buttons() const;

            /// @brief get dialogs main message
            /// @return string
            std::string get_message() const;

            /// @brief set dialogs main message
            /// @param string
            void set_message(const std::string& message);

            /// @brief get dialogs detailed description
            /// @return string
            std::string get_detailed_description() const;

            /// @brief set dialogs detailed description
            /// @param string
            void set_detailed_description(const std::string&);

            /// @brief connect a function to be called when the user makes a selection
            /// @param function Function with signature `(AlertDialog&, int button_id) -> void`
            template<typename Function_t>
            void on_selection(Function_t);

            /// @brief connect a function to be called when the user makes a selection
            /// @param function Function with signature `(AlertDialog&, int button_id, Data_t) -> void`
            template<typename Function_t, typename Data_t>
            void on_selection(Function_t, Data_t);

            /// @brief set whether the dialogs window is modal
            /// @param b true if it should be modal, false otherwise
            void set_is_modal(bool b);

            /// @brief get whether the dialogs window is modal
            /// @return true if modal, false otherwise
            bool get_is_modal() const;

            /// @brief show the dialog to the user
            void present() const;

        private:
            detail::AlertDialogInternal* _internal = nullptr;
            void update_buttons();
            static void on_choose_callback(GObject* source, GAsyncResult*, void* data);
    };

    //#endif

    template<typename Function_t, typename Data_t>
    void AlertDialog::on_selection(Function_t f_in, Data_t data_in)
    {
        delete _internal->on_selection;
        _internal->on_selection = new std::function<void(AlertDialog&, int)>([f = f_in, data = data_in](AlertDialog& dialog, int index){
            f(dialog, index, data);
        });
    }

    template<typename Function_t>
    void AlertDialog::on_selection(Function_t f_in)
    {
        delete _internal->on_selection;
        _internal->on_selection = new std::function<void(AlertDialog&, int)>([f = f_in](AlertDialog& dialog, int index){
            f(dialog, index);
        });
    }
}
