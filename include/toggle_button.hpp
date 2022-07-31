// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class ToggleButton : public Widget
    {
        public:
            ToggleButton();
            ~ToggleButton();

            bool get_active() const;
            void set_active(bool b);

            GtkWidget* get_native() override;

        protected:
            void on_toggled(GtkToggleButton* self, gpointer userdata);

        private:
            static void on_toggled_wrapper(GtkToggleButton* self, void* instance);
            GtkToggleButton* _native;
    };
}

#include <src/toggle_button.inl>