//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 8/8/23
//

#pragma once

#include <mousetrap/widget.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class ActionBar;
    namespace detail
    {
        using ActionBarInternal = GtkActionBar;
        DEFINE_INTERNAL_MAPPING(ActionBar);
    }
    #endif

    class ActionBar : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(ActionBar, realize),
        HAS_SIGNAL(ActionBar, unrealize),
        HAS_SIGNAL(ActionBar, destroy),
        HAS_SIGNAL(ActionBar, hide),
        HAS_SIGNAL(ActionBar, show),
        HAS_SIGNAL(ActionBar, map),
        HAS_SIGNAL(ActionBar, unmap)
    {
        public:
            /// @brief construct
            ActionBar();

            /// @brief construct from internal
            /// @param internal
            ActionBar(detail::ActionBarInternal*);

            /// @brief destructor
            ~ActionBar();

            /// @copydoc SignalEmitter::get_internal
            NativeObject get_internal() const override;

            /// @brief add a widget to the end of the box
            /// @param widget
            void push_back(const Widget&);

            /// @brief add a widget to the start of the box
            /// @param widget
            void push_front(const Widget&);

            /// @brief remove a widget from the box
            /// @param widget widget already in box, if widget is not contained in box, does nothing
            void remove(const Widget&);

            /// @brief set centered widget
            /// @param widget
            void set_center_child(const Widget&);

            /// @brief remove centered widget
            void remove_center_child();

            /// @brief reveal the action bar
            /// @param b true if revealed, false otherwise
            void set_is_revealed(bool b);

            /// @brief get whether action bar is revealed
            /// @return revealed
            bool get_is_revealed() const;

        private:
            detail::ActionBarInternal* _internal = nullptr;
    };
}
