// 
// Copyright 2022 Clemens Cords
// Created on 10/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/time.hpp>

namespace mousetrap
{
    enum InfoMessageType
    {
        INFO = GTK_MESSAGE_INFO,
        WARNING = GTK_MESSAGE_WARNING,
        QUESTION = GTK_MESSAGE_QUESTION,
        ERROR = GTK_MESSAGE_ERROR,
        OTHER = GTK_MESSAGE_OTHER
    };

    class InfoMessageBubble : public WidgetImplementation<GtkInfoBar>
    {
        public:
            InfoMessageBubble();

            /// \brief message shown for <hold_duration>, then linear lower opacity from 1 to 0 in <decay_duration>
            ///        if user mouses over bubble, opacity and timer is reset to start
            void set_hide_after(Time hold_duration, Time decay_duration = seconds(0));

            void set_message_type(InfoMessageType);
            void add_child(Widget*);
            void set_has_close_button(bool);
            void set_revealed(bool);
            bool get_revealed();

        private:
            static void on_close(GtkInfoBar*);
            static void on_response(GtkInfoBar*, int response);

            Time _hide_after_elapsed = seconds(0);
            Time _hide_after_hold_duration = seconds(0);
            Time _hide_after_decay_duration = seconds(0);
            bool _hide_after_timer_paused = true;
            int _previous_frame_clock_time_stamp = 0;

            static gboolean on_tick_callback_hide(GtkWidget*, GdkFrameClock* frame_clock, InfoMessageBubble* instance);

            MotionEventController _motion_event_controller;
            static void on_motion_enter(MotionEventController*, double x, double y, InfoMessageBubble* instance);
            static void on_motion_leave(MotionEventController*, InfoMessageBubble* instance);
    };
}

//

namespace mousetrap
{
    InfoMessageBubble::InfoMessageBubble()
        : WidgetImplementation<GtkInfoBar>(GTK_INFO_BAR(gtk_info_bar_new()))
    {
        g_signal_connect(get_native(), "close", G_CALLBACK(on_close), nullptr);
        g_signal_connect(get_native(), "response", G_CALLBACK(on_response), nullptr);

        // makes window react to mouseover
        //gtk_info_bar_set_default_response(get_native(), GTK_RESPONSE_OK);
    }

    void InfoMessageBubble::on_close(GtkInfoBar* bar)
    {
        gtk_widget_hide(GTK_WIDGET(bar));
    }

    void InfoMessageBubble::on_response(GtkInfoBar* bar, int response)
    {
        if (response == GTK_RESPONSE_CLOSE)
            gtk_widget_hide(GTK_WIDGET(bar));
    }

    void InfoMessageBubble::set_message_type(InfoMessageType type)
    {
        gtk_info_bar_set_message_type(get_native(), static_cast<GtkMessageType>(type));
    }

    void InfoMessageBubble::add_child(Widget* widget)
    {
        gtk_info_bar_add_child(get_native(), widget->operator GtkWidget*());
    }

    void InfoMessageBubble::set_has_close_button(bool b)
    {
        gtk_info_bar_set_show_close_button(get_native(), b);
    }

    void InfoMessageBubble::set_revealed(bool b)
    {
        gtk_info_bar_set_revealed(get_native(), b);
    }

    bool InfoMessageBubble::get_revealed()
    {
        return gtk_info_bar_get_revealed(get_native());
    }

    void InfoMessageBubble::set_hide_after(Time hold_duration, Time decay_duration)
    {
        _hide_after_elapsed = microseconds(0);
        _hide_after_hold_duration = hold_duration;
        _hide_after_decay_duration = decay_duration;
        _hide_after_timer_paused = false;
        _previous_frame_clock_time_stamp = 0;

        _motion_event_controller.connect_signal_motion_enter(on_motion_enter, this);
        _motion_event_controller.connect_signal_motion_leave(on_motion_leave, this);
        this->add_controller(&_motion_event_controller);

        gtk_widget_add_tick_callback(
            GTK_WIDGET(get_native()),
            (GtkTickCallback) G_CALLBACK(on_tick_callback_hide),
            this,
            (GDestroyNotify) nullptr
        );
    }

    void InfoMessageBubble::on_motion_enter(MotionEventController*, double x, double y, InfoMessageBubble* instance)
    {
        instance->_hide_after_elapsed = seconds(0);
        instance->set_opacity(1);
        instance->_hide_after_timer_paused = true;
    }

    void InfoMessageBubble::on_motion_leave(MotionEventController*, InfoMessageBubble* instance)
    {
        instance->_hide_after_timer_paused = false;
    }

    gboolean InfoMessageBubble::on_tick_callback_hide(GtkWidget*, GdkFrameClock* frame_clock, InfoMessageBubble* instance)
    {
        if (instance->_hide_after_timer_paused)
            return G_SOURCE_CONTINUE;

        if (instance->_previous_frame_clock_time_stamp == 0)
            instance->_previous_frame_clock_time_stamp = gdk_frame_clock_get_frame_time(frame_clock);

        auto current = gdk_frame_clock_get_frame_time(frame_clock);
        instance->_hide_after_elapsed += microseconds(current - instance->_previous_frame_clock_time_stamp);

        if (instance->_hide_after_elapsed > instance->_hide_after_hold_duration)
        {
            auto opacity = 1 - (instance->_hide_after_elapsed - instance->_hide_after_hold_duration).as_microseconds() / (instance->_hide_after_decay_duration).as_microseconds();
            instance->set_opacity(opacity);

            if (opacity <= 0)
            {
                instance->set_visible(false);
                instance->unparent();
                return G_SOURCE_REMOVE;
            }
        }

        instance->_previous_frame_clock_time_stamp = current;
        return G_SOURCE_CONTINUE;
    }
}