// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <iostream>
#include <sstream>

namespace mousetrap
{
    Widget::operator GObject*()
    {
        return G_OBJECT(operator GtkWidget*());
    }

    Widget::operator GtkWidget*() const
    {
        return GTK_WIDGET(_native);
    }

    template<typename T>
    Widget::Widget(T* in)
        : HasRealizeSignal<Widget>(this), HasMapSignal<Widget>(this), HasShowSignal<Widget>(this)
    {
        _native = g_object_ref(GTK_WIDGET(in));
    }

    template<typename T>
    WidgetImplementation<T>::WidgetImplementation(T* in)
        : Widget(in)
    {
        if (not GTK_IS_WIDGET(in))
            throw std::invalid_argument("[FATAL] In WidgetImplementation::WidgetImplementation(T*): Object is not a widget.");
    }

    template<typename T>
    WidgetImplementation<T>::operator T*() const
    {
        return (T*) Widget::operator GtkWidget*();
    }

    template<typename T>
    T* WidgetImplementation<T>::get_native() const
    {
        return (T*) Widget::operator GtkWidget*();
    }

    template<typename GObject_t>
    void Widget::add_reference(GObject_t* ref)
    {
        if (not G_IS_OBJECT(ref))
            throw std::invalid_argument("[FATAL] In Widget::add_reference: Attempting to increase the reference count on an object that does not support it.");

        _refs.push_back(g_object_ref(G_OBJECT(ref)));
    }

    void Widget::override_native(GtkWidget* new_native)
    {
        auto* old_native = _native;
        _native = g_object_ref(new_native);
        g_object_unref(old_native);
    }

    Widget::~Widget()
    {
        if (gtk_widget_get_parent(_native) == nullptr)
            g_object_unref(_native);

        for (auto* ref : _refs)
            g_object_unref(ref);
    }

    Vector2f Widget::get_size_request()
    {
        int w, h;
        gtk_widget_get_size_request(operator GtkWidget*(), &w, &h);
        return Vector2f(w, h);
    }

    void Widget::set_margin(float value)
    {
        set_margin_bottom(value);
        set_margin_top(value);
        set_margin_start(value);
        set_margin_end(value);
    }

    void Widget::set_margin_horizontal(float value)
    {
        set_margin_start(value);
        set_margin_end(value);
    }

    void Widget::set_margin_vertical(float value)
    {
        set_margin_top(value);
        set_margin_bottom(value);
    }

    void Widget::set_margin_bottom(float value)
    {
        gtk_widget_set_margin_bottom(operator GtkWidget*(), value);
    }

    void Widget::set_margin_top(float value)
    {
        gtk_widget_set_margin_top(operator GtkWidget*(), value);
    }

    void Widget::set_margin_start(float value)
    {
        gtk_widget_set_margin_start(operator GtkWidget*(), value);
    }

    void Widget::set_margin_end(float value)
    {
        gtk_widget_set_margin_end(operator GtkWidget*(), value);
    }

    void Widget::set_hexpand(bool should_expand)
    {
        gtk_widget_set_hexpand(operator GtkWidget*(), should_expand == TRUE);
    }

    void Widget::set_vexpand(bool should_expand)
    {
        gtk_widget_set_vexpand(operator GtkWidget*(), should_expand == TRUE);
    }

    void Widget::set_expand(bool both)
    {
        gtk_widget_set_hexpand(operator GtkWidget*(), both == TRUE);
        gtk_widget_set_vexpand(operator GtkWidget*(), both == TRUE);
    }

    void Widget::set_halign(GtkAlign alignment)
    {
        gtk_widget_set_halign(operator GtkWidget*(), alignment);
    }

    void Widget::set_valign(GtkAlign alignment)
    {
        gtk_widget_set_valign(operator GtkWidget*(), alignment);
    }

    void Widget::set_align(GtkAlign both)
    {
        gtk_widget_set_halign(operator GtkWidget*(), both);
        gtk_widget_set_valign(operator GtkWidget*(), both);
    }

    void Widget::set_size_request(Vector2f size)
    {
        gtk_widget_set_size_request(operator GtkWidget*(), size.x, size.y);
    }

    void Widget::set_opacity(float value)
    {
        gtk_widget_set_opacity(operator GtkWidget*(), value);
    }

    float Widget::get_opacity()
    {
        return gtk_widget_get_opacity(operator GtkWidget*());
    }

    Vector2f Widget::get_size()
    {
        GtkAllocation* allocation = new GtkAllocation();
        gtk_widget_get_allocation(operator GtkWidget*(), allocation);

        Vector2f out = Vector2f{allocation->width, allocation->height};
        delete allocation;

        return out;
    }

    bool Widget::operator==(const Widget& other) const
    {
        return this->_native == other._native;
    }

    bool Widget::operator!=(const Widget& other) const
    {
        return not (*this == other);
    }

    void Widget::set_tooltip_text(const std::string& text)
    {
        gtk_widget_set_tooltip_markup(operator GtkWidget*(), text.c_str());
    }

    void Widget::set_visible(bool b)
    {
        gtk_widget_set_visible(operator GtkWidget*(), b);
    }

