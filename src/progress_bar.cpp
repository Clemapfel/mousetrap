//
// Created by clem on 3/23/23.
//

#include <mousetrap/progress_bar.hpp>

namespace mousetrap
{
    ProgressBar::ProgressBar()
        : Widget(gtk_progress_bar_new()),
          CTOR_SIGNAL(ProgressBar, realize),
          CTOR_SIGNAL(ProgressBar, unrealize),
          CTOR_SIGNAL(ProgressBar, destroy),
          CTOR_SIGNAL(ProgressBar, hide),
          CTOR_SIGNAL(ProgressBar, show),
          CTOR_SIGNAL(ProgressBar, map),
          CTOR_SIGNAL(ProgressBar, unmap)
    {
        _internal = GTK_PROGRESS_BAR(operator NativeWidget());
        g_object_ref(_internal);
    }
    
    ProgressBar::ProgressBar(detail::ProgressBarInternal* internal) 
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(ProgressBar, realize),
          CTOR_SIGNAL(ProgressBar, unrealize),
          CTOR_SIGNAL(ProgressBar, destroy),
          CTOR_SIGNAL(ProgressBar, hide),
          CTOR_SIGNAL(ProgressBar, show),
          CTOR_SIGNAL(ProgressBar, map),
          CTOR_SIGNAL(ProgressBar, unmap)
    {
        _internal = g_object_ref(internal);
    }

    ProgressBar::~ProgressBar() 
    {
        g_object_unref(_internal);
    }

    NativeObject ProgressBar::get_internal() const 
    {
        return G_OBJECT(_internal);
    }
    
    void ProgressBar::pulse()
    {
        gtk_progress_bar_pulse(GTK_PROGRESS_BAR(operator NativeWidget()));
    }

    void ProgressBar::set_fraction(float v)
    {
        v = glm::clamp<float>(v, 0, 1);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(operator NativeWidget()), v);
    }

    float ProgressBar::get_fraction() const
    {
        return gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(operator NativeWidget()));
    }

    bool ProgressBar::get_is_inverted() const
    {
        return gtk_progress_bar_get_inverted(GTK_PROGRESS_BAR(operator NativeWidget()));
    }

    void ProgressBar::set_is_inverted(bool b)
    {
        gtk_progress_bar_set_inverted(GTK_PROGRESS_BAR(operator NativeWidget()), b);
    }

    void ProgressBar::set_text(const std::string& text)
    {
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(operator NativeWidget()), text.c_str());
        gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(operator NativeWidget()), true);
    }

    std::string ProgressBar::get_text() const
    {
        auto* text = gtk_progress_bar_get_text(GTK_PROGRESS_BAR(operator NativeWidget()));
        return text == nullptr ? "" : std::string(text);
    }

    void ProgressBar::set_show_text(bool b)
    {
        gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(operator NativeWidget()), b);
    }

    bool ProgressBar::get_show_text() const
    {
        return gtk_progress_bar_get_show_text(GTK_PROGRESS_BAR(operator NativeWidget()));
    }

    void ProgressBar::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(GTK_PROGRESS_BAR(operator NativeWidget())), (GtkOrientation) orientation);
    }

    Orientation ProgressBar::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(GTK_PROGRESS_BAR(operator NativeWidget())));
    }
}
