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

#include <map>
#include <string>

namespace mousetrap
{
    class Widget : public SignalEmitter
    {
        public:
            virtual void update(){};

            virtual operator GtkWidget*() = 0;
            operator GObject*() override;

            Vector2f get_size_request();
            void set_size_request(Vector2f);
            Vector2f get_size();

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

            void set_tooltip_text(const std::string&);
            void set_cursor(GtkCursorType type);

            void show();
            void add_controller(EventController*);

            void set_focusable(bool);
            void grab_focus();
    };

    template<typename T>
    struct WidgetWrapper : public Widget
    {
        WidgetWrapper(T* in)
            : _native(in)
        {}

        operator GtkWidget*() override {
            return GTK_WIDGET(_native);
        };

        operator T*() {
            return _native;
        }

        T* _native;
    };
};

#include <src/widget.inl>
