//
// Created by clem on 3/23/23.
//

#include <include/switch.hpp>

namespace mousetrap
{
    Switch::Switch()
        : WidgetImplementation<GtkSwitch>(GTK_SWITCH(gtk_switch_new())),
          CTOR_SIGNAL(Switch, activate)
    {}

    bool Switch::get_active() const
    {
        return gtk_switch_get_active(get_native());
    }

    void Switch::set_active(bool b)
    {
        gtk_switch_set_active(get_native(), b);
    }
}