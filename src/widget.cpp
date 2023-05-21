//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <iostream>
#include <sstream>
#include <mousetrap/widget.hpp>
#include <mousetrap/event_controller.hpp>
#include <mousetrap/clipboard.hpp>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(WidgetInternal, widget_internal, WIDGET_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(WidgetInternal, widget_internal, WIDGET_INTERNAL)

        static void widget_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_WIDGET_INTERNAL(object);
            
            if (self->tooltip_widget != nullptr)
                g_object_unref(self->tooltip_widget);

            G_OBJECT_CLASS(widget_internal_parent_class)->finalize(object);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(WidgetInternal, widget_internal, WIDGET_INTERNAL)

        static WidgetInternal* widget_internal_new(GtkWidget* native)
        {
            auto* self = (WidgetInternal*) g_object_new(widget_internal_get_type(), nullptr);
            widget_internal_init(self);

            self->native = native;
            self->tooltip_widget = nullptr;
            self->tick_callback = nullptr;
            self->tick_callback_id = guint(-1);

            return self;
        }
    }

    Widget::Widget(NativeWidget widget)
        : _internal(detail::widget_internal_new(widget)),
          CTOR_SIGNAL(Widget, realize),
          CTOR_SIGNAL(Widget, unrealize),
          CTOR_SIGNAL(Widget, destroy),
          CTOR_SIGNAL(Widget, hide),
          CTOR_SIGNAL(Widget, show),
          CTOR_SIGNAL(Widget, map),
          CTOR_SIGNAL(Widget, unmap)
    {
        detail::attach_ref_to(G_OBJECT(_internal->native), _internal);
    }

    Widget::~Widget()
    {
        g_object_unref(_internal);
    }

    Widget::operator NativeObject() const
    {
        return G_OBJECT(operator NativeWidget());
    }

    Widget::operator NativeWidget() const
    {
        return _internal->native;
    }
    
    NativeObject Widget::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void Widget::activate()
    {
        gtk_widget_activate(operator NativeWidget());
    }

    Vector2f Widget::get_size_request()
    {
        int w, h;
        gtk_widget_get_size_request(operator NativeWidget(), &w, &h);
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
        gtk_widget_set_margin_bottom(operator NativeWidget(), value);
    }

    float Widget::get_margin_bottom() const
    {
        return gtk_widget_get_margin_bottom(operator NativeWidget());
    }

    void Widget::set_margin_top(float value)
    {
        gtk_widget_set_margin_top(operator NativeWidget(), value);
    }

    float Widget::get_margin_top() const
    {
        return gtk_widget_get_margin_top(operator NativeWidget());
    }

    void Widget::set_margin_start(float value)
    {
        gtk_widget_set_margin_start(operator NativeWidget(), value);
    }

    float Widget::get_margin_start() const
    {
        return gtk_widget_get_margin_start(operator NativeWidget());
    }

    void Widget::set_margin_end(float value)
    {
        gtk_widget_set_margin_end(operator NativeWidget(), value);
    }

    float Widget::get_margin_end() const
    {
        return gtk_widget_get_margin_end(operator NativeWidget());
    }

    void Widget::set_expand_horizontally(bool should_expand)
    {
        gtk_widget_set_hexpand(operator NativeWidget(), should_expand == TRUE);
    }

    bool Widget::get_expand_horizontally() const
    {
        return gtk_widget_get_hexpand(operator NativeWidget());
    }

    void Widget::set_expand_vertically(bool should_expand)
    {
        gtk_widget_set_vexpand(operator NativeWidget(), should_expand == TRUE);
    }

    bool Widget::get_expand_vertically() const
    {
        return gtk_widget_get_vexpand(operator NativeWidget());
    }

    void Widget::set_expand(bool both)
    {
        gtk_widget_set_hexpand(operator NativeWidget(), both == TRUE);
        gtk_widget_set_vexpand(operator NativeWidget(), both == TRUE);
    }

    void Widget::set_horizontal_alignment(Alignment alignment)
    {
        gtk_widget_set_halign(operator NativeWidget(), (GtkAlign) alignment);
    }

    Alignment Widget::get_horizontal_alignment() const
    {
        return (Alignment) gtk_widget_get_halign(operator NativeWidget());
    }

    void Widget::set_vertical_alignment(Alignment alignment)
    {
        gtk_widget_set_valign(operator NativeWidget(), (GtkAlign) alignment);
    }

    Alignment Widget::get_vertical_alignment() const
    {
        return (Alignment) gtk_widget_get_valign(operator NativeWidget());
    }

    void Widget::set_alignment(Alignment both)
    {
        gtk_widget_set_halign(operator NativeWidget(), (GtkAlign) both);
        gtk_widget_set_valign(operator NativeWidget(), (GtkAlign) both);
    }

    void Widget::set_size_request(Vector2f size)
    {
        gtk_widget_set_size_request(operator NativeWidget(), size.x, size.y);
    }

    void Widget::set_opacity(float value)
    {
        gtk_widget_set_opacity(operator NativeWidget(), value);
    }

    float Widget::get_opacity()
    {
        return gtk_widget_get_opacity(operator NativeWidget());
    }

    bool Widget::operator==(const Widget& other) const
    {
        return this->operator NativeWidget() == other.operator NativeWidget();
    }

    bool Widget::operator!=(const Widget& other) const
    {
        return not (*this == other);
    }

    void Widget::set_tooltip_text(const std::string& text)
    {
        gtk_widget_set_tooltip_markup(operator NativeWidget(), text.c_str());
    }

    void Widget::set_visible(bool b)
    {
        gtk_widget_set_visible(operator NativeWidget(), b);
    }

    bool Widget::get_visible()
    {
        return gtk_widget_get_visible(operator NativeWidget());
    }

    void Widget::show()
    {
        gtk_widget_show(operator NativeWidget());
    }

    void Widget::hide()
    {
        gtk_widget_hide(operator NativeWidget());
    }

    void Widget::set_cursor(CursorType cursor)
    {
        switch (cursor)
        {
            case CursorType::NONE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "none");

            case CursorType::DEFAULT:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "default");

            case CursorType::HELP:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "help");

            case CursorType::POINTER:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "pointer");

            case CursorType::CONTEXT_MENU:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "context-menu");

            case CursorType::PROGRESS:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "progress");

            case CursorType::WAIT:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "wait");

            case CursorType::CELL:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "cell");

            case CursorType::CROSSHAIR:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "crosshair");

            case CursorType::TEXT:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "text");

            case CursorType::MOVE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "move");

            case CursorType::NOT_ALLOWED:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "not-allowed");

            case CursorType::GRAB:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "grab");

            case CursorType::GRABBING:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "grabbing");

            case CursorType::ALL_SCROLL:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "all-scroll");

            case CursorType::ZOOM_IN:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "zoom-in");

            case CursorType::ZOOM_OUT:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "zoom-out");

            case CursorType::COLUMN_RESIZE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "col-resize");

            case CursorType::ROW_RESIZE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "row-resize");

            case CursorType::NORTH_RESIZE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "n-resize");

            case CursorType::NORTH_EAST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "ne-resize");

            case CursorType::EAST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "e-resize");

            case CursorType::SOUTH_EAST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "se-resize");

            case CursorType::SOUTH_RESIZE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "s-resize");

            case CursorType::SOUTH_WEST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "sw-resize");

            case CursorType::WEST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "w-resize");

            case CursorType::NORTH_WEST_RESIZE:
                return gtk_widget_set_cursor_from_name(operator NativeWidget(), "nw-resize");
        }
    }

    void Widget::set_cursor_from_image(const Image& image, Vector2i offset)
    {
        auto* texture = gdk_texture_new_for_pixbuf(image.operator GdkPixbuf *());
        auto* cursor = gdk_cursor_new_from_texture(texture, offset.x, offset.y, nullptr);
        gtk_widget_set_cursor(operator NativeWidget(), cursor);

        g_object_unref(texture);
        g_object_unref(cursor);
    }

    void Widget::add_controller(const EventController& controller)
    {
        gtk_widget_add_controller(operator NativeWidget(), controller.operator GtkEventController*());
        gtk_widget_set_focusable(operator NativeWidget(), true);
    }

    void Widget::remove_controller(const EventController& controller)
    {
        gtk_widget_remove_controller(operator NativeWidget(), controller.operator GtkEventController*());
    }

    void Widget::set_is_focusable(bool b)
    {
        gtk_widget_set_focusable(operator NativeWidget(), b);
    }

    bool Widget::get_is_focusable() const
    {
        return gtk_widget_get_focusable(operator NativeWidget());
    }

    void Widget::set_focus_on_click(bool b)
    {
        gtk_widget_set_focus_on_click(operator NativeWidget(), b);
    }

    bool Widget::get_focus_on_click() const
    {
        return gtk_widget_get_focus_on_click(operator NativeWidget());
    }

    bool Widget::get_has_focus()
    {
        return gtk_widget_has_focus(operator NativeWidget());
    }

    void Widget::grab_focus()
    {
        gtk_widget_grab_focus(operator NativeWidget());
    }

    bool Widget::get_is_realized()
    {
        return gtk_widget_get_realized(operator NativeWidget());
    }

    Widget::preferred_size Widget::get_preferred_size() const
    {
        auto min = gtk_requisition_new();
        auto nat = gtk_requisition_new();
        gtk_widget_get_preferred_size(operator NativeWidget(), min, nat);

        return Widget::preferred_size{{min->width, min->height}, {nat->width, nat->height}};
    }

    Rectangle Widget::get_allocation() const
    {
        GtkAllocation* allocation = new GtkAllocation();
        gtk_widget_get_allocation(operator NativeWidget(), allocation);

        auto out = Rectangle{
            {allocation->x, allocation->y},
            {allocation->width, allocation->height}
        };

        delete allocation;
        return out;
    }

    void Widget::unparent()
    {
        if (gtk_widget_get_parent(operator NativeWidget()) != nullptr)
            gtk_widget_unparent(operator NativeWidget());
    }

    void Widget::set_can_respond_to_input(bool b)
    {
        gtk_widget_set_sensitive(operator NativeWidget(), b);
    }

    bool Widget::get_can_respond_to_input() const
    {
        return gtk_widget_is_sensitive(operator NativeWidget());
    }

    void Widget::set_tooltip_widget(const Widget& widget)
    {
        _internal->tooltip_widget = widget.operator NativeWidget();
        g_object_ref(_internal->tooltip_widget);
        gtk_widget_set_has_tooltip(operator NativeWidget(), true);
        connect_signal("query-tooltip", on_query_tooltip, _internal);
    }

    void Widget::remove_tooltip_widget()
    {
        if (_internal->tooltip_widget != nullptr)
            g_object_unref(_internal->tooltip_widget);

        _internal->tooltip_widget = nullptr;
        gtk_widget_set_has_tooltip(operator NativeWidget(), false);
    }

    gboolean Widget::on_query_tooltip(GtkWidget*, gint x, gint y, gboolean, GtkTooltip* tooltip, detail::WidgetInternal* instance)
    {
        if (instance->tooltip_widget == nullptr)
            return false;

        gtk_tooltip_set_custom(tooltip, instance->tooltip_widget);
        return true;
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
        if (_internal->tick_callback_id != guint(-1))
            gtk_widget_remove_tick_callback(operator NativeWidget(), _internal->tick_callback_id);
    }

    Clipboard Widget::get_clipboard() const
    {
        return Clipboard(this);
    }

    void Widget::set_hide_on_overflow(bool b)
    {
        gtk_widget_set_overflow(operator NativeWidget(), b ? GTK_OVERFLOW_HIDDEN : GTK_OVERFLOW_VISIBLE);
    }

    bool Widget::get_hide_on_overflow() const
    {
        auto overflow = gtk_widget_get_overflow(operator NativeWidget());
        if (overflow == GTK_OVERFLOW_HIDDEN)
            return true;
        else
            return false;
    }
}
