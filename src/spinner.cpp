//
// Created by clem on 3/23/23.
//

#include <mousetrap/spinner.hpp>

namespace mousetrap
{
    Spinner::Spinner()
        : Widget(gtk_spinner_new())
    {
        _internal = GTK_SPINNER(Widget::operator NativeWidget());
    }
    
    Spinner::Spinner(detail::SpinnerInternal* internal)
        : Widget(GTK_WIDGET(internal))
    {
        _internal = g_object_ref(internal);
    }

    Spinner::~Spinner() noexcept
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
