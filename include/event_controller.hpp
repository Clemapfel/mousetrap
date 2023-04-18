//
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/signal_emitter.hpp>
#include <include/signal_component.hpp>
#include <include/vector.hpp>

#include <functional>

namespace mousetrap
{
    /// @brief event controller propagation phase, decides at what part of the event loop the handler is invoked
    /// @see https://docs.gtk.org/gtk4/enum.PropagationPhase.html
    enum class PropagationPhase
    {
        /// @brief is never triggered
        NONE = GTK_PHASE_NONE,

        /// @brief is triggered during event propagation down from the toplevel window towards the inner-most widget
        CAPTURE = GTK_PHASE_CAPTURE,

        /// @brief is triggered during event propagation up from the inner most widget to the toplevel window
        BUBBLE = GTK_PHASE_BUBBLE,

        /// @brief triggered only when containing widget specifically pulls an event
        TARGET = GTK_PHASE_TARGET
    };

    /// @brief event controller
    class EventController : public SignalEmitter
    {
        public:
            /// @brief default ctor delete, cannot be instantiated. Use one of its base classes instead
            EventController() = delete;

            /// @brief dtor
            ~EventController();

            /// @brief expose as GObject \internal
            operator GObject*() const override;

            /// @brief expose as GtkEventController \internal
            operator GtkEventController*() const;

            /// @brief set propagation phase
            /// @param phase
            void set_propagation_phase(PropagationPhase);

            /// @brief get propagation phase
            /// @return phase
            PropagationPhase get_propagation_phase() const;

        protected:
            EventController(GtkEventController*);
            GtkEventController* _native;
    };

    /// @brief id of a mouse button
    enum class ButtonID : guint
    {
        /// @brief none, always invalid
        NONE = guint(-1),

        /// @brief represents any mouse button
        ANY = 0,

        /// @brief first mouse button, usually left
        BUTTON_01 = 1,

        /// @brief second mouse button, usually right
        BUTTON_02 = 2,

        /// @brief third mouse button
        BUTTON_03 = 3,

        /// @brief fourth mouse button
        BUTTON_04 = 4,

        /// @brief fifth mouse button
        BUTTON_05 = 5,

        /// @brief sixth mouse button
        BUTTON_06 = 6,

        /// @brief seventh mouse button
        BUTTON_07 = 7,

        /// @brief eighth mouse button
        BUTTON_08 = 8,

        /// @brief nineth mouse button
        BUTTON_09 = 9,
    };

    /// @brief single-click gesture, can be activated by a mouse button press or touchscreen tap
    class SingleClickGesture : public EventController
    {
        public:
            /// @brief get the button for the current event
            /// @return ButtonID, may be mousetrap::ButtonID::NONE if no event is currently bound
            ButtonID get_current_button() const;

            /// @brief set which button the event controller should listen to
            /// @param id button, ButtonID::ANY to listen to any, ButtonID::NONE to listen to no button
            void set_only_listens_to_button(ButtonID);

            /// @brief get which button the event controller should listen to
            /// @return button id, or ButtonID::ANY if listening to any, ButtonID::NONE if listening to no button
            ButtonID get_only_listens_to_button() const;

            /// @brief get whether the controller should only list to touch events
            /// @param b true if only listening to touch events, false otherwise
            bool get_touch_only() const;

            /// @brief set whether the controller should only listen to touch events
            /// @param b true if it should only consider touch events, false otherwise
            void set_touch_only(bool);

        protected:
            SingleClickGesture(GtkGestureSingle*);
    };
}
