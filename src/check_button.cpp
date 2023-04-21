//
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/check_button.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    CheckButton::CheckButton()
        : WidgetImplementation<GtkCheckButton>(GTK_CHECK_BUTTON(gtk_check_button_new())),
          CTOR_SIGNAL(CheckButton, activate),
          CTOR_SIGNAL(CheckButton, toggled)
    {}

    #if GTK_MINOR_VERSION >= 8

    void CheckButton::set_child(const Widget& child)
    {
        _child = &child;
        WARN_IF_SELF_INSERTION(CheckButton::set_child, this, _child);

        gtk_check_button_set_child(get_native(), child.operator GtkWidget*());
    }

    void CheckButton::remove_child()
    {
        _child = nullptr;
        gtk_check_button_set_child(get_native(), nullptr);
    }

    Widget* CheckButton::get_child() const
    {
        return const_cast<Widget*>(_child);
    }

    #endif

    void CheckButton::set_state(CheckButtonState state)
    {
        switch (state)
        {
            case CheckButtonState::ACTIVE:
                gtk_check_button_set_active(get_native(), TRUE);
                return;
            case CheckButtonState::INACTIVE:
                gtk_check_button_set_active(get_native(), FALSE);
                return;
            case CheckButtonState::INCONSISTENT:
                gtk_check_button_set_inconsistent(get_native(), true);
        }
    }

    CheckButtonState CheckButton::get_state() const
    {
        if (gtk_check_button_get_inconsistent(get_native()))
            return CheckButtonState::INCONSISTENT;
        else
            return gtk_check_button_get_active(get_native()) ? CheckButtonState::ACTIVE : CheckButtonState::INACTIVE;
    }

    bool CheckButton::get_active() const
    {
        return get_state() == CheckButtonState::ACTIVE;
    }
}