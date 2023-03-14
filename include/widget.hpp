// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk-4.0/gdk/gdk.h>
#include <include/vector.hpp>
#include <include/gtk_common.hpp>
#include <include/signal_emitter.hpp>
#include <include/event_controller.hpp>
#include <include/signal_component.hpp>

#include <map>
#include <string>
#include <include/frame_clock.hpp>

namespace mousetrap
{
    template<typename GtkWidget_t>
    struct WidgetImplementation;

    class Widget : public SignalEmitter,
        public HasRealizeSignal<Widget>,
        public HasMapSignal<Widget>,
        public HasShowSignal<Widget>
    {
        template<typename T>
        friend struct WidgetImplementation;

        public:
            virtual operator GtkWidget*() const; // can be overridden if template argument is not parent widget type of compound widget
            operator GObject*() override;

            Vector2f get_size_request();
            void set_size_request(Vector2f);
            Vector2f get_size();

            bool operator==(const Widget& other) const;
            bool operator!=(const Widget& other) const;

            void set_margin_top(float);
            void set_margin_bottom(float);
            void set_margin_start(float);
            void set_margin_end(float);
            void set_margin(float);
            void set_margin_horizontal(float);
            void set_margin_vertical(float);

            void set_hexpand(bool should_expand);
            void set_vexpand(bool should_expand);
            void set_expand(bool both);

            void set_halign(GtkAlign);
            void set_valign(GtkAlign);
            void set_align(GtkAlign both);

            void set_opacity(float);
            float get_opacity();

            void set_visible(bool);
            bool get_visible();

            void set_tooltip_text(const std::string&);
            void set_tooltip_widget(Widget*);

            // for pictures, c.f. https://docs.gtk.org/gdk4/ctor.Cursor.new_from_name.html
            void set_cursor(GtkCursorType type);

            void hide();
            void show();
            void add_controller(EventController*);

            void set_focusable(bool);
            void grab_focus();
            void set_focus_on_click(bool);
            bool get_has_focus();

            bool get_is_realized();

            using preferred_size = struct {Vector2f minimum_size; Vector2f natural_size;};
            preferred_size get_preferred_size();

            void unparent();
            void set_can_respond_to_input(bool);
            bool get_can_respond_to_input() const;

            void beep();

            /// \brief f(FrameClock, Arg_t data) -> Bool
            /// \returns true if tick should continue, false if it should break
            template<typename Function_t, typename Arg_t>
            void add_tick_callback(Function_t, Arg_t);

        protected:
            Widget() = delete;

            template<typename GObject_t>
            void add_reference(GObject_t*);

            void override_native(GtkWidget*);

        private:
            template<typename GtkWidget_t>
            Widget(GtkWidget_t*);

            virtual ~Widget();

            GtkWidget* _native;
            std::vector<GObject*> _refs;

            std::function<bool(GdkFrameClock*)> _tick_callback_f;
            std::function<void(void*)> _destroy_notify_f;

            static gboolean tick_callback_wrapper(GtkWidget*, GdkFrameClock*, Widget* instance);
            static void tick_callback_destroy_notify(void*);

            Widget* _tooltip_widget = nullptr;
            static gboolean on_query_tooltip(GtkWidget*, gint x, gint y, gboolean, GtkTooltip* tooltip, Widget* instance);
    };

    template<typename GtkWidget_t>
    struct WidgetImplementation : public Widget
    {
        WidgetImplementation(GtkWidget_t*);
        operator GtkWidget_t*() const;
        GtkWidget_t* get_native() const;
    };
};

namespace mousetrap
{
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
}


