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

namespace mousetrap
{
    template<typename GtkWidget_t>
    struct WidgetImplementation;

    class Widget : public SignalEmitter, public HasRealizeSignal<Widget>
    {
        template<typename T>
        friend struct WidgetImplementation;

        public:
            operator GtkWidget*();
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
            void set_cursor(GtkCursorType type);

            void show();
            void add_controller(EventController*);

            void set_focusable(bool);
            void grab_focus();

            bool get_is_realized();

        protected:
            Widget() = delete;

            template<typename GObject_t>
            void add_reference(GObject_t*);

        private:
            template<typename GtkWidget_t>
            Widget(GtkWidget_t*);

            virtual ~Widget();

            GtkWidget* _native;
            std::vector<GObject*> _refs;
    };

    template<typename GtkWidget_t>
    struct WidgetImplementation : public Widget
    {
        WidgetImplementation(GtkWidget_t*);
        operator GtkWidget_t*() const;
        GtkWidget_t* get_native() const;
    };


};

#include <src/widget.inl>
