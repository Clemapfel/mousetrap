// 
// Copyright 2022 Clemens Cords
// Created on 9/20/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/event_controller.hpp> // TODO

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
            : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_key_new()))
    {}

    template<typename Function_t, typename T>
    void KeyEventController::connect_signal_key_pressed(Function_t f, T data)
    {
        auto temp =  std::function<on_key_pressed_function_t<T>>(f);
        _on_key_pressed_f = std::function<on_key_pressed_function_t<void*>>(*((std::function<on_key_pressed_function_t<void*>>*) &temp));
        _on_key_pressed_data = data;
        connect_signal("key-pressed", f, data);
    }

    void KeyEventController::on_key_pressed_wrapper(GtkEventControllerKey* self, guint keyval, guint keycode,
                                                    GdkModifierType state, KeyEventController* instance)
    {
        if (instance->_on_key_pressed_data == nullptr)
            return;

        (instance->_on_key_pressed_f)(instance, keyval, keycode, state, instance->_on_key_pressed_data);
    }

    template<typename Function_t, typename T>
    void KeyEventController::connect_signal_key_released(Function_t f, T data)
    {
        auto temp =  std::function<on_key_released_function_t<T>>(f);
        _on_key_released_f = std::function<on_key_released_function_t<void*>>(*((std::function<on_key_released_function_t<void*>>*) &temp));
        _on_key_released_data = data;
        connect_signal("key-released", f, data);
    }

    void KeyEventController::on_key_released_wrapper(GtkEventControllerKey* self, guint keyval, guint keycode,
                                                     GdkModifierType state, KeyEventController* instance)
    {
        if (instance->_on_key_released_data == nullptr)
            return;

        (instance->_on_key_released_f)(instance, keyval, keycode, state, instance->_on_key_released_data);
    }

    template<typename Function_t, typename T>
    void KeyEventController::connect_signal_modifiers_changed(Function_t f, T data)
    {
        auto temp =  std::function<on_modifiers_changed_function_t<T>>(f);
        _on_modifiers_changed_f = std::function<on_modifiers_changed_function_t<void*>>(*((std::function<on_modifiers_changed_function_t<void*>>*) &temp));
        _on_modifiers_changed_data = data;
        connect_signal("key-released", f, data);
    }

    void KeyEventController::on_modifiers_changed_wrapper(GtkEventControllerKey*, GdkModifierType state, KeyEventController* instance)
    {
        if (instance->_on_modifiers_changed_data == nullptr)
            return;

        (instance->_on_modifiers_changed_f)(instance, state, instance->_on_modifiers_changed_data);
    }

    // MOTION

    MotionEventController::MotionEventController()
            : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_motion_new()))
    {}

    void MotionEventController::connect_enter(OnEnterSignature f, void* data)
    {
        connect_signal("enter", f, data);
    }

    void MotionEventController::connect_leave(OnLeaveSignature f, void* data)
    {
        connect_signal("leave", f, data);
    }

    void MotionEventController::connect_motion(OnMotionSignature f, void* data)
    {
        connect_signal("motion", f, data);
    }

    // CLICK

    ClickEventController::ClickEventController()
            : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_click_new()))
    {}

    void ClickEventController::connect_pressed(OnPressedSignature f, void* data)
    {
        connect_signal("pressed", f, data);
    }

    void ClickEventController::connect_released(OnReleasedSignature f, void* data)
    {
        connect_signal("released", f, data);
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