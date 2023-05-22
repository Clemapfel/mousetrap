#include <mousetrap/level_bar.hpp>

namespace mousetrap
{
    LevelBar::LevelBar(float min, float max)
        : Widget(gtk_level_bar_new_for_interval(min, max))
    {}
    
    LevelBar::LevelBar(detail::LevelBarInternal* internal) 
        : Widget(GTK_WIDGET(internal))
    {
        _internal = g_object_ref(internal);
    }
    
    LevelBar::~LevelBar()
    {
        g_object_unref(_internal);
    }

    NativeObject LevelBar::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void LevelBar::add_marker(const std::string& name, float value)
    {
        gtk_level_bar_add_offset_value(GTK_LEVEL_BAR(operator NativeWidget()), name.c_str(), value);
    }

    void LevelBar::remove_marker(const std::string& name)
    {
        gtk_level_bar_remove_offset_value(GTK_LEVEL_BAR(operator NativeWidget()), name.c_str());
    }

    void LevelBar::set_inverted(bool b)
    {
        gtk_level_bar_set_inverted(GTK_LEVEL_BAR(operator NativeWidget()), b);
    }

    bool LevelBar::get_inverted() const
    {
        return gtk_level_bar_get_inverted(GTK_LEVEL_BAR(operator NativeWidget()));
    }

    void LevelBar::set_mode(LevelBarMode mode)
    {
        gtk_level_bar_set_mode(GTK_LEVEL_BAR(operator NativeWidget()), (GtkLevelBarMode) mode);
    }

    LevelBarMode LevelBar::get_mode() const
    {
        return (LevelBarMode) gtk_level_bar_get_mode(GTK_LEVEL_BAR(operator NativeWidget()));
    }

    void LevelBar::set_min_value(float min)
    {
        gtk_level_bar_set_min_value(GTK_LEVEL_BAR(operator NativeWidget()), min);
    }

    float LevelBar::get_min_value() const
    {
        return gtk_level_bar_get_min_value(GTK_LEVEL_BAR(operator NativeWidget()));
    }

    void LevelBar::set_max_value(float max)
    {
        gtk_level_bar_set_max_value(GTK_LEVEL_BAR(operator NativeWidget()), max);
    }

    float LevelBar::get_max_value() const
    {
        return gtk_level_bar_get_max_value(GTK_LEVEL_BAR(operator NativeWidget()));
    }

    void LevelBar::set_value(float v)
    {
        gtk_level_bar_set_value(GTK_LEVEL_BAR(operator NativeWidget()), v);
    }

    float LevelBar::get_value() const
    {
        return gtk_level_bar_get_value(GTK_LEVEL_BAR(operator NativeWidget()));
    }

    void LevelBar::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(GTK_LEVEL_BAR(operator NativeWidget())), (GtkOrientation) orientation);
    }

    Orientation LevelBar::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(GTK_LEVEL_BAR(operator NativeWidget())));
    }
}
