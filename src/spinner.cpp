//
// Created by clem on 3/23/23.
//

#include <mousetrap/spinner.hpp>

namespace mousetrap
{
    Spinner::Spinner()
        : Widget(gtk_spinner_new()),
          CTOR_SIGNAL(Spinner, realize),
          CTOR_SIGNAL(Spinner, unrealize),
          CTOR_SIGNAL(Spinner, destroy),
          CTOR_SIGNAL(Spinner, hide),
          CTOR_SIGNAL(Spinner, show),
          CTOR_SIGNAL(Spinner, map),
          CTOR_SIGNAL(Spinner, unmap)
    {
        _internal = GTK_SPINNER(Widget::operator NativeWidget());
        set_is_spinning(true);
        g_object_ref_sink(_internal);
    }
    
    Spinner::Spinner(detail::SpinnerInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Spinner, realize),
          CTOR_SIGNAL(Spinner, unrealize),
          CTOR_SIGNAL(Spinner, destroy),
          CTOR_SIGNAL(Spinner, hide),
          CTOR_SIGNAL(Spinner, show),
          CTOR_SIGNAL(Spinner, map),
          CTOR_SIGNAL(Spinner, unmap)
    {
        _internal = g_object_ref(internal);
    }

    Spinner::~Spinner()
    {
        g_object_unref(_internal);
    }

    NativeObject Spinner::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    bool Spinner::get_is_spinning() const
    {
        return gtk_spinner_get_spinning(_internal);
    }

    void Spinner::set_is_spinning(bool b)
    {
        gtk_spinner_set_spinning(_internal, b);
    }

    void Spinner::start()
    {
        gtk_spinner_start(_internal);
    }

    void Spinner::stop()
    {
        gtk_spinner_stop(_internal);
    }
}
