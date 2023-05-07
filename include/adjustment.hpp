// 
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>
#include <include/signal_emitter.hpp>
#include <include/signal_component.hpp>

namespace mousetrap
{
    class Adjustment : public SignalEmitter, public HasValueChangedSignal<Adjustment>
    {
        public:
            Adjustment();
            Adjustment(GtkAdjustment*);
            Adjustment(float current, float lower, float upper, float increment, float page_size = 0, float page_increment = 0);

            operator GtkAdjustment*();
            operator GObject*() override;

            float get_lower() const;
            float get_upper() const;
            float get_value() const;
            float get_step_increment() const;

            float get_minimum_increment() const;

            void set_lower(float);
            void set_upper(float);
            void set_value(float);
            void set_step_increment(float);
            void set_page_increment(float);
            void set_page_size(float);

        private:
            GtkAdjustment* _native;
    };
}
