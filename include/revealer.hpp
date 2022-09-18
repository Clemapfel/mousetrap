// 
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    enum class TransitionType
    {
        NONE = GTK_REVEALER_TRANSITION_TYPE_NONE,
        CROSSFADE = GTK_REVEALER_TRANSITION_TYPE_CROSSFADE,
        SLIDE_LEFT_TO_RIGHT = GTK_REVEALER_TRANSITION_TYPE_SLIDE_RIGHT,
        SLIDE_RIGHT_TO_LEFT = GTK_REVEALER_TRANSITION_TYPE_SLIDE_LEFT,
        SLIDE_TOP_TO_BOTTOM = GTK_REVEALER_TRANSITION_TYPE_SLIDE_DOWN,
        SLIDE_BOTTOM_TO_TOP = GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP,
        SWING_LEFT_TO_RIGHT = GTK_REVEALER_TRANSITION_TYPE_SWING_RIGHT,
        SWING_RIGHT_TO_LEFT = GTK_REVEALER_TRANSITION_TYPE_SWING_LEFT,
        SWING_TOP_TO_BOTTOM = GTK_REVEALER_TRANSITION_TYPE_SWING_DOWN,
        SWING_BOTTOM_TO_TOP = GTK_REVEALER_TRANSITION_TYPE_SWING_UP,
    };

    class Revealer : public WidgetImplementation<GtkRevealer>
    {
        public:
            Revealer(TransitionType = TransitionType::CROSSFADE);

            void set_child(Widget*);

            void set_revealed(bool b);
            bool get_revealed();
            void set_transition_type(TransitionType);

        private:
            GtkRevealer* _native;
    };
}

#include <src/revealer.inl>
