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
            virtual ~Button();

            void set_label(const std::string&);
            void set_icon(const std::string& path);
            void set_relief_active(bool b);

            GtkWidget* get_native();

        protected:
            void on_clicked(GtkButton* self, gpointer user_data);

        private:
            static void on_clicked_wrapper(GtkButton* self, void* instance);
            GtkButton* _native;
            GtkImage* _icon;
    };
}

#include <src/button.inl>