    bool Widget::get_visible()
    {
        return gtk_widget_get_visible(operator GtkWidget*());
    }

    void Widget::show()
    {
        gtk_widget_show(operator GtkWidget*());
    }

    void Widget::hide()
    {
        gtk_widget_hide(operator GtkWidget*());
    }

    void Widget::set_cursor(GtkCursorType cursor)
    {
        switch (cursor)
        {
            case GtkCursorType::NONE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "none");

            case GtkCursorType::DEFAULT:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "default");

            case GtkCursorType::HELP:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "help");

            case GtkCursorType::POINTER:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "pointer");

            case GtkCursorType::CONTEXT_MENU:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "context-menu");

            case GtkCursorType::PROGRESS:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "progress");

            case GtkCursorType::WAIT:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "wait");

            case GtkCursorType::CELL:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "cell");

            case GtkCursorType::CROSSHAIR:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "crosshair");

            case GtkCursorType::TEXT:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "text");

            case GtkCursorType::MOVE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "move");

            case GtkCursorType::NOT_ALLOWED:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "not-allowed");

            case GtkCursorType::GRAB:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "grab");

            case GtkCursorType::GRABBING:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "grabbing");

            case GtkCursorType::ALL_SCROLL:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "all-scroll");

            case GtkCursorType::ZOOM_IN:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "zoom-in");

            case GtkCursorType::ZOOM_OUT:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "zoom-out");

            case GtkCursorType::COLUMN_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "col-resize");

            case GtkCursorType::ROW_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "row-resize");

            case GtkCursorType::NORTH_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "n-resize");

            case GtkCursorType::NORTH_EAST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "ne-resize");

            case GtkCursorType::EAST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "e-resize");

            case GtkCursorType::SOUTH_EAST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "se-resize");

            case GtkCursorType::SOUTH_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "s-resize");

            case GtkCursorType::SOUTH_WEST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "sw-resize");

            case GtkCursorType::WEST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "w-resize");

            case GtkCursorType::NORTH_WEST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "nw-resize");
          }
    }

    void Widget::add_controller(EventController* controller)
    {
        gtk_widget_add_controller(operator GtkWidget*(), controller->operator GtkEventController*());
        gtk_widget_set_focusable(operator GtkWidget*(), true);
    }

    void Widget::set_focusable(bool b)
    {
        gtk_widget_set_focusable(operator GtkWidget*(), b);
    }

    void Widget::set_focus_on_click(bool b)
    {
        gtk_widget_set_focus_on_click(operator GtkWidget*(), b);
    }

    bool Widget::get_has_focus()
    {
        return gtk_widget_has_focus(operator GtkWidget*());
    }

    void Widget::grab_focus()
    {
        gtk_widget_grab_focus(operator GtkWidget*());
    }

    bool Widget::get_is_realized()
    {
        return gtk_widget_get_realized(operator GtkWidget*());
    }

    Widget::preferred_size Widget::get_preferred_size()
    {
        auto min = gtk_requisition_new();
        auto nat = gtk_requisition_new();
        gtk_widget_get_preferred_size(operator GtkWidget*(), min, nat);

        return Widget::preferred_size{{min->width, min->height}, {nat->width, nat->height}};
    }

    void Widget::unparent()
    {
        if (gtk_widget_get_parent(operator GtkWidget*()) != nullptr)
            gtk_widget_unparent(operator GtkWidget*());
    }

    void Widget::set_can_respond_to_input(bool b)
    {
        gtk_widget_set_sensitive(operator GtkWidget*(), b);
    }

    void Widget::set_tooltip_widget(Widget* widget)
    {
        _tooltip_widget = widget;
        gtk_widget_set_has_tooltip(operator GtkWidget*(), true);
        g_signal_connect(operator GtkWidget*(), "query-tooltip", G_CALLBACK(on_query_tooltip), this);
    }

    gboolean Widget::on_query_tooltip(GtkWidget*, gint x, gint y, gboolean, GtkTooltip* tooltip, Widget* instance)
    {
        if (instance->_tooltip_widget == nullptr)
            return false;

        gtk_tooltip_set_custom(tooltip, instance->_tooltip_widget->operator GtkWidget*());
        return true;
    }

    void Widget::beep()
    {
        if (gtk_widget_get_parent(operator GtkWidget*()) != nullptr)
            gtk_widget_error_bell(operator GtkWidget*());
    }

    template<typename Function_t, typename Arg_t>
    void Widget::add_tick_callback(Function_t f_in, Arg_t arg_in)
    {
        _tick_callback_f = [f = f_in, arg = arg_in](GdkFrameClock* clock) -> bool{
            return f(FrameClock(clock), arg);
        };

        gtk_widget_add_tick_callback(
            _native,
            (GtkTickCallback) G_CALLBACK(tick_callback_wrapper),
            this,
            (GDestroyNotify) G_CALLBACK(tick_callback_destroy_notify)
        );
    }

    gboolean Widget::tick_callback_wrapper(GtkWidget*, GdkFrameClock* clock, Widget* instance)
    {
        if (instance->_tick_callback_f)
            return instance->_tick_callback_f(clock);
    }

    void Widget::tick_callback_destroy_notify(void*)
    {}
}
