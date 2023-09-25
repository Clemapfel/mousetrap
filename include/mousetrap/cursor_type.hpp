//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/17/23
//

#pragma once

namespace mousetrap
{
    /// @brief cursor type
    /// @see https://docs.gtk.org/gdk4/ctor.Cursor.new_from_name.html
    enum class CursorType
    {
        /// @brief invisible cursor
        NONE,

        /// @brief default, usually equal to mousetrap::CursorType::POINTER
        DEFAULT,

        /// @brief help symbol, usually a questionmark
        HELP,

        /// @brief pointer
        POINTER,

        /// @brief open context menu
        CONTEXT_MENU,

        /// @brief loading symbol
        PROGRESS,

        /// @brief instruct user to wait for an action to complete
        WAIT,

        /// @brief cell-wise editing
        CELL,

        /// @brief cross hair cursor
        CROSSHAIR,

        /// @brief text insert caret
        TEXT,

        /// @brief move, usually a compass rose like shape
        MOVE,

        /// @brief a "no" sign
        NOT_ALLOWED,

        /// @brief an open hand, about to grab something
        GRAB,

        /// @brief a closed hand, currently grabbing something
        GRABBING,

        /// @brief
        ALL_SCROLL,

        /// @brief zoom in, usually a magnifier with a plus
        ZOOM_IN,

        /// @brief zoom out, usually a magnifier with a minus
        ZOOM_OUT,

        /// @brief column resize, usually a up-down-arrow
        COLUMN_RESIZE,

        /// @brief row resize, usually a left-right-arrow
        ROW_RESIZE,

        /// @brief top bar
        NORTH_RESIZE,

        /// @brief top right corner
        NORTH_EAST_RESIZE,

        /// @brief right bar
        EAST_RESIZE,

        /// @brief bottom right corner
        SOUTH_EAST_RESIZE,

        /// @brief bottom bar
        SOUTH_RESIZE,

        /// @brief bottom left corner
        SOUTH_WEST_RESIZE,

        /// @brief left bar
        WEST_RESIZE,

        /// @brief top left corner
        NORTH_WEST_RESIZE,

        /// @brief horizontal resize, equal to column resize
        HORIZONTAL_RESIZE = COLUMN_RESIZE,

        /// @brief vertical resize, equal to row resize
        VERTICAL_RESIZE = ROW_RESIZE,
    };

}