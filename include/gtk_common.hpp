// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

extern "C"
{
    ///
    bool gtk_initialize_opengl(GtkWindow* window);

    ///
    enum class GtkCursorType
    {
        NONE,
        DEFAULT,
        HELP,
        POINTER,
        CONTEXT_MENU,
        PROGRESS,
        WAIT,
        CELL,
        CROSSHAIR,
        TEXT,
        MOVE,
        NOT_ALLOWED,
        GRAB,
        GRABBING,
        ALL_SCROLL,
        ZOOM_IN,
        ZOOM_OUT,
        COLUMN_RESIZE,
        ROW_RESIZE,
        NORTH_RESIZE,
        NORTH_EAST_RESIZE,
        EAST_RESIZE,
        SOUTH_EAST_RESIZE,
        SOUTH_RESIZE,
        SOUTH_WEST_RESIZE,
        WEST_RESIZE,
        NORTH_WEST_RESIZE,
        HORIZONTAL_RESIZE = COLUMN_RESIZE,
        VERTICAL_RESIZE = ROW_RESIZE,
    };
}


#include <src/gtk_common.inl>
