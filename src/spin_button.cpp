//
// Created by clem on 3/22/23.
//

#include <mousetrap/spin_button.hpp>
#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(SpinButtonInternal, spin_button_internal, SPIN_BUTTON_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(SpinButtonInternal, spin_button_internal, SPIN_BUTTON_INTERNAL)

        static void spin_button_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_SPIN_BUTTON_INTERNAL(object);
            G_OBJECT_CLASS(spin_button_internal_parent_class)->finalize(object);
            delete self->adjustment;
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(SpinButtonInternal, spin_button_internal, SPIN_BUTTON_INTERNAL)

        static SpinButtonInternal* spin_button_internal_new(GtkSpinButton* scale)
        {
            auto* self = (SpinButtonInternal*) g_object_new(spin_button_internal_get_type(), nullptr);
            spin_button_internal_init(self);
            self->adjustment = new Adjustment(gtk_spin_button_get_adjustment(scale));
            self->native = scale;
            self->value_to_text_function = nullptr;
            self->text_to_value_function = nullptr;
            return self;
        }
    }

    SpinButton::SpinButton(float min, float max, float step, Orientation orientation)
        : Widget(gtk_spin_button_new_with_range(min, max, step)),
          CTOR_SIGNAL(SpinButton, value_changed),
          CTOR_SIGNAL(SpinButton, wrapped),
          CTOR_SIGNAL(SpinButton, realize),
          CTOR_SIGNAL(SpinButton, unrealize),
          CTOR_SIGNAL(SpinButton, destroy),
          CTOR_SIGNAL(SpinButton, hide),
          CTOR_SIGNAL(SpinButton, show),
          CTOR_SIGNAL(SpinButton, map),
          CTOR_SIGNAL(SpinButton, unmap)
    {
        _internal = detail::spin_button_internal_new(GTK_SPIN_BUTTON(Widget::operator NativeWidget()));
        g_object_ref(_internal);
        detail::attach_ref_to(G_OBJECT(_internal->native), _internal);
        set_orientation(orientation);
    }

    SpinButton::SpinButton(detail::SpinButtonInternal* internal)
        : Widget(GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(SpinButton, value_changed),
          CTOR_SIGNAL(SpinButton, wrapped),
          CTOR_SIGNAL(SpinButton, realize),
          CTOR_SIGNAL(SpinButton, unrealize),
          CTOR_SIGNAL(SpinButton, destroy),
          CTOR_SIGNAL(SpinButton, hide),
          CTOR_SIGNAL(SpinButton, show),
          CTOR_SIGNAL(SpinButton, map),
          CTOR_SIGNAL(SpinButton, unmap)
    {
        _internal = g_object_ref(internal);
    }

    SpinButton::~SpinButton()
    {
        g_object_unref(_internal);
    }

    NativeObject SpinButton::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void SpinButton::set_n_digits(size_t n)
    {
        gtk_spin_button_set_digits(_internal->native, n);
    }

    size_t SpinButton::get_n_digits() const
    {
        return gtk_spin_button_get_digits(_internal->native);
    }

    void SpinButton::set_should_wrap(bool b)
    {
        gtk_spin_button_set_wrap(_internal->native, b);
    }

    bool SpinButton::get_should_wrap() const
    {
        return gtk_spin_button_get_wrap(_internal->native);
    }

    Adjustment* SpinButton::get_adjustment()
    {
        return _internal->adjustment;
    }

    const Adjustment* SpinButton::get_adjustment() const
    {
        return _internal->adjustment;
    }

    void SpinButton::set_acceleration_rate(float v)
    {
        if (v < 0)
            v = 0;

        gtk_spin_button_set_climb_rate(_internal->native, v);
    }

    float SpinButton::get_acceleration_rate() const
    {
        return gtk_spin_button_get_climb_rate(_internal->native);
    }

    void SpinButton::set_should_snap_to_ticks(bool b)
    {
        gtk_spin_button_set_snap_to_ticks(_internal->native, b);
    }

    bool SpinButton::get_should_snap_to_ticks() const
    {
        return gtk_spin_button_get_snap_to_ticks(_internal->native);
    }

    void SpinButton::set_allow_only_numeric(bool b)
    {
        gtk_spin_button_set_numeric(_internal->native, b);
    }

    bool SpinButton::get_allow_only_numeric() const
    {
        return gtk_spin_button_get_numeric(_internal->native);
    }

    float SpinButton::get_lower() const
    {
        return gtk_adjustment_get_lower(gtk_spin_button_get_adjustment(_internal->native));
    }

    float SpinButton::get_increment() const
    {
        return gtk_adjustment_get_minimum_increment(gtk_spin_button_get_adjustment(_internal->native));
    }

    float SpinButton::get_upper() const
    {
        return gtk_adjustment_get_upper(gtk_spin_button_get_adjustment(_internal->native));
    }

    float SpinButton::get_value() const
    {
        return gtk_adjustment_get_value(gtk_spin_button_get_adjustment(_internal->native));
    }

    void SpinButton::set_value(float value)
    {
        gtk_adjustment_set_value(gtk_spin_button_get_adjustment(_internal->native), value);
    }

    void SpinButton::set_lower(float value)
    {
        gtk_adjustment_set_lower(gtk_spin_button_get_adjustment(_internal->native), value);
    }

    void SpinButton::set_upper(float value)
    {
        gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(_internal->native), value);
    }

    void SpinButton::set_increment(float value)
    {
        gtk_adjustment_set_step_increment(gtk_spin_button_get_adjustment(_internal->native), value);
    }

    void SpinButton::reset_text_to_value_function()
    {
        disconnect_signal("input");
    }

    void SpinButton::reset_value_to_text_function()
    {
        disconnect_signal("output");
    }

    gint SpinButton::on_input(GtkSpinButton* button, double* new_value, detail::SpinButtonInternal* instance)
    {
        std::string text = gtk_editable_get_text(GTK_EDITABLE(button));
        float res = 0;
        try
        {
            auto button = SpinButton(instance);
            res = instance->text_to_value_function(button, text);
        }
        catch (std::exception& e)
        {
            log::critical("In SpinButton::on_input: Bound text-to-value function caused an exception: " + std::string(e.what()));
            return GTK_INPUT_ERROR;
        }

        *new_value = (double) res;
        return TRUE;
    }

    bool SpinButton::on_output(GtkSpinButton*, detail::SpinButtonInternal* instance)
    {
        float value = gtk_adjustment_get_value(gtk_spin_button_get_adjustment(instance->native));

        try
        {
            auto button = SpinButton(instance);
            std::string res = instance->value_to_text_function(button, value);
            gtk_editable_set_text(GTK_EDITABLE(instance->native), res.c_str());
            return TRUE;
        }
        catch (std::exception& e)
        {
            log::critical("In SpinButton::on_output: Bound value-to-text function caused an exception: " + std::string(e.what()));
            return FALSE;
        }
    }

    void SpinButton::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(_internal->native), (GtkOrientation) orientation);
    }

    Orientation SpinButton::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(_internal->native));
    }
}
