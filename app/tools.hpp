//
// Copyright 2022 Clemens Cords
// Created on 9/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    using ToolID = std::string;
    
    static inline const ToolID MARQUEE_NEIGHBORHODD_SELECT = "marquee_neighborhood_select";
    static inline const ToolID MARQUEE_RECTANGLE = "marquee_rectangle";
    static inline const ToolID MARQUEE_RECTANGLE_ADD = "marquee_rectangle_add";
    static inline const ToolID MARQUEE_RECTANGLE_SUBTRACT = "marquee_rectangle_subtract";
    static inline const ToolID MARQUEE_CIRCLE = "marquee_circle";
    static inline const ToolID MARQUEE_CIRCLE_ADD = "marquee_circle_add";
    static inline const ToolID MARQUEE_CIRCLE_SUBTRACT = "marquee_circle_subtract";
    static inline const ToolID MARQUEE_POLYGON = "marquee_polygon";
    static inline const ToolID MARQUEE_POLYGON_ADD = "marquee_polygon_add";
    static inline const ToolID MARQUEE_POLYGON_SUBTRACT = "marquee_polygon_subtract";
    
    static inline const ToolID PENCIL = "pencil";
    static inline const ToolID BRUSH = PENCIL;
    static inline const ToolID ERASE = "eraser";
    static inline const ToolID EYEDROPPER = "eyedropper";
    static inline const ToolID BUCKET_FILL = "bucket_fill";
    static inline const ToolID LINE = "line";
    
    static inline const ToolID RECTANGLE_OUTLINE = "rectangle_outline";
    static inline const ToolID RECTANGLE_FILL = "rectangle_fill";
    static inline const ToolID CIRCLE_OUTLINE = "circle_outline";
    static inline const ToolID CIRCLE_FILL = "circle_fill";
    static inline const ToolID POLYGON_OUTLINE = "polygon_outline";
    static inline const ToolID POLYGON_FILL = "polygon_fill";
   
    static inline const ToolID GRADIENT_DITHERED = "gradient_dithered";
    static inline const ToolID GRADIENT_SMOOTH = "gradient_smooth";
    
    enum class BrushType
    {
        RECTANGLE,
        CIRCLE,
        CUSTOM
    };
}