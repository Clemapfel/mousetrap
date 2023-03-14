// 
// Copyright 2022 Clemens Cords
// Created on 10/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/project_state.hpp>

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
                Label label;

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

    namespace state
    {
        inline BubbleLogArea* bubble_log;
    }
}
