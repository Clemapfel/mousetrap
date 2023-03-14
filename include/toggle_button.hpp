// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class ToggleButton : public WidgetImplementation<GtkToggleButton>, public HasToggledSignal<ToggleButton>
    {
        public:
            ToggleButton();

            bool get_active() const;
            void set_active(bool b);

            void set_action(Action& stateful_action);

            void set_label(const std::string&);
            void set_child(Widget*);
            void set_has_frame(bool b);
    };
}

#include <src/toggle_button.inl>