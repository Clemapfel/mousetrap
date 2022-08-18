// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/range.hpp>

namespace mousetrap
{
    class SpinButton : public Widget
    {
        public:
            SpinButton(float min, float max, float step);
            virtual ~SpinButton();

           void set_value(float);
           float get_value();

           void set_digits(size_t);
           void set_width_chars(size_t);

           void set_wrap(bool);
           GtkWidget* get_native() override;

        protected:
            /// \returns TRUE if conversion worked, FALSE for not handled GTK_INPUT_ERROR for failed
            virtual gint on_input(GtkSpinButton* self, gdouble* new_value, gpointer user_data);

            /// \returns true if value has been displayed
            virtual gboolean on_output(GtkSpinButton* self, gpointer user_data);

            virtual void on_value_changed(GtkSpinButton* self, gpointer user_data);

        private:
            static gint on_input_wrapper(GtkSpinButton* self, gdouble* new_value, void* instance);
            static gboolean on_output_wrapper(GtkSpinButton* self, void* instance);
            static void on_value_changed_wrapper(GtkSpinButton* self, void* instance);

            GtkSpinButton* _native;
    };
}

#include <src/spin_button.inl>
