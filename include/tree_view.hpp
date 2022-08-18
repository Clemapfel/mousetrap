// 
// Copyright 2022 Clemens Cords
// Created on 8/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>
#include <include/widget.hpp>

namespace mousetrap
{
    enum TreeViewRendererType
    {
        TEXT,
        PIXBUF,
        TOGGLE,
        COMBO_BOX,
        SPIN_BUTTON,
        KEYBOARD_ACCELERATOR,
        SPINNER,
        PROGRESS,
    };

    enum TreeViewColumnValueType
    {
        PIXBUF_STATIC, // GDK_TYPE_PIXBUF
        PIXBUF_TOGGLE, // GDK_TYPE_PIXBUF with expander

        TEXTURE_STATIC, // GDK_TYPE_TEXTURE
        TEXTURE_TOGGLE, // GDK_TYPE_TEXTURE with expander

        BOOLEAN, // G_TYPE_BOOLEAN
        CHAR,    // G_TYPE_CHAR
        UCHAR,   // G_TYPE_UCHAR
        INT32,   // G_TYPE_INT
        UINT32,  // G_TYPE_UINT
        INT64,   // G_TYPE_INT64
        UINT64,  // G_TYPE_UINT64
        FLOAT,   // G_TYPE_FLOAT
        DOUBLE,  // G_TYPE_DOUBLE
        STRING   // G_TYPE_STRING
    };

    /*
     * To support the tree view, GtkCellRendererPixbuf also supports rendering
     * two alternative pixbufs, when the GtkCellRenderer:is-expander property
     * is TRUE. If the GtkCellRenderer:is-expanded property is TRUE and the
     * GtkCellRendererPixbuf:pixbuf-expander-open property is set to a pixbuf,
     * it renders that pixbuf, if the GtkCellRenderer:is-expanded property
     * is FALSE and the GtkCellRendererPixbuf:pixbuf-expander-closed property
     * is set to a pixbuf, it renders that one.
     */

    using TreeViewIterator = GtkTreeIter;

    class TreeView : public Widget
    {
        public:
            /// \tparam ValueRendererPair_t: std::pair<TreeViewRendererType, TreeViewColumnValueType>
            template<typename... ValueRendererPair_t> requires (std::is_same_v<ValueRendererPair_t, std::pair<TreeViewColumnValueType, TreeViewRendererType>> && ...)
            TreeView(size_t n_columns, ValueRendererPair_t...);

            template<typename T>
            GtkTreeIter* add_value(size_t column_index, T value, GtkTreeIter* = nullptr);

    };


}