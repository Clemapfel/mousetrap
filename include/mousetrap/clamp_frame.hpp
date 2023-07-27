//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 7/27/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientation.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class ClampFrame;
    namespace detail
    {
        using ClampFrameInternal = AdwClamp;
        DEFINE_INTERNAL_MAPPING(ClampFrame);
    }
    #endif

    /// @brief container widget enforces a maximum size
    /// \signals
    /// \widget_signals{ClampFrame}
    class ClampFrame : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(ClampFrame, realize),
        HAS_SIGNAL(ClampFrame, unrealize),
        HAS_SIGNAL(ClampFrame, destroy),
        HAS_SIGNAL(ClampFrame, hide),
        HAS_SIGNAL(ClampFrame, show),
        HAS_SIGNAL(ClampFrame, map),
        HAS_SIGNAL(ClampFrame, unmap)

    {
        public:
            /// @brief construct
            /// @param orientation clamps width if horizontal, height if vertical
            ClampFrame(Orientation);

            /// @brief construct from internal \internal
            /// @param internal
            ClampFrame(detail::ClampFrameInternal*);

            /// @brief dtor
            ~ClampFrame();

            /// @copydoc SignalEmitter::get_internal
            NativeObject get_internal() const override;

            /// @brief set orientation
            /// @param orientation
            void set_orientation(Orientation);

            /// @brief get orientation
            /// @return orientation
            Orientation get_orientation() const;

            /// @brief set the maximum width if horizontal, height if vertical
            /// @param size in px
            void set_maximum_size(float);

            /// @brief get maximum width if horizontal, height if vertical
            /// @return size in px
            float get_maximum_size() const;

            /// @brief set child
            /// @param widget
            void set_child(const Widget& widget);

            /// @brief remove child, equivalent to <tt>set_child(nullptr)</tt>
            void remove_child();

        private:
            detail::ClampFrameInternal* _internal = nullptr;
    };
}