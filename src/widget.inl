// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <iostream>

namespace mousetrap
{
    Widget::operator GObject*()
    {
        return G_OBJECT(operator GtkWidget*());
    }

    Vector2f Widget::get_size_request()
    {
        int w, h;
        gtk_widget_get_size_request(this->operator GtkWidget*(), &w, &h);
        return Vector2f(w, h);
    }

    void Widget::set_margin(float value)
    {
        set_margin_bottom(value);
        set_margin_top(value);
        set_margin_start(value);
        set_margin_end(value);
    }

    void Widget::set_margin_bottom(float value)
    {
        gtk_widget_set_margin_bottom(this->operator GtkWidget*(), value);
    }

    void Widget::set_margin_top(float value)
    {
        gtk_widget_set_margin_top(this->operator GtkWidget*(), value);
    }

    void Widget::set_margin_start(float value)
    {
        gtk_widget_set_margin_start(this->operator GtkWidget*(), value);
    }

    void Widget::set_margin_end(float value)
    {
        gtk_widget_set_margin_end(this->operator GtkWidget*(), value);
    }

    void Widget::set_hexpand(bool should_expand)
    {
        gtk_widget_set_hexpand(this->operator GtkWidget*(), should_expand == TRUE);
    }

    void Widget::set_vexpand(bool should_expand)
    {
        gtk_widget_set_vexpand(this->operator GtkWidget*(), should_expand == TRUE);
    }

    void Widget::set_expand(bool both)
    {
        gtk_widget_set_hexpand(this->operator GtkWidget*(), both == TRUE);
        gtk_widget_set_vexpand(this->operator GtkWidget*(), both == TRUE);
    }

    void Widget::set_halign(GtkAlign alignment)
    {
        gtk_widget_set_halign(this->operator GtkWidget*(), alignment);
    }

    void Widget::set_valign(GtkAlign alignment)
    {
        gtk_widget_set_valign(this->operator GtkWidget*(), alignment);
    }

    void Widget::set_align(GtkAlign both)
    {
        gtk_widget_set_halign(this->operator GtkWidget*(), both);
        gtk_widget_set_valign(this->operator GtkWidget*(), both);
    }

    void Widget::set_size_request(Vector2f size)
    {
        gtk_widget_set_size_request(this->operator GtkWidget*(), size.x, size.y);
    }

    void Widget::set_opacity(float value)
    {
        gtk_widget_set_opacity(this->operator GtkWidget*(), value);
    }

    float Widget::get_opacity()
    {
        return gtk_widget_get_opacity(this->operator GtkWidget*());
    }

    Vector2f Widget::get_size()
    {
        GtkAllocation* allocation = new GtkAllocation();
        gtk_widget_get_allocation(this->operator GtkWidget*(), allocation);

        Vector2f out = Vector2f{allocation->width, allocation->height};
        delete allocation;

        return out;
    }

    void Widget::set_tooltip_text(const std::string& text)
    {
        gtk_widget_set_tooltip_markup(this->operator GtkWidget*(), text.c_str());
    }

    void Widget::show()
    {
        gtk_widget_show(this->operator GtkWidget*());
    }

    void Widget::set_cursor(GtkCursorType cursor)
    {

    }
}
