//
// Created by clem on 3/23/23.
//

#include <include/spinner.hpp>

namespace mousetrap
{
    Spinner::Spinner()
        : WidgetImplementation<GtkSpinner>(GTK_SPINNER(gtk_spinner_new()))
    {}

    bool Spinner::get_is_spinning() const
    {
        return gtk_spinner_get_spinning(get_native());
    }

    void Spinner::set_is_spinning(bool b)
    {
        gtk_spinner_set_spinning(get_native(), b);
    }

    void Spinner::start()
    {
        gtk_spinner_start(get_native());
    }

    void Spinner::stop()
    {
        gtk_spinner_stop(get_native());
    }
}
