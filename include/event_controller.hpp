// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/signal_emitter.hpp>
#include <include/signal_component.hpp>

#include <functional>

namespace mousetrap
{
    class EventController : public SignalEmitter
    {
        public:
            operator GObject*();
            operator GtkEventController*();

            void set_propagation_phase(GtkPropagationPhase);

            ~EventController();

        protected:
            EventController(GtkEventController*);
            GtkEventController* _native;
    };

    /// handles keyboard key pressed
    class KeyEventController : public EventController,
        public HasKeyPressedSignal<KeyEventController>,
        public HasKeyReleasedSignal<KeyEventController>,
        public HasModifiersChangedSignal<KeyEventController>
    {
        public:
            KeyEventController();
    };

    /// handles pointer motion
    class MotionEventController : public EventController,
        public HasMotionEnterSignal<MotionEventController>,
        public HasMotionLeaveSignal<MotionEventController>,
        public HasMotionSignal<MotionEventController>
    {
        public:
            MotionEventController();
    };

    /// handles mouse button press
    class ClickEventController : public EventController,
        public HasClickPressedSignal<ClickEventController>,
        public HasClickReleasedSignal<ClickEventController>
    {
        public:
            ClickEventController();
    };

    // handles mouse scroll
    class ScrollEventController : public EventController
    {
        public:
            ScrollEventController(bool emit_vertical = true, bool emit_horizontal = false);

            using OnScrollBeginSignature = void(*)(GtkEventControllerScroll* self, void* data);
            void connect_scroll_begin(OnScrollBeginSignature f, void* data = nullptr);

            using OnScrollEndSignature = void(*)(GtkEventControllerScroll* self, void* data);
            void connect_scroll_end(OnScrollEndSignature f, void* data = nullptr);

            using OnScrollSignature = void(*)(GtkEventControllerScroll* self, gdouble dx, gdouble dy, void* data);
            void connect_scroll(OnScrollSignature f, void* data = nullptr);
    };

    // handles focus gain/loss
    class FocusEventController : public EventController
    {
        public:
            FocusEventController();

            using OnEnterSignature = void(*)(GtkEventControllerFocus* self, void*);
            void connect_enter(OnEnterSignature f, void* data = nullptr);

            using OnLeaveSignature = void(*)(GtkEventControllerFocus* self, void*);
            void connect_leave(OnLeaveSignature f, void* data = nullptr);
    };

    // handles two-finger pinch/zoom
    class ZoomGestureEventController : public EventController
    {
        public:
            ZoomGestureEventController();

            using OnScaleChangedSignature = void(*)(GtkGestureZoom* self, gdouble scale, void* data);
            void connect_scale_changed(OnScaleChangedSignature f, void* data = nullptr);
    };
}

// #include <src/event_controller.inl>

namespace mousetrap
{
    EventController::EventController(GtkEventController* controller)
    {
        _native = g_object_ref(controller);
    }

    EventController::~EventController()
    {
        g_object_unref(_native);
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
        : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_key_new())),
          HasKeyPressedSignal<KeyEventController>(this),
          HasKeyReleasedSignal<KeyEventController>(this),
          HasModifiersChangedSignal<KeyEventController>(this)
    {}

    MotionEventController::MotionEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_motion_new())),
          HasMotionEnterSignal<MotionEventController>(this),
          HasMotionLeaveSignal<MotionEventController>(this),
          HasMotionSignal<MotionEventController>(this)
    {}

    ClickEventController::ClickEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_click_new())),
          HasClickPressedSignal<ClickEventController>(this),
          HasClickReleasedSignal<ClickEventController>(this)
    {}

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

    void ScrollEventController::connect_scroll(OnScrollSignature f, void* data)
    {
        connect_signal("scroll", f, data);
    }

    void ScrollEventController::connect_scroll_begin(OnScrollBeginSignature f, void* data)
    {
        connect_signal("scroll-begin", f, data);
    }

    void ScrollEventController::connect_scroll_end(OnScrollEndSignature f, void* data)
    {
        connect_signal("scroll-end", f, data);
    }

    // FOCUS

    FocusEventController::FocusEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_focus_new()))
    {}

    void FocusEventController::connect_enter(OnEnterSignature f, void* data)
    {
        connect_signal("enter", f, data);
    }

    void FocusEventController::connect_leave(OnLeaveSignature f, void* data)
    {
        connect_signal("leave", f, data);
    }

    // ZOOM

    ZoomGestureEventController::ZoomGestureEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_zoom_new()))
    {}

    void ZoomGestureEventController::connect_scale_changed(OnScaleChangedSignature f, void* data)
    {
        connect_signal("scale-changed", f, data);
    }
}