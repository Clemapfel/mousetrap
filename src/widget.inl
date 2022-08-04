// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <iostream>

namespace mousetrap
{
    Widget::operator GtkWidget*()
    {
        return get_native();
    }

    bool Widget::warn_if_nullptr(const std::string& function_id)
    {
        if (get_native() == nullptr)
        {
            std::cerr << "[WARNING] In Widget::" << function_id << ": widget unitialized, get_native() returning nullptr"
                      << std::endl;
            return true;
        }
        return false;
    }

    void Widget::set_signal_blocked(const std::string& signal_id, bool b)
    {
        if (warn_if_nullptr("set_signal_blocked"))
            return;

        auto it = _signal_handlers.find(signal_id);

        if (it == _signal_handlers.end())
        {
            std::cerr << "[WARNING] In Widget::set_signal_blocked: no signal with id \"" << signal_id << "\" connected."
                      << std::endl;
            return;
        }

        if (b)
            g_signal_handler_block(get_native(), it->second);
        else
            g_signal_handler_unblock(get_native(), it->second);
    }

    void Widget::set_all_signals_blocked(bool b)
    {
        if (warn_if_nullptr("set_all_signals_blocked"))
            return;

        for (auto& pair : _signal_handlers)
        {
            if (b)
                g_signal_handler_block(get_native(), pair.second);
            else
                g_signal_handler_unblock(get_native(), pair.second);
        }
    }

    template<typename Function_t>
    void Widget::connect_signal(const std::string& signal_id, Function_t* function, void* data)
    {
        if (warn_if_nullptr("connect_signal"))
            return;

        auto handler_id = g_signal_connect(get_native(), signal_id.c_str(), G_CALLBACK(function), data);
        _signal_handlers.insert_or_assign(signal_id, handler_id);
    }

    Vector2f Widget::get_size_request()
    {
        if (warn_if_nullptr("get_size_request"))
            return Vector2f{0, 0};

        int w, h;
        gtk_widget_get_size_request(get_native(), &w, &h);
        return Vector2f(w, h);
    }

    void Widget::set_margin(float value)
    {
        if (warn_if_nullptr("set_margin"))
            return;

        set_margin_bottom(value);
        set_margin_top(value);
        set_margin_start(value);
        set_margin_end(value);
    }

    void Widget::set_margin_bottom(float value)
    {
        if (warn_if_nullptr("set_margin_bottom"))
            return;
        gtk_widget_set_margin_bottom(get_native(), value);
    }

    void Widget::set_margin_top(float value)
    {
        if (warn_if_nullptr("set_margin_top"))
            return;

        gtk_widget_set_margin_top(get_native(), value);
    }

    void Widget::set_margin_start(float value)
    {
        if (warn_if_nullptr("set_margin_start"))
            return;

        gtk_widget_set_margin_start(get_native(), value);
    }

    void Widget::set_margin_end(float value)
    {
        if (warn_if_nullptr("set_margin_end"))
            return;

        gtk_widget_set_margin_end(get_native(), value);
    }

    void Widget::set_hexpand(bool should_expand)
    {
        if (warn_if_nullptr("set_hexpand"))
            return;

        gtk_widget_set_hexpand(get_native(), should_expand == TRUE);
    }

    void Widget::set_vexpand(bool should_expand)
    {
        if (warn_if_nullptr("set_vexpand"))
            return;

        gtk_widget_set_vexpand(get_native(), should_expand == TRUE);
    }

    void Widget::set_halign(GtkAlign alignment)
    {
        if (warn_if_nullptr("set_halign"))
            return;

        gtk_widget_set_halign(get_native(), alignment);
    }

    void Widget::set_valign(GtkAlign alignment)
    {
        if (warn_if_nullptr("set_valign"))
            return;

        gtk_widget_set_valign(get_native(), alignment);
    }

    void Widget::set_size_request(Vector2f size)
    {
        if (warn_if_nullptr("set_size_request"))
            return;

        gtk_widget_set_size_request(get_native(), size.x, size.y);
    }

    void Widget::set_opacity(float value)
    {
        if (warn_if_nullptr("set_opacity"))
            return;

        gtk_widget_set_opacity(get_native(), value);
    }

    float Widget::get_opacity()
    {
        if (warn_if_nullptr("set_opacity"))
            return 0;

        return gtk_widget_get_opacity(get_native());
    }

    Vector2f Widget::get_size()
    {
        if (warn_if_nullptr("get_size"))
            return Vector2f{0, 0};

        GtkAllocation* allocation = new GtkAllocation();
        gtk_widget_get_allocation(get_native(), allocation);

        Vector2f out = Vector2f{allocation->width, allocation->height};
        delete allocation;

        return out;
    }

    void Widget::set_tooltip_text(const std::string& text)
    {
        if (warn_if_nullptr("set_tooltip_text"))
            return;

        gtk_widget_set_tooltip_text(get_native(), text.c_str());
    }

    void Widget::add_events(GdkEventMask event)
    {
        if (warn_if_nullptr("add_events"))
            return;

        gtk_widget_add_events(get_native(), event);
    }
}
