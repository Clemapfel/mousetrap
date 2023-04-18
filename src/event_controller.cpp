//
// Created by clem on 3/18/23.
//


#include <include/event_controller.hpp>
#include <include/vector.hpp>

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

    EventController::operator GObject*() const
    {
        return G_OBJECT(_native);
    }

    EventController::operator GtkEventController*() const
    {
        return _native;
    }

    void EventController::set_propagation_phase(PropagationPhase phase)
    {
        gtk_event_controller_set_propagation_phase(_native, (GtkPropagationPhase) phase);
    }

    PropagationPhase EventController::get_propagation_phase() const
    {
        return (PropagationPhase) gtk_event_controller_get_propagation_phase(_native);
    }
}

namespace mousetrap
{
    ButtonID SingleClickGesture::get_current_button() const
    {
        auto id = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(_native));
        return id == 0 ? ButtonID::NONE : (ButtonID) id;
    }

    void SingleClickGesture::set_only_listens_to_button(ButtonID id)
    {
        gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(_native), (guint) id);
    }

    ButtonID SingleClickGesture::get_only_listens_to_button() const
    {
        return (ButtonID) gtk_gesture_single_get_button(GTK_GESTURE_SINGLE(_native));
    }

    void SingleClickGesture::set_touch_only(bool b)
    {
        gtk_gesture_single_set_touch_only(GTK_GESTURE_SINGLE(_native), b);
    }

    bool SingleClickGesture::get_touch_only() const
    {
        return gtk_gesture_single_get_touch_only(GTK_GESTURE_SINGLE(_native));
    }

    SingleClickGesture::SingleClickGesture(GtkGestureSingle* gesture)
        : EventController(GTK_EVENT_CONTROLLER(gesture))
    {}
}
