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

        MARQUEE_RECTANGLE_REPLACE = 1,
        MARQUEE_RECTANGLE_ADD = MARQUEE_RECTANGLE_REPLACE + 1,
        MARQUEE_RECTANGLE_SUBTRACT = MARQUEE_RECTANGLE_REPLACE + 2,

        MARQUEE_CIRCLE_REPLACE = 4,
        MARQUEE_CIRCLE_ADD = MARQUEE_CIRCLE_REPLACE + 1,
        MARQUEE_CIRCLE_SUBTRACT = MARQUEE_CIRCLE_REPLACE + 2,

        MARQUEE_POLYGON_REPLACE = 7,
        MARQUEE_POLYGON_ADD = MARQUEE_POLYGON_REPLACE + 1,
        MARQUEE_POLYGON_SUBTRACT = MARQUEE_POLYGON_REPLACE + 2,

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

        GRADIENT,

        MARQUEE_RECTANGLE = MARQUEE_RECTANGLE_REPLACE,
        MARQUEE_CIRCLE = MARQUEE_CIRCLE_REPLACE,
        MARQUEE_POLYGON = MARQUEE_POLYGON_REPLACE,
    };

    inline std::string tool_id_to_string(ToolID id)
    {
        if (id == MOVE_SELECTION) return "move_selection";
        else if (id == MARQUEE_NEIGHBORHODD_SELECT) return "marquee_neighborhood_select";
        else if (id == MARQUEE_RECTANGLE) return "marquee_rectangle_replace";
        else if (id == MARQUEE_RECTANGLE_ADD) return "marquee_rectangle_add";
        else if (id == MARQUEE_RECTANGLE_SUBTRACT) return "marquee_rectangle_subtract";
        else if (id == MARQUEE_CIRCLE) return "marquee_circle_replace";
        else if (id == MARQUEE_CIRCLE_ADD) return "marquee_circle_add";
        else if (id == MARQUEE_CIRCLE_SUBTRACT) return "marquee_circle_subtract";
        else if (id == MARQUEE_POLYGON) return "marquee_polygon_replace";
        else if (id == MARQUEE_POLYGON_ADD) return "marquee_polygon_add";
        else if (id == MARQUEE_POLYGON_SUBTRACT) return "marquee_polygon_subtract";
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
    }

    inline ToolID string_to_tool_id(std::string id)
    {
        if (id == "move_selection") return MOVE_SELECTION;
        else if (id == "marquee_neighborhood_select") return MARQUEE_NEIGHBORHODD_SELECT;
        else if (id == "marquee_rectangle_replace") return MARQUEE_RECTANGLE;
        else if (id == "marquee_rectangle_add") return MARQUEE_RECTANGLE_ADD;
        else if (id == "marquee_rectangle_subtract") return MARQUEE_RECTANGLE_SUBTRACT;
        else if (id == "marquee_circle_replace") return MARQUEE_CIRCLE;
        else if (id == "marquee_circle_add") return MARQUEE_CIRCLE_ADD;
        else if (id == "marquee_circle_subtract") return MARQUEE_CIRCLE_SUBTRACT;
        else if (id == "marquee_polygon_replace") return MARQUEE_POLYGON;
        else if (id == "marquee_polygon_add") return MARQUEE_POLYGON_ADD;
        else if (id == "marquee_polygon_subtract") return MARQUEE_POLYGON_SUBTRACT;
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
    }
}