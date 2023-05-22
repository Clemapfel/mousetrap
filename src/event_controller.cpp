//
// Created by clem on 3/18/23.
//


#include <mousetrap/event_controller.hpp>
#include <mousetrap/vector.hpp>

namespace mousetrap
{
    EventController::EventController(GtkEventController* controller)
    {
        _internal = (detail::EventControllerInternal*) controller;
    }

    EventController::~EventController()
    {
        g_object_unref(_internal);
    }

    EventController::operator GObject*() const
    {
        return G_OBJECT(_internal);
    }

    NativeObject EventController::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void EventController::set_propagation_phase(PropagationPhase phase)
    {
        gtk_event_controller_set_propagation_phase(_internal, (GtkPropagationPhase) phase);
    }

    PropagationPhase EventController::get_propagation_phase() const
    {
        return (PropagationPhase) gtk_event_controller_get_propagation_phase(_internal);
    }
}

namespace mousetrap
{
    ButtonID SingleClickGesture::get_current_button() const
    {
        auto id = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(EventController::get_internal()));
        return id == 0 ? ButtonID::NONE : (ButtonID) id;
    }

    void SingleClickGesture::set_only_listens_to_button(ButtonID id)
    {
        gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(EventController::get_internal()), (guint) id);
    }

    ButtonID SingleClickGesture::get_only_listens_to_button() const
    {
        return (ButtonID) gtk_gesture_single_get_button(GTK_GESTURE_SINGLE(EventController::get_internal()));
    }

    void SingleClickGesture::set_touch_only(bool b)
    {
        gtk_gesture_single_set_touch_only(GTK_GESTURE_SINGLE(EventController::get_internal()), b);
    }

    bool SingleClickGesture::get_touch_only() const
    {
        return gtk_gesture_single_get_touch_only(GTK_GESTURE_SINGLE(EventController::get_internal()));
    }

    SingleClickGesture::SingleClickGesture(GtkGestureSingle* gesture)
        : EventController(GTK_EVENT_CONTROLLER(gesture))
    {}
}
