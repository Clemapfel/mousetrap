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
            void set_propagation_limit(GtkPropagationLimit);
            GdkEvent* get_current_event();

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
    class ScrollEventController : public EventController,
        public HasScrollBeginSignal<ScrollEventController>,
        public HasScrollEndSignal<ScrollEventController>,
        public HasScrollSignal<ScrollEventController>
    {
        public:
            ScrollEventController(bool emit_vertical = true, bool emit_horizontal = true);
    };

    // handles focus gain/loss
    class FocusEventController : public EventController,
        public HasFocusGainedSignal<FocusEventController>,
        public HasFocusLostSignal<FocusEventController>
    {
        public:
            FocusEventController();
    };

    // handles drag start end
    class DragEventController : public EventController,
        public HasDragBeginSignal<DragEventController>,
        public HasDragEndSignal<DragEventController>,
        public HasDragUpdateSignal<DragEventController>
    {
        public:
            DragEventController();

            Vector2f get_start_position();
            Vector2f get_current_offset();
    };

    // handles pinch zoom
    class ZoomEventController : public EventController,
        public HasScaleChangedSignal<ZoomEventController>
    {
        public:
            ZoomEventController();

            float get_scale_delta();
    };
}

#include <src/event_controller.inl>
