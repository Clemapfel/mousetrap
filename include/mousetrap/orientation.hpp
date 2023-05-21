//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/15/23
//

#pragma once

#include <mousetrap/gtk_common.hpp>

namespace mousetrap
{
    /// @brief orientation of an object
    enum class Orientation
    {
        /// @brief oriented along the x axis, left to right
        HORIZONTAL = GTK_ORIENTATION_HORIZONTAL,

        /// @brief oriented along the y axis, top to bottom
        VERTICAL = GTK_ORIENTATION_VERTICAL
    };

    /// \todo REMOVE THIS
    #ifdef UNDEF
    /// @copydoc Box::get_orientation
    Orientation get_orientation() const;

    /// @copydoc Box::set_orientation
    void set_orientation(Orientation orientation);
    #endif

    #ifndef DOXYGEN
        #define IMPLEMENT_ORIENTABLE(T) \
        void T::set_orientation(Orientation orientation) \
        { \
            gtk_orientable_set_orientation(GTK_ORIENTABLE(operator NativeObject()), (GtkOrientation) orientation); \
        } \
        \
        Orientation T::get_orientation() const \
        { \
            return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(operator NativeObject()));  \
        }
    #endif
}