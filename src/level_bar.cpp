#include <include/level_bar.hpp>

namespace mousetrap
{
    LevelBar::LevelBar(float min, float max)
    : WidgetImplementation<GtkLevelBar>(GTK_LEVEL_BAR(gtk_level_bar_new_for_interval(min, max)))
    {}

    void LevelBar::add_marker(const std::string& name, float value)
    {
        gtk_level_bar_add_offset_value(get_native(), name.c_str(), value);
    }

    void LevelBar::remove_marker(const std::string& name)
    {
        gtk_level_bar_remove_offset_value(get_native(), name.c_str());
    }

    void LevelBar::set_inverted(bool b)
    {
        gtk_level_bar_set_inverted(get_native(), b);
    }

    bool LevelBar::get_inverted() const
    {
        return gtk_level_bar_get_inverted(get_native());
    }

    void LevelBar::set_mode(LevelBarMode mode)
    {
        gtk_level_bar_set_mode(get_native(), (GtkLevelBarMode) mode);
    }

    LevelBarMode LevelBar::get_mode() const
    {
        return (LevelBarMode) gtk_level_bar_get_mode(get_native());
    }

    void LevelBar::set_min_value(float min)
    {
        gtk_level_bar_set_min_value(get_native(), min);
    }

    float LevelBar::get_min_value() const
    {
        return gtk_level_bar_get_min_value(get_native());
    }

    void LevelBar::set_max_value(float max)
    {
        gtk_level_bar_set_max_value(get_native(), max);
    }

    float LevelBar::get_max_value() const
    {
        return gtk_level_bar_get_max_value(get_native());
    }

    void LevelBar::set_value(float v)
    {
        gtk_level_bar_set_value(get_native(), v);
    }

    float LevelBar::get_value() const
    {
        return gtk_level_bar_get_value(get_native());
    }

    void LevelBar::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(get_native()), (GtkOrientation) orientation);
    }

    Orientation LevelBar::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(get_native()));
    }
}
