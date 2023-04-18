//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/11/23
//

#pragma once

#include <include/event_controller.hpp>

namespace mousetrap
{
    /// @brief type of stylus tool
    enum class ToolType
    {
        /// @brief unknown
        UNKNOWN = GDK_DEVICE_TOOL_TYPE_UNKNOWN,

        /// @brief tablet stylus
        PEN = GDK_DEVICE_TOOL_TYPE_UNKNOWN,

        /// @brief tablet eraser
        ERASER = GDK_DEVICE_TOOL_TYPE_UNKNOWN,

        /// @brief brush stylus
        BRUSH = GDK_DEVICE_TOOL_TYPE_UNKNOWN,

        /// @brief pencil stylus
        PENCIL = GDK_DEVICE_TOOL_TYPE_UNKNOWN,

        /// @brief airbrush tool
        AIRBRUSH = GDK_DEVICE_TOOL_TYPE_UNKNOWN,

        /// @brief tool is a lens cursor
        LENS = GDK_DEVICE_TOOL_TYPE_UNKNOWN,

        /// @brief tool is actually a mouse
        MOUSE = GDK_DEVICE_TOOL_TYPE_UNKNOWN
    };

    /// @brief axis of stylus, not all of these will be used
    enum class DeviceAxis
    {
        /// @brief x position
        X = GDK_AXIS_X,

        /// @brief y position
        Y = GDK_AXIS_Y,

        /// @brief difference along x axis
        DELTA_X = GDK_AXIS_DELTA_X,

        /// @brief difference along y axis
        DELTA_Y = GDK_AXIS_DELTA_Y,

        /// @brief pressure sensor
        PRESSURE = GDK_AXIS_PRESSURE,

        /// @brief x tilt
        X_TILT = GDK_AXIS_XTILT,

        /// @brief y tilt
        Y_TILT = GDK_AXIS_YTILT,

        /// @brief stylus wheel, if present
        WHEEL = GDK_AXIS_WHEEL,
        
        /// @brief distance of stylus top from touchpad
        DISTANCE = GDK_AXIS_DISTANCE,
        
        /// @brief rotation of the stlus
        ROTATION = GDK_AXIS_ROTATION,
        
        /// @brief stylus slider, if present
        SLIDER = GDK_AXIS_SLIDER,
    };

    /// @brief get human-readable id of axis
    /// @param axis
    /// @return string
    std::string device_axis_to_string(DeviceAxis);

    /// @brief handles events emitted by a touchpad stylus
    /// @todo expose high resolution motion history: https://docs.gtk.org/gtk4/method.GestureStylus.get_backlog.html
    class StylusEventController : public SingleClickGesture,
        HAS_SIGNAL(StylusEventController, stylus_down),
        HAS_SIGNAL(StylusEventController, stylus_up),
        HAS_SIGNAL(StylusEventController, proximity),
        HAS_SIGNAL(StylusEventController, motion)
    {
        public:
            /// @brief construct
            StylusEventController();

            /// @brief get hardware id of stylus
            /// @return id, or 0 if ID is unknown
            size_t get_hardware_id() const;

            /// @brief get current tool type the stylus
            /// @return tool type
            ToolType get_device_type() const;

            /// @brief get whether device has specified axis
            /// @param axis
            /// @return true if axis is present and can be queried
            bool has_axis(DeviceAxis) const;

            /// @brief get axis value
            /// @param axis
            /// @return value, if axis is not present, value will be 0
            double get_axis_value(DeviceAxis) const;
    };
}
