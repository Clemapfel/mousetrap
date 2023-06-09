//
// Created by clem on 3/23/23.
//

#include <mousetrap/switch.hpp>

namespace mousetrap
{
    Switch::Switch()
        : Widget(gtk_switch_new()),
          CTOR_SIGNAL(Switch, activate)
    {
        _internal = GTK_SWITCH(Widget::operator NativeWidget());
        g_object_ref(_internal);
    }

    Switch::Switch(detail::SwitchInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Switch, activate)
    {
        _internal = g_object_ref(internal);
    }

    Switch::~Switch()
    {
        g_object_unref(_internal);
    }

    NativeObject Switch::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    bool Switch::get_is_active() const
    {
        return gtk_switch_get_active(GTK_SWITCH(_internal));
    }

    void Switch::set_is_active(bool b)
    {
        gtk_switch_set_active(GTK_SWITCH(_internal), b);
    }
}