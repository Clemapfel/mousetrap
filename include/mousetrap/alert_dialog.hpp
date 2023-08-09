//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 6/26/23
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/signal_emitter.hpp>
#include <mousetrap/window.hpp>

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

            AdwMessageDialog* native;
            std::vector<std::string>* button_labels;
            std::function<void(AlertDialog&, int id)>* on_selection;
        };
        using AlertDialogInternal = _AlertDialogInternal;
        DEFINE_INTERNAL_MAPPING(AlertDialog);
    }
    #endif

    class AlertDialog : public detail::notify_if_gtk_uninitialized
    {
        public:
            /// @brief construct
            AlertDialog(const std::string& message, const std::string& detailed_message = "");

            /// @brief construct from internal
            AlertDialog(detail::AlertDialogInternal*);

            /// @brief dtor
            ~AlertDialog();

            using ButtonID = int;

            /// @param append a button to the end of the button row
            /// @param label button label
            /// @returns buttons internal ID
            ButtonID add_button(const std::string&);

            /// @brief make it so button with given ID is activated when the user presses enter
            /// @param button id, obtained from `add_button`
            void set_default_button(ButtonID, bool color_as_suggested = false);

            /// @brief set the label of a button
            /// @param id
            /// @param new_label
            void set_button_label(ButtonID, const std::string& new_label);

            /// @brief get the ID of the button at given position, from left to right
            /// @param position
            /// @return button id
            std::string get_button_label(int) const;

            /// @brief get number of buttons
            /// @return size_t
            size_t get_n_buttons() const;

            /// @brief add a widget underneath the message body
            /// @param widget
            void set_extra_widget(const Widget&);

            /// @brief remove the widget underneath the message body
            void remove_extra_widget();

            /// @brief expose internal
            NativeObject get_internal() const;

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

            /// @brief hide the window
            void close() const;

        private:
            detail::AlertDialogInternal* _internal = nullptr;
            static void on_response(AdwMessageDialog*, gchar* id, detail::AlertDialogInternal* internal);
    };

    //#endif

    template<typename Function_t, typename Data_t>
    void AlertDialog::on_selection(Function_t f_in, Data_t data_in)
    {
        delete _internal->on_selection;
        _internal->on_selection = new std::function<void(AlertDialog&, int)>([f = f_in, data = data_in](AlertDialog& dialog, int id){
            f(dialog, id, data);
        });
    }

    template<typename Function_t>
    void AlertDialog::on_selection(Function_t f_in)
    {
        delete _internal->on_selection;
        _internal->on_selection = new std::function<void(AlertDialog&, int)>([f = f_in](AlertDialog& dialog, int id){
            f(dialog, id);
        });
    }
}
