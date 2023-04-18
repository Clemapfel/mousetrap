//
// Created by clem on 4/11/23.
//

#include <include/stylus_event_controller.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    StylusEventController::StylusEventController()
        : SingleClickGesture(GTK_GESTURE_SINGLE(gtk_gesture_stylus_new())),
            CTOR_SIGNAL(StylusEventController, stylus_down),
            CTOR_SIGNAL(StylusEventController, stylus_up),
            CTOR_SIGNAL(StylusEventController, proximity),
            CTOR_SIGNAL(StylusEventController, motion)
    {}

    size_t StylusEventController::get_hardware_id() const
    {
        auto* device = gtk_gesture_stylus_get_device_tool(GTK_GESTURE_STYLUS(_native));
        if (device == nullptr)
            return 0;

        return gdk_device_tool_get_hardware_id(device);
    }

    ToolType StylusEventController::get_device_type() const
    {
        auto* device = gtk_gesture_stylus_get_device_tool(GTK_GESTURE_STYLUS(_native));
        if (device == nullptr)
            return ToolType::UNKNOWN;

        return (ToolType) gdk_device_tool_get_tool_type(device);
    }

    bool StylusEventController::has_axis(DeviceAxis axis) const
    {
        double value;
        return gtk_gesture_stylus_get_axis(GTK_GESTURE_STYLUS(_native), (GdkAxisUse) axis, &value);
    }

    std::string device_axis_to_string(DeviceAxis axis)
    {
        switch (axis)
        {
            case DeviceAxis::X:
                return "X";
            case DeviceAxis::Y:
                return "Y";
            case DeviceAxis::DELTA_X:
                return "DELTA_X";
            case DeviceAxis::DELTA_Y:
                return "DELTA_Y";
            case DeviceAxis::PRESSURE:
                return "PRESSURE";
            case DeviceAxis::X_TILT:
                return "X_TILT";
            case DeviceAxis::Y_TILT:
                return "Y_TILT";
            case DeviceAxis::WHEEL:
                return "WHEEL";
            case DeviceAxis::DISTANCE:
                return "DISTANCE";
            case DeviceAxis::ROTATION:
                return "ROTATION";
            case DeviceAxis::SLIDER:
                return "SLIDER";
            default:
                return "UNKNOWN";
        }
    }

    double StylusEventController::get_axis_value(DeviceAxis axis) const
    {
        double value;
        auto has = gtk_gesture_stylus_get_axis(GTK_GESTURE_STYLUS(_native), (GdkAxisUse) axis, &value);

        if (has)
            return value;
        else
        {
            log::warning("In StylusEventController::get_axis_value: Unable to query value for DeviceAxis::" + device_axis_to_string(axis));
            return 0;
        }
    }
}