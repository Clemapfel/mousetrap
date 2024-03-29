//
// Created by clem on 3/23/23.
//

#include <mousetrap/scale.hpp>
#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(ScaleInternal, scale_internal, SCALE_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(ScaleInternal, scale_internal, SCALE_INTERNAL)

        static void scale_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_SCALE_INTERNAL(object);
            G_OBJECT_CLASS(scale_internal_parent_class)->finalize(object);
            delete self->adjustment;
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(ScaleInternal, scale_internal, SCALE_INTERNAL)

        static ScaleInternal* scale_internal_new(GtkScale* scale)
        {
            auto* self = (ScaleInternal*) g_object_new(scale_internal_get_type(), nullptr);
            scale_internal_init(self);
            self->native = scale;
            self->adjustment = new Adjustment(gtk_range_get_adjustment(GTK_RANGE(scale)));
            self->formatting_function = nullptr;

            detail::attach_ref_to(G_OBJECT(self->native), self);
            return self;
        }
    }
    
    Scale::Scale(float min, float max, float step, Orientation orientation)
        : Widget(gtk_scale_new((GtkOrientation) orientation, gtk_adjustment_new(glm::mix(min, max, 0.5), min, max, step, 0, 0))),
          CTOR_SIGNAL(Scale, value_changed),
          CTOR_SIGNAL(Scale, realize),
          CTOR_SIGNAL(Scale, unrealize),
          CTOR_SIGNAL(Scale, destroy),
          CTOR_SIGNAL(Scale, hide),
          CTOR_SIGNAL(Scale, show),
          CTOR_SIGNAL(Scale, map),
          CTOR_SIGNAL(Scale, unmap)
    {
        _internal = detail::scale_internal_new(GTK_SCALE(operator NativeWidget()));
        g_object_ref(_internal);
        gtk_scale_set_draw_value(GTK_SCALE(operator NativeWidget()), false);
    }
    
    Scale::Scale(detail::ScaleInternal* internal)
        : Widget(GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(Scale, value_changed),
          CTOR_SIGNAL(Scale, realize),
          CTOR_SIGNAL(Scale, unrealize),
          CTOR_SIGNAL(Scale, destroy),
          CTOR_SIGNAL(Scale, hide),
          CTOR_SIGNAL(Scale, show),
          CTOR_SIGNAL(Scale, map),
          CTOR_SIGNAL(Scale, unmap)
    {
        _internal = g_object_ref(internal);
    }
    
    Scale::~Scale() 
    {
        g_object_unref(_internal);
    }

    NativeObject Scale::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    Adjustment Scale::get_adjustment() const
    {
        return Adjustment((detail::AdjustmentInternal*) _internal->adjustment->get_internal());
    }

    float Scale::get_lower() const
    {
        return gtk_adjustment_get_lower(gtk_range_get_adjustment(GTK_RANGE(GTK_SCALE(operator NativeWidget()))));
    }

    float Scale::get_step_increment() const
    {
        return gtk_adjustment_get_minimum_increment(gtk_range_get_adjustment(GTK_RANGE(GTK_SCALE(operator NativeWidget()))));
    }

    float Scale::get_upper() const
    {
        return gtk_adjustment_get_upper(gtk_range_get_adjustment(GTK_RANGE(GTK_SCALE(operator NativeWidget()))));
    }

    float Scale::get_value() const
    {
        return gtk_adjustment_get_value(gtk_range_get_adjustment(GTK_RANGE(GTK_SCALE(operator NativeWidget()))));
    }

    void Scale::set_value(float value)
    {
        gtk_adjustment_set_value(gtk_range_get_adjustment(GTK_RANGE(GTK_SCALE(operator NativeWidget()))), value);
    }

    void Scale::set_lower(float value)
    {
        gtk_adjustment_set_lower(gtk_range_get_adjustment(GTK_RANGE(GTK_SCALE(operator NativeWidget()))), value);
    }

    void Scale::set_upper(float value)
    {
        gtk_adjustment_set_upper(gtk_range_get_adjustment(GTK_RANGE(GTK_SCALE(operator NativeWidget()))), value);
    }

    void Scale::set_step_increment(float value)
    {
        gtk_adjustment_set_step_increment(gtk_range_get_adjustment(GTK_RANGE(GTK_SCALE(operator NativeWidget()))), value);
    }

    void Scale::set_should_draw_value(bool b)
    {
        gtk_scale_set_draw_value(GTK_SCALE(operator NativeWidget()), b);
    }

    bool Scale::get_should_draw_value() const
    {
        return gtk_scale_get_draw_value(GTK_SCALE(operator NativeWidget()));
    }

    void Scale::set_has_origin(bool b)
    {
        gtk_scale_set_has_origin(GTK_SCALE(operator NativeWidget()), b);
    }

    bool Scale::get_has_origin() const
    {
        return gtk_scale_get_has_origin(GTK_SCALE(operator NativeWidget()));
    }

    void Scale::add_mark(float at, RelativePosition pos, const std::string& label)
    {
        gtk_scale_add_mark(GTK_SCALE(operator NativeWidget()), at, (GtkPositionType) pos, label.empty() ? nullptr : label.c_str());
    }

    void Scale::clear_marks()
    {
        gtk_scale_clear_marks(GTK_SCALE(operator NativeWidget()));
    }

    void Scale::reset_format_value_function()
    {
        gtk_scale_set_format_value_func(GTK_SCALE(operator NativeWidget()), nullptr, nullptr, nullptr);
    }

    char* Scale::on_format_value(GtkScale* scale, double value, detail::ScaleInternal* instance)
    {
        auto out = instance->formatting_function(value);
        return g_strdup(out.c_str());
    }

    void Scale::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(GTK_SCALE(operator NativeWidget())), (GtkOrientation) orientation);
    }

    Orientation Scale::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(GTK_SCALE(operator NativeWidget())));
    }
}
