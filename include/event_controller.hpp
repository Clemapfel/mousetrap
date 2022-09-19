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

            ~EventController();

        protected:
            EventController(GtkEventController*);
            GtkEventController* _native;
    };

    /// handles keyboard key pressed
    class KeyEventController : public EventController
    {
        public:
            KeyEventController();

            // signal key pressed

            template<typename T>
            using on_key_pressed_function_t = bool(KeyEventController*, guint keyval, guint keycode, GdkModifierType state, T data);

            template<typename Function_t, typename T>
            void connect_signal_key_pressed(Function_t f, T data);

            // signal key released

            template<typename T>
            using on_key_released_function_t = void(KeyEventController* self, guint keyval, guint keycode, GdkModifierType state, T data);

            template<typename Function_t, typename T>
            void connect_signal_key_released(Function_t f, T data);

            // signal modifiers changed

            template<typename T>
            using on_modifiers_changed_function_t = bool(KeyEventController* self, GdkModifierType keyval, T data);

            template<typename Function_t, typename T>
            void connect_signal_modifiers_changed(Function_t f, T data);

        private:
            static void on_key_pressed_wrapper(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, KeyEventController* instance);
            std::function<on_key_pressed_function_t<void*>> _on_key_pressed_f;
            void* _on_key_pressed_data;

            static void on_key_released_wrapper(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, KeyEventController* instance);
            std::function<on_key_released_function_t<void*>> _on_key_released_f;
            void* _on_key_released_data;

            static void on_modifiers_changed_wrapper(GtkEventControllerKey* self, GdkModifierType state, KeyEventController* instance);
            std::function<on_modifiers_changed_function_t<void*>> _on_modifiers_changed_f;
            void* _on_modifiers_changed_data;
    };

    /// handles pointer motion
    class MotionEventController : public EventController
    {
        public:
            MotionEventController();

            using OnEnterSignature = void(*)(GtkEventControllerMotion* self, gdouble x, gdouble y, void* data);
            void connect_enter(OnEnterSignature f, void* data = nullptr);

            using OnLeaveSignature = void(*)(GtkEventControllerMotion* self, void* data);
            void connect_leave(OnLeaveSignature f, void* data = nullptr);

            using OnMotionSignature = void(*)(GtkEventControllerMotion* self, gdouble x, gdouble y, void* data);
            void connect_motion(OnMotionSignature f, void* data = nullptr);
    };

    /// handles mouse button press
    class ClickEventController : public EventController
    {
        public:
            ClickEventController();

            using OnPressedSignature = void(*)(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, void* data);
            void connect_pressed(OnPressedSignature f, void* data = nullptr);

            using OnReleasedSignature = void(*)(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, void* data);
            void connect_released(OnReleasedSignature f, void* data = nullptr);
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

#include <src/event_controller.inl>