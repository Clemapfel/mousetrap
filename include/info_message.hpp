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
        ERROR = GTK_MESSAGE_ERROR,
        OTHER = GTK_MESSAGE_OTHER
    };

    class InfoMessage : public WidgetImplementation<GtkInfoBar>
    {
        public:
            InfoMessage();

            /// \brief message shown for <hold_duration>, then linear lower opacity from 1 to 0 in <decay_duration>
            ///        if user mouses over bubble, opacity and timer is reset to start
            void set_hide_after(Time hold_duration, Time decay_duration = seconds(0));

            /// \brief if true, mousing over message will reset it's hide time
            void set_hide_interruptable(bool);

            void set_message_type(InfoMessageType);
            InfoMessageType get_message_type();
            void add_child(Widget*);
            void set_has_close_button(bool);
            void set_revealed(bool);
            bool get_revealed();

            template<typename Function_t, typename Data_t>
            void connect_signal_hide(Function_t, Data_t);

            void set_signal_hide_blocked(bool);

        private:
            static void on_close(GtkInfoBar*, InfoMessage* instance);
            static void on_response(GtkInfoBar*, int response, InfoMessage* instance);

            std::function<void()> _on_hide_f;
            bool _on_hide_blocked;

            Time _hide_after_elapsed = seconds(0);
            Time _hide_after_hold_duration = seconds(0);
            Time _hide_after_decay_duration = seconds(0);
            bool _hide_after_timer_paused = true;
            int64_t _previous_frame_clock_time_stamp = 0;

            static gboolean on_tick_callback_hide(GtkWidget*, GdkFrameClock* frame_clock, InfoMessage* instance);

            bool _hide_interruptable = true;
            MotionEventController _motion_event_controller;
            static void on_motion_enter(MotionEventController*, double x, double y, InfoMessage* instance);
            static void on_motion_leave(MotionEventController*, InfoMessage* instance);
    };
}

//

namespace mousetrap
{
    InfoMessage::InfoMessage()
        : WidgetImplementation<GtkInfoBar>(GTK_INFO_BAR(gtk_info_bar_new()))
    {
        g_signal_connect(get_native(), "close", G_CALLBACK(on_close), this);
        g_signal_connect(get_native(), "response", G_CALLBACK(on_response), this);

        // makes window react to mouseover
        //gtk_info_bar_set_default_response(get_native(), GTK_RESPONSE_OK);
    }

    void InfoMessage::set_signal_hide_blocked(bool b)
    {
        _on_hide_blocked= b;
    }

    template<typename Function_t, typename Data_t>
    void InfoMessage::connect_signal_hide(Function_t f_in, Data_t data_in)
    {
        _on_hide_f = [f = f_in, data = data_in, instance = this](){

            if (instance->_on_hide_blocked)
                return;

            f(instance, data);
        };
    }

    void InfoMessage::on_close(GtkInfoBar* bar, InfoMessage* instance)
    {
        gtk_widget_hide(GTK_WIDGET(bar));
        instance->_on_hide_f();
    }

    void InfoMessage::on_response(GtkInfoBar* bar, int response, InfoMessage* instance)
    {
        if (response == GTK_RESPONSE_CLOSE)
            on_close(instance->operator _GtkInfoBar*(), instance);
    }

    void InfoMessage::set_message_type(InfoMessageType type)
    {
        gtk_info_bar_set_message_type(get_native(), static_cast<GtkMessageType>(type));
    }

    InfoMessageType InfoMessage::get_message_type()
    {
        (InfoMessageType) gtk_info_bar_get_message_type(get_native());
    }

    void InfoMessage::add_child(Widget* widget)
    {
        gtk_info_bar_add_child(get_native(), widget->operator GtkWidget*());
    }

    void InfoMessage::set_has_close_button(bool b)
    {
        gtk_info_bar_set_show_close_button(get_native(), b);
    }

    void InfoMessage::set_revealed(bool b)
    {
        gtk_info_bar_set_revealed(get_native(), b);
    }

    bool InfoMessage::get_revealed()
    {
        return gtk_info_bar_get_revealed(get_native());
    }

    void InfoMessage::set_hide_after(Time hold_duration, Time decay_duration)
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

    void InfoMessage::set_hide_interruptable(bool b)
    {
        _hide_interruptable = b;
    }

    void InfoMessage::on_motion_enter(MotionEventController*, double x, double y, InfoMessage* instance)
    {
        if (not instance->_hide_interruptable)
            return;

        instance->_hide_after_elapsed = seconds(0);
        instance->set_opacity(1);
        instance->_hide_after_timer_paused = true;
    }

    void InfoMessage::on_motion_leave(MotionEventController*, InfoMessage* instance)
    {

        instance->_hide_after_timer_paused = false;
    }

    gboolean InfoMessage::on_tick_callback_hide(GtkWidget*, GdkFrameClock* frame_clock, InfoMessage* instance)
    {
        if (instance->_hide_after_timer_paused)
            return G_SOURCE_CONTINUE;

        auto current = gdk_frame_clock_get_frame_time(frame_clock);

        if (instance->_previous_frame_clock_time_stamp == 0)
            instance->_previous_frame_clock_time_stamp = current;

        instance->_hide_after_elapsed += microseconds(current - instance->_previous_frame_clock_time_stamp);

        if (instance->_hide_after_elapsed > instance->_hide_after_hold_duration)
        {
            auto opacity = 1 - (instance->_hide_after_elapsed - instance->_hide_after_hold_duration).as_microseconds() / (instance->_hide_after_decay_duration).as_microseconds();
            instance->set_opacity(opacity);

            if (opacity <= 0)
            {
                instance->set_visible(false);
                instance->_on_hide_f();
                return G_SOURCE_REMOVE;
            }
        }

        instance->_previous_frame_clock_time_stamp = current;
        return G_SOURCE_CONTINUE;
    }
}