// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Label : public Widget
    {
        public:
            Label(const std::string&);
            virtual ~Label();

            void set_text(const std::string&);
            void set_use_markup(bool b);

            GtkWidget* get_native() override;

        private:
            GtkLabel* _native;
    };
}

#include <src/label.inl>