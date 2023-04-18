//
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#include <include/adjustment.hpp>
#include <include/log.hpp>
#include <glm/glm.hpp>

namespace mousetrap
{
    Adjustment::operator GtkAdjustment*() const
    {
        return _native;
    }

    Adjustment::Adjustment()
        : Adjustment(0, 0, 0, 0)
    {}

    Adjustment::Adjustment(GtkAdjustment* in)
        : CTOR_SIGNAL(Adjustment, value_changed), CTOR_SIGNAL(Adjustment, properties_changed)
    {
        _native = g_object_ref(in);
    }

    Adjustment::Adjustment(float current, float lower, float upper, float increment)
        : Adjustment(gtk_adjustment_new(current, lower, upper, increment, 0, glm::distance(lower, upper)))
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

    Adjustment::~Adjustment()
    {
        if (_native != nullptr)
            g_object_unref(_native);
    }

    Adjustment::Adjustment(Adjustment&& other) noexcept
        : CTOR_SIGNAL(Adjustment, value_changed), CTOR_SIGNAL(Adjustment, properties_changed)
    {
        _native = other._native;
        other._native = nullptr;
    }

    Adjustment::operator GObject*() const
    {
        return G_OBJECT(_native);
    }

    /*
    Adjustment::Adjustment(const Adjustment& other)
        : CTOR_SIGNAL(Adjustment, value_changed), CTOR_SIGNAL(Adjustment, properties_changed)
    {
        _native = g_object_ref(gtk_adjustment_new(
            other.get_value(),
            other.get_lower(),
            other.get_upper(),
            other.get_increment(),
            glm::distance(other.get_upper(), other.get_lower()),
            0
        ));
    }
     */

    Adjustment& Adjustment::operator=(Adjustment&& other) noexcept
    {
        _native = other._native;
        other._native = nullptr;
        return *this;
    }

    /*
    Adjustment& Adjustment::operator=(const Adjustment& other)
    {
        _native = g_object_ref(gtk_adjustment_new(
        other.get_value(),
        other.get_lower(),
        other.get_upper(),
        other.get_increment(),
        glm::distance(other.get_upper(), other.get_lower()),
        0
        ));
        return *this;
    }
     */

    float Adjustment::get_lower() const
    {
        return gtk_adjustment_get_lower(_native);
    }

    float Adjustment::get_increment() const
    {
        return gtk_adjustment_get_minimum_increment(_native);
    }

    float Adjustment::get_upper() const
    {
        return gtk_adjustment_get_upper(_native);
    }

    float Adjustment::get_value() const
    {
        return gtk_adjustment_get_value(_native);
    }

    void Adjustment::set_value(float value)
    {
        gtk_adjustment_set_value(_native, value);
    }

    void Adjustment::set_lower(float value)
    {
        gtk_adjustment_set_lower(_native, value);

        auto lower = gtk_adjustment_get_lower(_native);
        auto upper = gtk_adjustment_get_upper(_native);

        if (upper < lower)
        {
            std::stringstream str;
            str << "In Adjustment::set_lower: lower " << lower << " larged than upper " << upper;
            log::warning(str.str(), MOUSETRAP_DOMAIN);
        }
    }

    void Adjustment::set_upper(float value)
    {
        gtk_adjustment_set_upper(_native, value);

        auto lower = gtk_adjustment_get_lower(_native);
        auto upper = gtk_adjustment_get_upper(_native);

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

        gtk_adjustment_set_step_increment(_native, value);
    }
}