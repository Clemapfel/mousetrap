//
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/check_button.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    CheckButton::CheckButton()
        : Widget(gtk_check_button_new()),
          CTOR_SIGNAL(CheckButton, activate),
          CTOR_SIGNAL(CheckButton, toggled)
    {
        _internal = GTK_CHECK_BUTTON(operator NativeWidget());
        g_object_ref_sink(_internal);
    }

    CheckButton::CheckButton(detail::CheckButtonInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(CheckButton, activate),
          CTOR_SIGNAL(CheckButton, toggled)
    {
        _internal = g_object_ref(internal);
    }

    CheckButton::~CheckButton()
    {
        g_object_unref(_internal);
    }

    NativeObject CheckButton::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    #if GTK_MINOR_VERSION >= 8

    void CheckButton::set_child(const Widget& child)
    {
        auto* ptr = &child;
        WARN_IF_SELF_INSERTION(CheckButton::set_child, this, ptr);

        gtk_check_button_set_child(GTK_CHECK_BUTTON(operator NativeWidget()), child.operator GtkWidget*());
    }

    void CheckButton::remove_child()
    {
        gtk_check_button_set_child(GTK_CHECK_BUTTON(operator NativeWidget()), nullptr);
    }

    #endif

    void CheckButton::set_state(CheckButtonState state)
    {
        switch (state)
        {
            case CheckButtonState::ACTIVE:
                gtk_check_button_set_active(GTK_CHECK_BUTTON(operator NativeWidget()), TRUE);
                return;
            case CheckButtonState::INACTIVE:
                gtk_check_button_set_active(GTK_CHECK_BUTTON(operator NativeWidget()), FALSE);
                return;
            case CheckButtonState::INCONSISTENT:
                gtk_check_button_set_inconsistent(GTK_CHECK_BUTTON(operator NativeWidget()), true);
        }
    }

    CheckButtonState CheckButton::get_state() const
    {
        if (gtk_check_button_get_inconsistent(GTK_CHECK_BUTTON(operator NativeWidget())))
            return CheckButtonState::INCONSISTENT;
        else
            return gtk_check_button_get_active(GTK_CHECK_BUTTON(operator NativeWidget())) ? CheckButtonState::ACTIVE : CheckButtonState::INACTIVE;
    }

    bool CheckButton::get_active() const
    {
        return get_state() == CheckButtonState::ACTIVE;
    }
}