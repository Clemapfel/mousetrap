//
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/adjustment.hpp>
#include <mousetrap/log.hpp>
#include <glm/glm.hpp>

namespace mousetrap
{
    Adjustment::Adjustment(detail::AdjustmentInternal* in)
        : CTOR_SIGNAL(Adjustment, value_changed),
          CTOR_SIGNAL(Adjustment, properties_changed)
    {
        _internal = g_object_ref_sink(in);
    }

    Adjustment::Adjustment(float current, float lower, float upper, float increment)
        : Adjustment(gtk_adjustment_new(current, lower, upper, increment, 0, 0))
    {
        if (upper < lower)
        {
            std::stringstream str;
            str << "In Adjustment::Adjustment: lower " << lower << " larged than upper " << upper;
            log::warning(str.str(), MOUSETRAP_DOMAIN);
        }

        if (increment < 0)
        {
            std::stringstream str;
            str << "In Adjustment:Adjustment: " << increment << " is not a valid increment, increment has to be positive";
            log::warning(str.str(), MOUSETRAP_DOMAIN);
        }
    }

    Adjustment::Adjustment()
        : Adjustment(0, 0, 0, 0)
    {}

    Adjustment::~Adjustment()
    {
        g_object_unref(_internal);
    }

    Adjustment::operator NativeObject() const
    {
        return G_OBJECT(_internal);
    }
    
    NativeObject Adjustment::get_internal() const
    {
        return operator NativeObject();
    }

    float Adjustment::get_lower() const
    {
        return gtk_adjustment_get_lower(_internal);
    }

    float Adjustment::get_increment() const
    {
        return gtk_adjustment_get_minimum_increment(_internal);
    }

    float Adjustment::get_upper() const
    {
        return gtk_adjustment_get_upper(_internal);
    }

    float Adjustment::get_value() const
    {
        return gtk_adjustment_get_value(_internal);
    }

    void Adjustment::set_value(float value)
    {
        gtk_adjustment_set_value(_internal, value);
    }

    void Adjustment::set_lower(float value)
    {
        gtk_adjustment_set_lower(_internal, value);

        auto lower = gtk_adjustment_get_lower(_internal);
        auto upper = gtk_adjustment_get_upper(_internal);

        if (upper < lower)
        {
            std::stringstream str;
            str << "In Adjustment::set_lower: lower " << lower << " larged than upper " << upper;
            log::warning(str.str(), MOUSETRAP_DOMAIN);
        }
    }

    void Adjustment::set_upper(float value)
    {
        gtk_adjustment_set_upper(_internal, value);

        auto lower = gtk_adjustment_get_lower(_internal);
        auto upper = gtk_adjustment_get_upper(_internal);

        if (upper < lower)
        {
            std::stringstream str;
            str << "In Adjustment::set_upper: lower " << lower << " larged than upper " << upper;
            log::warning(str.str(), MOUSETRAP_DOMAIN);
        }
    }

    void Adjustment::set_increment(float value)
    {
        if (value < 0)
        {
            std::stringstream str;
            str << "In Adjustment:set_increment: " << value << " is not a valid increment, increment has to be positive";
            log::warning(str.str(), MOUSETRAP_DOMAIN);
        }

        gtk_adjustment_set_step_increment(_internal, value);
    }
}