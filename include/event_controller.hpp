// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/signal_emitter.hpp>

#include <functional>

namespace mousetrap
{
    class EventController : public SignalEmitter
    {
        public:
            operator GObject*();
            operator GtkEventController*();

            void set_propagation_phase(GtkPropagationPhase);

        protected:
            EventController(GtkEventController*);
            GtkEventController* _native;
    };

    /// handles keyboard key pressed
    class KeyEventController : public EventController
    {
        public:
            KeyEventController();

            using OnKeyPressedSignature = gboolean(*)(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, void* user_data);
            void connect_key_pressed(OnKeyPressedSignature f, void* user_data = nullptr);

            using OnKeyReleasedSignature = void(*)(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, void* user_data);
            void connect_key_released(OnKeyReleasedSignature f, void* user_data = nullptr);
    };

    /// handles pointer motion
    class MotionEventController : public EventController
    {
        public:
            MotionEventController();

            using OnEnterSignature = void(*)(GtkEventControllerMotion* self, gdouble x, gdouble y, void* user_data);
            void connect_enter(OnEnterSignature f, void* user_data = nullptr);

            using OnLeaveSignature = void(*)(GtkEventControllerMotion* self, void* user_data);
            void connect_leave(OnLeaveSignature f, void* user_data = nullptr);

            using OnMotionSignature = void(*)(GtkEventControllerMotion* self, gdouble x, gdouble y, void* user_data);
            void connect_motion(OnMotionSignature f, void* user_data = nullptr);
    };

    /// handles mouse button press
    class ClickEventController : public EventController
    {
        public:
            ClickEventController();

            using OnPressedSignature = void(*)(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, void* user_data);
            void connect_pressed(OnPressedSignature f, void* user_data = nullptr);

            using OnReleasedSignature = void(*)(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, void* user_data);
            void connect_released(OnReleasedSignature f, void* user_data = nullptr);
    };

    // handles mouse scroll
    class ScrollEventController : public EventController
    {
        public:
            ScrollEventController(bool emit_vertical = true, bool emit_horizontal = false);

            using OnScrollBeginSignature = void(*)(GtkEventControllerScroll* self, void* user_data);
            void connect_scroll_begin(OnScrollBeginSignature f, void* user_data = nullptr);

            using OnScrollEndSignature = void(*)(GtkEventControllerScroll* self, void* user_data);
            void connect_scroll_end(OnScrollEndSignature f, void* user_data = nullptr);

            using OnScrollSignature = void(*)(GtkEventControllerScroll* self, gdouble dx, gdouble dy, void* user_data);
            void connect_scroll(OnScrollSignature f, void* user_data = nullptr);
    };

    // handles focus gain/loss
    class FocusEventController : public EventController
    {
        public:
            FocusEventController();

            using OnEnterSignature = void(*)(GtkEventControllerFocus* self, void*);
            void connect_enter(OnEnterSignature f, void* user_data = nullptr);

            using OnLeaveSignature = void(*)(GtkEventControllerFocus* self, void*);
            void connect_leave(OnLeaveSignature f, void* user_data = nullptr);
    };

    // handles two-finger pinch/zoom
    class ZoomGestureEventController : public EventController
    {
        public:
            ZoomGestureEventController();

            using OnScaleChangedSignature = void(*)(GtkGestureZoom* self, gdouble scale, void* user_data);
            void connect_scale_changed(OnScaleChangedSignature f, void* user_data = nullptr);
    };
}

// #include <src/event_controller.inl>

namespace mousetrap
{
    EventController::EventController(GtkEventController* controller)
    {
        _native = controller;
    }

    EventController::operator GObject*()
    {
        return G_OBJECT(_native);
    }

    EventController::operator GtkEventController*()
    {
        return _native;
    }

    void EventController::set_propagation_phase(GtkPropagationPhase phase)
    {
        gtk_event_controller_set_propagation_phase(_native, phase);
    }

    // KEY

    KeyEventController::KeyEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_key_new()))
    {}

    void KeyEventController::connect_key_pressed(OnKeyPressedSignature f, void* user_data)
    {
        connect_signal("key-pressed", f, user_data);
    }

    void KeyEventController::connect_key_released(OnKeyReleasedSignature f, void* user_data)
    {
        connect_signal("key-released", f, user_data);
    }

    // MOTION

    MotionEventController::MotionEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_motion_new()))
    {}

    void MotionEventController::connect_enter(OnEnterSignature f, void* user_data)
    {
        connect_signal("enter", f, user_data);
    }

    void MotionEventController::connect_leave(OnLeaveSignature f, void* user_data)
    {
        connect_signal("leave", f, user_data);
    }

    void MotionEventController::connect_motion(OnMotionSignature f, void* user_data)
    {
        connect_signal("motion", f, user_data);
    }

    // CLICK

    ClickEventController::ClickEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_click_new()))
    {}

    void ClickEventController::connect_pressed(OnPressedSignature f, void* user_data)
    {
        connect_signal("pressed", f, user_data);
    }

    void ClickEventController::connect_released(OnReleasedSignature f, void* user_data)
    {
        connect_signal("released", f, user_data);
    }

    // SCROLL

    ScrollEventController::ScrollEventController(bool emit_vertical, bool emit_horizontal)
        : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_scroll_new([&]() {

            if (emit_vertical and emit_horizontal)
               return GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES;
            else if (emit_vertical)
               return GTK_EVENT_CONTROLLER_SCROLL_VERTICAL;
            else if (emit_horizontal)
               return GTK_EVENT_CONTROLLER_SCROLL_HORIZONTAL;
            else
                return GTK_EVENT_CONTROLLER_SCROLL_NONE;
        }())))
    {}

    void ScrollEventController::connect_scroll(OnScrollSignature f, void* user_data)
    {
        connect_signal("scroll", f, user_data);
    }

    void ScrollEventController::connect_scroll_begin(OnScrollBeginSignature f, void* user_data)
    {
        connect_signal("scroll-begin", f, user_data);
    }

    void ScrollEventController::connect_scroll_end(OnScrollEndSignature f, void* user_data)
    {
        connect_signal("scroll-end", f, user_data);
    }

    // FOCUS

    FocusEventController::FocusEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_focus_new()))
    {}

    void FocusEventController::connect_enter(OnEnterSignature f, void* user_data)
    {
        connect_signal("enter", f, user_data);
    }

    void FocusEventController::connect_leave(OnLeaveSignature f, void* user_data)
    {
        connect_signal("leave", f, user_data);
    }

    // ZOOM

    ZoomGestureEventController::ZoomGestureEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_zoom_new()))
    {}

    void ZoomGestureEventController::connect_scale_changed(OnScaleChangedSignature f, void* user_data)
    {
        connect_signal("scale-changed", f, user_data);
    }
}