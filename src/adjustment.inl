// 
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Adjustment::operator GtkAdjustment*()
    {
        return _native;
    }

    Adjustment::~Adjustment()
    {
        g_object_unref(_native);
    }

    Adjustment::Adjustment(float current, float lower, float upper, float increment, float page_size, float page_increment)
        : HasValueChangedSignal<Adjustment>(this)
    {
        _native = g_object_ref(gtk_adjustment_new(current, lower, upper, increment, page_size, page_increment));
    }

    Adjustment::Adjustment()
            : Adjustment(0, 0, 0, 0, 0, 0)
    {}

    Adjustment::Adjustment(GtkAdjustment* in)
        : HasValueChangedSignal<Adjustment>(this)
    {
        _native = g_object_ref(in);
    }

    Adjustment::operator GObject*()
    {
        return G_OBJECT(_native);
    }

    void Adjustment::clamp_page(float lower, float upper)
    {
        gtk_adjustment_clamp_page(_native, lower, upper);
    }

    void Adjustment::configure(float current, float lower, float upper, float increment, float page_size,
                               float page_increment)
    {
        gtk_adjustment_configure(_native, current, lower, upper, increment, page_size, page_increment);
    }

    float Adjustment::get_lower() const
    {
        return gtk_adjustment_get_lower(_native);
    }

    float Adjustment::get_minimum_increment() const
    {
        return gtk_adjustment_get_minimum_increment(_native);
    }

    float Adjustment::get_page_increment() const
    {
        return gtk_adjustment_get_page_increment(_native);
    }

    float Adjustment::get_page_size() const
    {
        return gtk_adjustment_get_page_size(_native);
    }

    float Adjustment::get_step_increment() const
    {
        return gtk_adjustment_get_step_increment(_native);
    }

    float Adjustment::get_upper() const
    {
        return gtk_adjustment_get_upper(_native);
    }

    float Adjustment::get_value() const
    {
        return gtk_adjustment_get_value(_native);
    }

    void Adjustment::set_lower(float value)
    {
        gtk_adjustment_set_lower(_native, value);
    }

    void Adjustment::set_upper(float value)
    {
        gtk_adjustment_set_upper(_native, value);
    }

    void Adjustment::set_page_increment(float value)
    {
        gtk_adjustment_set_page_increment(_native, value);
    }

    void Adjustment::set_page_size(float value)
    {
        gtk_adjustment_set_page_size(_native, value);
    }

    void Adjustment::set_value(float value)
    {
        gtk_adjustment_set_value(_native, value);
    }
}