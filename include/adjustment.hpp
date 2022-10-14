// 
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

namespace mousetrap
{
    class Adjustment : public SignalEmitter, public HasValueChangedSignal<Adjustment>
    {
        public:
            Adjustment();
            Adjustment(GtkAdjustment*);
            Adjustment(float current, float lower, float upper, float increment, float page_size = 0, float page_increment = 0);
            virtual ~Adjustment();

            Adjustment(const Adjustment&);
            Adjustment(Adjustment&&);

            Adjustment& operator=(const Adjustment&);
            Adjustment& operator=(Adjustment&&);

            operator GtkAdjustment*();
            operator GObject*() override;

            void clamp_page(float lower, float upper);
            void configure(float current, float lower, float upper, float increment, float page_size, float page_increment);

            float get_lower() const;
            float get_upper() const;
            float get_value() const;
            float get_step_increment() const;
            float get_page_increment() const;
            float get_page_size() const;

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

#include <src/adjustment.inl>
