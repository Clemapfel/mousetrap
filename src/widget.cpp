//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <iostream>
#include <sstream>
#include <include/widget.hpp>
#include <include/event_controller.hpp>
#include <include/clipboard.hpp>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(WidgetInternal, widget_internal, WIDGET_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(WidgetInternal, widget_internal, WIDGET_INTERNAL)

        static void widget_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_WIDGET_INTERNAL(object);
            G_OBJECT_CLASS(widget_internal_parent_class)->finalize(object);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(WidgetInternal, widget_internal, WIDGET_INTERNAL)

        static WidgetInternal* widget_internal_new()
        {
            auto* self = (WidgetInternal*) g_object_new(widget_internal_get_type(), nullptr);
            widget_internal_init(self);

            self->tooltip_widget = nullptr;
            self->tick_callback = nullptr;
            self->tick_callback_id = false;

            return self;
        }
    }

    Widget::Widget()
        : CTOR_SIGNAL(Widget, realize),
          CTOR_SIGNAL(Widget, unrealize),
          CTOR_SIGNAL(Widget, destroy),
          CTOR_SIGNAL(Widget, hide),
          CTOR_SIGNAL(Widget, show),
          CTOR_SIGNAL(Widget, map),
          CTOR_SIGNAL(Widget, unmap)
    {}

    Widget::~Widget()
    {
        if (_internal != nullptr)
            g_object_unref(_internal);
    }

    Widget::operator GObject*() const
    {
        return G_OBJECT(operator GtkWidget*());
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

    float Widget::get_margin_bottom() const
    {
        return gtk_widget_get_margin_bottom(operator GtkWidget*());
    }

    void Widget::set_margin_top(float value)
    {
        gtk_widget_set_margin_top(operator GtkWidget*(), value);
    }

    float Widget::get_margin_top() const
    {
        return gtk_widget_get_margin_top(operator GtkWidget*());
    }

    void Widget::set_margin_start(float value)
    {
        gtk_widget_set_margin_start(operator GtkWidget*(), value);
    }

    float Widget::get_margin_start() const
    {
        return gtk_widget_get_margin_start(operator GtkWidget*());
    }

    void Widget::set_margin_end(float value)
    {
        gtk_widget_set_margin_end(operator GtkWidget*(), value);
    }

    float Widget::get_margin_end() const
    {
        return gtk_widget_get_margin_end(operator GtkWidget*());
    }

    void Widget::set_expand_horizontally(bool should_expand)
    {
        gtk_widget_set_hexpand(operator GtkWidget*(), should_expand == TRUE);
    }

    bool Widget::get_expand_horizontally() const
    {
        return gtk_widget_get_hexpand(operator GtkWidget*());
    }

    void Widget::set_expand_vertically(bool should_expand)
    {
        gtk_widget_set_vexpand(operator GtkWidget*(), should_expand == TRUE);
    }

    bool Widget::get_expand_vertically() const
    {
        return gtk_widget_get_vexpand(operator GtkWidget*());
    }

    void Widget::set_expand(bool both)
    {
        gtk_widget_set_hexpand(operator GtkWidget*(), both == TRUE);
        gtk_widget_set_vexpand(operator GtkWidget*(), both == TRUE);
    }

    void Widget::set_horizontal_alignment(Alignment alignment)
    {
        gtk_widget_set_halign(operator GtkWidget*(), (GtkAlign) alignment);
    }

    Alignment Widget::get_horizontal_alignment() const
    {
        return (Alignment) gtk_widget_get_halign(operator GtkWidget*());
    }

    void Widget::set_vertical_alignment(Alignment alignment)
    {
        gtk_widget_set_valign(operator GtkWidget*(), (GtkAlign) alignment);
    }

    Alignment Widget::get_vertical_alignment() const
    {
        return (Alignment) gtk_widget_get_valign(operator GtkWidget*());
    }

    void Widget::set_alignment(Alignment both)
    {
        gtk_widget_set_halign(operator GtkWidget*(), (GtkAlign) both);
        gtk_widget_set_valign(operator GtkWidget*(), (GtkAlign) both);
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

    bool Widget::operator==(const Widget& other) const
    {
        return this->operator GtkWidget*() == other.operator GtkWidget*();
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

    void Widget::set_cursor(CursorType cursor)
    {
        switch (cursor)
        {
            case CursorType::NONE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "none");

            case CursorType::DEFAULT:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "default");

            case CursorType::HELP:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "help");

            case CursorType::POINTER:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "pointer");

            case CursorType::CONTEXT_MENU:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "context-menu");

            case CursorType::PROGRESS:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "progress");

            case CursorType::WAIT:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "wait");

            case CursorType::CELL:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "cell");

            case CursorType::CROSSHAIR:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "crosshair");

            case CursorType::TEXT:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "text");

            case CursorType::MOVE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "move");

            case CursorType::NOT_ALLOWED:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "not-allowed");

            case CursorType::GRAB:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "grab");

            case CursorType::GRABBING:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "grabbing");

            case CursorType::ALL_SCROLL:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "all-scroll");

            case CursorType::ZOOM_IN:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "zoom-in");

            case CursorType::ZOOM_OUT:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "zoom-out");

            case CursorType::COLUMN_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "col-resize");

            case CursorType::ROW_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "row-resize");

            case CursorType::NORTH_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "n-resize");

            case CursorType::NORTH_EAST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "ne-resize");

            case CursorType::EAST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "e-resize");

            case CursorType::SOUTH_EAST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "se-resize");

            case CursorType::SOUTH_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "s-resize");

            case CursorType::SOUTH_WEST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "sw-resize");

            case CursorType::WEST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "w-resize");

            case CursorType::NORTH_WEST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator GtkWidget*(), "nw-resize");
        }
    }

    void Widget::add_controller(const EventController& controller)
    {
       gtk_widget_add_controller(operator GtkWidget*(), controller.operator GtkEventController*());
        gtk_widget_set_focusable(operator GtkWidget*(), true);
    }

    void Widget::remove_controller(const EventController& controller)
    {
        gtk_widget_remove_controller(operator GtkWidget*(), controller.operator GtkEventController*());
    }

    void Widget::set_is_focusable(bool b)
    {
        gtk_widget_set_focusable(operator GtkWidget*(), b);
    }

    bool Widget::get_is_focusable() const
    {
        return gtk_widget_get_focusable(operator GtkWidget*());
    }

    void Widget::set_focus_on_click(bool b)
    {
        gtk_widget_set_focus_on_click(operator GtkWidget*(), b);
    }

    bool Widget::get_focus_on_click() const
    {
        return gtk_widget_get_focus_on_click(operator GtkWidget*());
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

    bool Widget::get_can_respond_to_input() const
    {
        return gtk_widget_is_sensitive(operator GtkWidget*());
    }

    void Widget::set_tooltip_widget(Widget* widget)
    {
        initialize();

        _internal->tooltip_widget = widget;
        gtk_widget_set_has_tooltip(operator GtkWidget*(), true);
        g_signal_connect(operator GtkWidget*(), "query-tooltip", G_CALLBACK(on_query_tooltip), _internal);
    }

    gboolean Widget::on_query_tooltip(GtkWidget*, gint x, gint y, gboolean, GtkTooltip* tooltip, detail::WidgetInternal* instance)
    {
        if (instance->tooltip_widget == nullptr)
            return false;

        gtk_tooltip_set_custom(tooltip, instance->tooltip_widget->operator GtkWidget*());
        return true;
    }

    void Widget::initialize()
    {
        if (_internal == nullptr)
        {
            _internal = detail::widget_internal_new();
            g_object_ref(_internal);
            detail::attach_ref_to(G_OBJECT(operator GtkWidget*()), _internal);
        }
    }

    gboolean Widget::tick_callback_wrapper(GtkWidget*, GdkFrameClock* clock, detail::WidgetInternal* instance)
    {
        if (instance->tick_callback != nullptr)
            return (gboolean) instance->tick_callback(clock);
        else
            return true;
    }

    void Widget::remove_tick_callback()
    {
        initialize();

        if (_internal->tick_callback_id != guint(-1))
            gtk_widget_remove_tick_callback(operator GtkWidget*(), _internal->tick_callback_id);
    }

    Clipboard Widget::get_clipboard() const
    {
        return Clipboard(this);
    }

    void Widget::set_hide_on_overflow(bool b)
    {
        gtk_widget_set_overflow(operator GtkWidget*(), b ? GTK_OVERFLOW_HIDDEN : GTK_OVERFLOW_VISIBLE);
    }

    bool Widget::get_hide_on_overflow() const
    {
        auto overflow = gtk_widget_get_overflow(operator GtkWidget*());
        if (overflow == GTK_OVERFLOW_HIDDEN)
            return true;
        else
            return false;
    }
}
