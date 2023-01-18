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

            void set_autohide(bool);

            template<typename Function_t, typename Data_t>
            void connect_signal_hide(Function_t, Data_t);
            void set_signal_hide_blocked(bool);

        private:
            static void on_close(GtkInfoBar*, InfoMessage* instance);
            static void on_response(GtkInfoBar*, int response, InfoMessage* instance);

            std::function<void()> _on_hide_f;
            bool _on_hide_blocked = false;

            bool _should_hide = true;

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

#include <src/info_message.inl>
