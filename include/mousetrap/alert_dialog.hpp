//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 6/26/23
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/signal_emitter.hpp>

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

        };
        using AlertDialogInternal = _AlertDialogInternal;
        DEFINE_INTERNAL_MAPPING(AlertDialog);
    }
    #endif

    class AlertDialog : public detail::notify_if_gtk_uninitialized
    {
        public:
            AlertDialog(const std::string& message, const std::string& detailed_message = "");

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

            /// TODO
            template<typename Function_t>
            void on_selection(Function_t);

            /// TODO
            template<typename Function_t, typename Data_t>
            void on_selection(Function_t, Data_t);


            /// @brief set whether the dialogs window is modal
            /// @param b true if it should be modal, false otherwise
            void set_is_modal(bool b);

            /// @brief get whether the dialogs window is modal
            /// @return true if modal, false otherwise
            bool get_is_modal() const;
    };

    //#endif
}
