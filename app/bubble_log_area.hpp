// 
// Copyright 2022 Clemens Cords
// Created on 10/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class BubbleLogArea : public AppComponent
    {
        public:
            BubbleLogArea();
            operator Widget*() override;
            void update() override {};

            void send_message(const std::string&, InfoMessageType type = InfoMessageType::INFO);
            void set_has_close_button(bool);
            void set_bubble_reveal_transition_type(TransitionType);

        private:
            struct Message
            {
                Revealer revealer;
                InfoMessage message;

                Time elapsed = seconds(0);
                int previous_timestamp = 0;
            };

            bool _has_close_button = true;
            TransitionType _transition_type = TransitionType::SLIDE_RIGHT_TO_LEFT;

            static inline bool _suppress_info = false;
            static inline bool _suppress_warning = false;
            static inline bool _suppress_error = false;

            static inline Time _message_reveal_delay = seconds(0);
            static inline Time _message_hold_duration = seconds(0);
            static inline Time _message_decay_duration = seconds(0);

            Adjustment _scrolled_window_vadjustment;
            ScrolledWindow _scrolled_window;
            Box _box = Box(GTK_ORIENTATION_VERTICAL, state::margin_unit);
            SeparatorLine _spacer;

            static inline size_t _current_id = 0;
            std::unordered_map<size_t, Message*> _messages;

            using on_message_hide_data = struct{BubbleLogArea* instance; size_t id;};
            static void on_message_hide(InfoMessage*, on_message_hide_data);
            static gboolean delayed_reveal(Widget*, GdkFrameClock*, Message*);
    };
}

//

namespace mousetrap
{
    void BubbleLogArea::set_has_close_button(bool b)
    {
        _has_close_button = b;
    }

    void BubbleLogArea::set_bubble_reveal_transition_type(TransitionType type)
    {
        _transition_type = type;
    }

    BubbleLogArea::BubbleLogArea()
    {
        _scrolled_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_EXTERNAL);
        _scrolled_window.set_child(&_box);
        _scrolled_window.set_expand(true);
        _scrolled_window.set_vadjustment(_scrolled_window_vadjustment);

        _spacer.set_expand(true);
        _spacer.set_opacity(0);
        _spacer.set_valign(GTK_ALIGN_START);

        _box.push_back(&_spacer);
        _box.set_expand(true);
        _box.set_valign(GTK_ALIGN_END);
        _box.set_vexpand(false);

        _suppress_info = state::settings_file->get_value_as<bool>("bubble_log_area", "suppress_message_type_info");
        _suppress_warning = state::settings_file->get_value_as<bool>("bubble_log_area", "suppress_message_type_warning");
        _suppress_error = state::settings_file->get_value_as<bool>("bubble_log_area", "suppress_message_type_error");

        _message_reveal_delay = seconds(state::settings_file->get_value_as<float>("bubble_log_area", "message_reveal_delay"));
        _message_hold_duration = seconds(state::settings_file->get_value_as<float>("bubble_log_area", "message_hold_duration"));
        _message_decay_duration = seconds(state::settings_file->get_value_as<float>("bubble_log_area", "message_decay_duration"));
    }

    BubbleLogArea::operator Widget*()
    {
        return &_scrolled_window;
    }

    gboolean BubbleLogArea::delayed_reveal(Widget*, GdkFrameClock* clock, Message* message)
    {
        auto current = gdk_frame_clock_get_frame_time(clock);

        if ( message->previous_timestamp == 0)
            message->previous_timestamp = current;

        message->elapsed += microseconds(current - message->previous_timestamp);

        if (message->elapsed > _message_reveal_delay)
        {
            message->revealer.set_revealed(true);

            auto hold_duration = _message_hold_duration;
            if (message->message.get_message_type() == InfoMessageType::ERROR)
                _message_hold_duration = microseconds(_message_hold_duration.as_microseconds() * 3);

            message->message.set_hide_after(
                hold_duration,
                _message_decay_duration
            );

            return G_SOURCE_REMOVE;
        }

        message->previous_timestamp = current;
        return G_SOURCE_CONTINUE;
    }

    void BubbleLogArea::on_message_hide(InfoMessage*, on_message_hide_data data)
    {
        auto* message = data.instance->_messages.at(data.id);
        message->revealer.set_revealed(false);
        message->revealer.unparent();

        delete message;
        data.instance->_messages.erase(data.id);
    }

    void BubbleLogArea::send_message(const std::string& text, InfoMessageType type)
    {
        if (type == INFO and _suppress_info)
            return;

        if (type == WARNING and _suppress_warning)
            return;

        if (type == ERROR and _suppress_error)
            return;

        auto id = _current_id++;
        auto& message = _messages.insert({id, new Message{
            Revealer(_transition_type),
            InfoMessage()
        }}).first->second;

        message->revealer.set_child(&message->message);
        message->revealer.set_revealed(false);
        message->message.add_child(new Label(text));
        message->message.set_message_type(type);
        message->message.set_has_close_button(_has_close_button);
        message->message.set_valign(GTK_ALIGN_END);
        message->message.set_vexpand(false);
        message->message.set_halign(GTK_ALIGN_END);
        message->message.set_hexpand(false);
        message->message.connect_signal_hide(on_message_hide, on_message_hide_data{this, id});
        message->message.set_hide_interruptable(false);

        auto* w = message->revealer.operator GtkWidget*();
        gtk_widget_add_tick_callback(w, (GtkTickCallback) G_CALLBACK(delayed_reveal), message, (GDestroyNotify) nullptr);

        auto* box = new Box(GTK_ORIENTATION_HORIZONTAL);
        auto* spacer = new SeparatorLine();
        spacer->set_halign(GTK_ALIGN_START);
        spacer->set_hexpand(true);

        box->push_back(spacer);
        box->push_back(&message->revealer);
        _box.push_back(box);

        _scrolled_window_vadjustment.set_value(_scrolled_window_vadjustment.get_upper());
    }
}