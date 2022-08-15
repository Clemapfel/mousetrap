// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk-4.0/gtk/gtk.h>

#include <include/vector.hpp>

#include <map>
#include <string>

namespace mousetrap
{
    class Widget
    {
        public:
            virtual GtkWidget* get_native() = 0;
            virtual void update(){};

            operator GtkWidget*();

            Vector2f get_size_request();
            void set_size_request(Vector2f);
            Vector2f get_size();

            void set_signal_blocked(const std::string& signal_id, bool);
            void set_all_signals_blocked(bool);

            template<typename Function_t>
            void connect_signal(const std::string& signal_id, Function_t*, void* data = nullptr);

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

            void set_tooltip_text(const std::string&);

            template<typename... GdkEventMask_t> requires (std::is_same_v<GdkEventMask_t, GdkEventMask> && ...)
            void add_events(GdkEventMask_t...);

        private:
            /// \returns true if get_native() == nullptr
            bool warn_if_nullptr(const std::string& function_id);

            std::map<std::string, size_t> _signal_handlers;
    };

    template<typename T>
    struct WidgetWrapper : public Widget
    {
        WidgetWrapper(T* in)
            : _native(in)
        {}

        GtkWidget* get_native() override {
            return GTK_WIDGET(_native);
        };

        T* _native;
    };
};

#include <src/widget.inl>
