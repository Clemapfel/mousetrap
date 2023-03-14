namespace mousetrap
{
    inline LevelBar::LevelBar(float min, float max)
            : WidgetImplementation<GtkLevelBar>(GTK_LEVEL_BAR(gtk_level_bar_new_for_interval(min, max)))
    {}

    inline void LevelBar::add_offset_marker(std::string name, float value)
    {
        gtk_level_bar_add_offset_value(get_native(), name.c_str(), value);
    }

    inline void LevelBar::set_inverted(bool b)
    {
        gtk_level_bar_set_inverted(get_native(), b);
    }

    inline bool LevelBar::get_inverted() const
    {
        return gtk_level_bar_get_inverted(get_native());
    }

    inline void LevelBar::set_mode(LevelBarMode mode)
    {
        gtk_level_bar_set_mode(get_native(), (GtkLevelBarMode) mode);
    }

    inline LevelBarMode LevelBar::get_mode() const
    {
        return (LevelBarMode) gtk_level_bar_get_mode(get_native());
    }

    inline void LevelBar::set_min_value(float min)
    {
        gtk_level_bar_set_min_value(get_native(), min);
    }

    inline float LevelBar::get_min_value() const
    {
        return gtk_level_bar_get_min_value(get_native());
    }

    inline void LevelBar::set_max_value(float max)
    {
        gtk_level_bar_set_max_value(get_native(), max);
    }

    inline float LevelBar::get_max_value() const
    {
        return gtk_level_bar_get_max_value(get_native());
    }

    inline void LevelBar::set_value(float v)
    {
        gtk_level_bar_set_value(get_native(), v);
    }

    inline float LevelBar::get_value() const
    {
        return gtk_level_bar_get_value(get_native());
    }
}
