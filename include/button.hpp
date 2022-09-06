// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Button : public Widget
    {
        public:
            Button();

            void set_label(const std::string&);
            void set_icon(const std::string& path);
            void set_has_frame(bool b);
            void set_child(Widget*);

            operator GtkWidget*() override;

        private:
            GtkButton* _native;
            GtkImage* _icon;
    };
}

#include <src/button.inl>
