// 
// Copyright 2022 Clemens Cords
// Created on 12/19/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class EditableLabel : public WidgetImplementation<GtkEditableLabel> // TODO gtk editable signals
    {
        public:
            EditableLabel(const std::string&);

            void set_text(const std::string&);
            std::string get_text() const;

            void set_is_editing(bool b, bool accept_value = true);
            bool get_is_editing() const;
    };
}

// #include <source/editable_label.inl>

namespace mousetrap
{
    EditableLabel::EditableLabel(const std::string& str)
        : WidgetImplementation<GtkEditableLabel>(GTK_EDITABLE_LABEL(gtk_editable_label_new(str.c_str())))
    {}

    void EditableLabel::set_text(const std::string& str)
    {
        gtk_editable_set_text(GTK_EDITABLE(get_native()), str.c_str());
    }

    std::string EditableLabel::get_text() const
    {
        return gtk_editable_get_text(GTK_EDITABLE(get_native()));
    }

    void EditableLabel::set_is_editing(bool b, bool accept_value)
    {
        auto current = gtk_editable_label_get_editing(get_native());
        if (current and not b)
            gtk_editable_label_stop_editing(get_native(), accept_value);
        else if (not current and b)
            gtk_editable_label_start_editing(get_native());
    }

    bool EditableLabel::get_is_editing() const
    {
        return gtk_editable_label_get_editing(get_native());
    }
}