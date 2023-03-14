//
// Copyright 2022 Clemens Cords
// Created on 9/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    enum ToolID
    {
        MOVE_SELECTION = -1,

        MARQUEE_NEIGHBORHODD_SELECT = 0,
        MARQUEE_RECTANGLE,
        MARQUEE_CIRCLE,
        MARQUEE_POLYGON,

        ERASER,
        BRUSH,
        BUCKET_FILL,
        COLOR_SELECT,
        LINE,

        RECTANGLE_OUTLINE,
        CIRCLE_OUTLINE,
        POLYGON_OUTLINE,

        RECTANGLE_FILL,
        CIRCLE_FILL,
        POLYGON_FILL,

        GRADIENT
    };

    inline std::string tool_id_to_string(ToolID id)
    {
        if (id == MOVE_SELECTION) return "move_selection";
        else if (id == MARQUEE_NEIGHBORHODD_SELECT) return "marquee_neighborhood_select";
        else if (id == MARQUEE_RECTANGLE) return "marquee_rectangle";
        else if (id == MARQUEE_CIRCLE) return "marquee_circle";
        else if (id == MARQUEE_POLYGON) return "marquee_polygon";
        else if (id == BRUSH) return "brush";
        else if (id == ERASER) return "eraser";
        else if (id == COLOR_SELECT) return "color_select";
        else if (id == BUCKET_FILL) return "bucket_fill";
        else if (id == LINE) return "line";
        else if (id == RECTANGLE_OUTLINE) return "rectangle_outline";
        else if (id == RECTANGLE_FILL) return "rectangle_fill";
        else if (id == CIRCLE_OUTLINE) return "circle_outline";
        else if (id == CIRCLE_FILL) return "circle_fill";
        else if (id == POLYGON_OUTLINE) return "polygon_outline";
        else if (id == POLYGON_FILL) return "polygon_fill";
        else if (id == GRADIENT) return "gradient";
        else
            return "brush"; // unreachable
    }

    inline ToolID string_to_tool_id(std::string id)
    {
        if (id == "move_selection") return MOVE_SELECTION;
        else if (id == "marquee_neighborhood_select") return MARQUEE_NEIGHBORHODD_SELECT;
        else if (id == "marquee_rectangle") return MARQUEE_RECTANGLE;
        else if (id == "marquee_circle") return MARQUEE_CIRCLE;
        else if (id == "marquee_polygon") return MARQUEE_POLYGON;
        else if (id == "pencil" or id == "brush") return BRUSH;
        else if (id == "eraser") return ERASER;
        else if (id == "color_select") return COLOR_SELECT;
        else if (id == "bucket_fill") return BUCKET_FILL;
        else if (id == "line") return LINE;
        else if (id == "rectangle_outline") return RECTANGLE_OUTLINE;
        else if (id == "rectangle_fill") return RECTANGLE_FILL;
        else if (id == "circle_outline") return CIRCLE_OUTLINE;
        else if (id == "circle_fill") return CIRCLE_FILL;
        else if (id == "polygon_outline") return POLYGON_OUTLINE;
        else if (id == "polygon_fill") return POLYGON_FILL;
        else if (id == "gradient") return GRADIENT;
        else
            return BRUSH; // unreachable
    }

    enum class SelectionMode
    {
        REPLACE,
        ADD,
        SUBTRACT
    };
}