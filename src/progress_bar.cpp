//
// Created by clem on 3/23/23.
//

#include <include/progress_bar.hpp>

namespace mousetrap
{
    ProgressBar::ProgressBar()
        : WidgetImplementation<GtkProgressBar>(GTK_PROGRESS_BAR(gtk_progress_bar_new()))
    {}

    void ProgressBar::pulse()
    {
        gtk_progress_bar_pulse(get_native());
    }

    void ProgressBar::set_fraction(float v)
    {
        v = glm::clamp<float>(v, 0, 1);
        gtk_progress_bar_set_fraction(get_native(), v);
    }

    float ProgressBar::get_fraction() const
    {
        return gtk_progress_bar_get_fraction(get_native());
    }

    bool ProgressBar::get_is_inverted() const
    {
        return gtk_progress_bar_get_inverted(get_native());
    }

    void ProgressBar::set_is_inverted(bool b)
    {
        gtk_progress_bar_set_inverted(get_native(), b);
    }

    void ProgressBar::set_text(const std::string& text)
    {
        gtk_progress_bar_set_text(get_native(), text.c_str());
    }

    std::string ProgressBar::get_text() const
    {
        return gtk_progress_bar_get_text(get_native());
    }

    void ProgressBar::set_show_text_or_percentage(bool b)
    {
        gtk_progress_bar_set_show_text(get_native(), b);
    }

    bool ProgressBar::get_show_text_or_percentage() const
    {
        return gtk_progress_bar_get_show_text(get_native());
    }

    void ProgressBar::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(get_native()), (GtkOrientation) orientation);
    }

    Orientation ProgressBar::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(get_native()));
    }
}
