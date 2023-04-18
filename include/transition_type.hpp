//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/20/23
//

#pragma once

#include <include/gtk_common.hpp>

namespace mousetrap
{
    /// @brief type of animation when mousetrap::Revealer is triggered
    enum class RevealerTransitionType
    {
        /// @brief do not animate, is revealed instantly
        NONE = GTK_REVEALER_TRANSITION_TYPE_NONE,

        /// @brief crossfade by slowly increasing opacity from 0 to 1
        CROSSFADE = GTK_REVEALER_TRANSITION_TYPE_CROSSFADE,

        /// @brief slide from left to right
        SLIDE_RIGHT = GTK_REVEALER_TRANSITION_TYPE_SLIDE_RIGHT,

        /// @brief slide from right to left
        SLIDE_LEFT = GTK_REVEALER_TRANSITION_TYPE_SLIDE_LEFT,

        /// @brief slide from top to bottom
        SLIDE_DOWN = GTK_REVEALER_TRANSITION_TYPE_SLIDE_DOWN,

        /// @brief slide from bottom to top
        SLIDE_UP = GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP,

        /// @brief swing from left to right
        SWING_RIGHT = GTK_REVEALER_TRANSITION_TYPE_SWING_RIGHT,

        /// @brief swing from right to left
        SWING_LEFT = GTK_REVEALER_TRANSITION_TYPE_SWING_LEFT,

        /// @brief swing from top to bottom
        SWING_DOWN = GTK_REVEALER_TRANSITION_TYPE_SWING_DOWN,

        /// @brief swing from bottom to top
        SWING_UP = GTK_REVEALER_TRANSITION_TYPE_SWING_UP,
    };

    /// @brief type of animation when switching between pages of a mousetrap::Stack
    enum class StackTransitionType
    {
        /// @brief do not animate, is revealed immediately
        NONE = GTK_STACK_TRANSITION_TYPE_NONE,

        /// @brief crossfade by slowly increasing opacity from 0 to 1
        CROSSFADE = GTK_STACK_TRANSITION_TYPE_CROSSFADE,

        /// @brief slide from left to right
        SLIDE_RIGHT = GTK_STACK_TRANSITION_TYPE_SLIDE_RIGHT,

        /// @brief slide from right to left
        SLIDE_LEFT = GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT,

        /// @brief slide from top to bottom
        SLIDE_DOWN = GTK_STACK_TRANSITION_TYPE_SLIDE_DOWN,

        /// @brief slide from bottom to top
        SLIDE_UP = GTK_STACK_TRANSITION_TYPE_SLIDE_UP,

        /// @brief slide to the left, enter from the right
        SLIDE_LEFT_RIGHT = GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT,

        /// @brief slide to top, enter from bottom
        SLIDE_UP_DOWN = GTK_STACK_TRANSITION_TYPE_SLIDE_UP_DOWN,

        /// @brief slide above previous page from bottom to top
        OVER_UP = GTK_STACK_TRANSITION_TYPE_OVER_UP,

        /// @brief slide above previous page from top to bottom
        OVER_DOWN = GTK_STACK_TRANSITION_TYPE_OVER_DOWN,

        /// @brief slide above previous page from right to left
        OVER_LEFT = GTK_STACK_TRANSITION_TYPE_OVER_LEFT,

        /// @brief slide above previous page from left to right
        OVER_RIGHT = GTK_STACK_TRANSITION_TYPE_OVER_RIGHT,

        /// @brief slide below previous page from bottom to top
        UNDER_UP = GTK_STACK_TRANSITION_TYPE_UNDER_UP,

        /// @brief slide below previous page from top to bottom
        UNDER_DOWN = GTK_STACK_TRANSITION_TYPE_UNDER_DOWN,

        /// @brief slide below previous page from right to left
        UNDER_LEFT = GTK_STACK_TRANSITION_TYPE_UNDER_LEFT,

        /// @brief slide below previous page from left to right
        UNDER_RIGHT = GTK_STACK_TRANSITION_TYPE_UNDER_RIGHT,

        /// @brief slide above previous page up, enter from bottom
        OVER_UP_DOWN = GTK_STACK_TRANSITION_TYPE_OVER_UP_DOWN,

        /// @brief slide above previous page down, enter from top
        OVER_DOWN_UP = GTK_STACK_TRANSITION_TYPE_OVER_DOWN_UP,

        /// @brief slide above previous page left, enter from the right
        OVER_LEFT_RIGHT = GTK_STACK_TRANSITION_TYPE_OVER_LEFT_RIGHT,

        /// @brief slide above previous page right, enter from the left
        OVER_RIGHT_LEFT = GTK_STACK_TRANSITION_TYPE_OVER_RIGHT_LEFT,

        /// @brief rotate as if all pages where on a horizontal wheel, rotate left
        ROTATE_LEFT = GTK_STACK_TRANSITION_TYPE_ROTATE_LEFT,

        /// @brief rotate as if all pages where on a horizontal wheel, rotate right
        ROTATE_RIGHT = GTK_STACK_TRANSITION_TYPE_ROTATE_RIGHT,

        /// @brief rotate as if all pages where on a horizontal wheel, rotate left, enter from the right
        ROTATE_LEFT_RIGHT = GTK_STACK_TRANSITION_TYPE_ROTATE_LEFT_RIGHT
    };
}