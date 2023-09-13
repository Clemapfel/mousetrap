//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 8/26/23
//

#pragma once

#include <mousetrap/widget.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class PopupMessage;
    class PopupMessageOverlay;
    namespace detail
    {
        using PopupMessageOverlayInternal = AdwToastOverlay;
        DEFINE_INTERNAL_MAPPING(PopupMessageOverlay);
        
        using PopupMessageInternal = AdwToast;
        DEFINE_INTERNAL_MAPPING(PopupMessage);
    }
    #endif

    /// @brief
    class PopupMessage : public SignalEmitter,
        HAS_SIGNAL(PopupMessage, dismissed),
        HAS_SIGNAL(PopupMessage, button_clicked)
    {
        public:
            /// @brief construct from string
            /// @param formatted_string
            PopupMessage(const std::string& formatted_string, const std::string& button_label = "");

            /// @brief create from internal
            PopupMessage(detail::PopupMessageInternal*);

            /// @brief destructor
            ~PopupMessage();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief expose a GObject
            operator NativeObject() const;

            /// @brief set title string
            void set_title(const std::string&);

            /// @brief get title string
            std::string get_title() const;

            /// @brief set whether this message has elevated priority
            /// @param b true if high priority, false otherwise
            void set_is_high_priority(bool);

            /// @brief get whether this message has elevated priority
            /// @return true if high priority, false otherwise
            bool get_is_high_priority() const;

            /// @brief set time limit after which the message will dissapear
            /// @param duration time, or 0 for inifinite (default)
            void set_timeout(Time duration);

            /// @brief set time limit after which the message will dissapear
            /// @return time, or 0 for inifinite (default)
            Time get_timeout() const;

            /// @brief set button label, or "" to hide button
            void set_button_label(const std::string&);

            /// @brief get title string
            std::string get_button_label() const;

            /// @brief set action triggered when button is clicked
            /// @param action
            void set_button_action(const Action&);

            /// @brief get id of action set via `set_button_action`, or "" if no action was set
            std::string get_button_action_id() const;



        private:
            detail::PopupMessageInternal* _internal = nullptr;
    };
    
    /// @brief Overlay that allows displaying a `PopupMessage`
    class PopupMessageOverlay : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(PopupMessageOverlay, realize),
        HAS_SIGNAL(PopupMessageOverlay, unrealize),
        HAS_SIGNAL(PopupMessageOverlay, destroy),
        HAS_SIGNAL(PopupMessageOverlay, hide),
        HAS_SIGNAL(PopupMessageOverlay, show),
        HAS_SIGNAL(PopupMessageOverlay, map),
        HAS_SIGNAL(PopupMessageOverlay, unmap)
    {
        public:
            /// @brief construct as empty
            PopupMessageOverlay();

            /// @brief create from internal
            PopupMessageOverlay(detail::PopupMessageOverlayInternal*);

            /// @brief destructor
            ~PopupMessageOverlay();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief set child
            void set_child(const Widget&);

            /// @brief remove child
            void remove_child();

            /// @brief Present a popup message
            void show_message(const PopupMessage&);

        private:
            detail::PopupMessageOverlayInternal* _internal = nullptr;
    };
}
