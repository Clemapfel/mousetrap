//
// Created by clem on 8/8/23.
//

#include <mousetrap/action_bar.hpp>

namespace mousetrap 
{
    ActionBar::ActionBar()
        : Widget(gtk_action_bar_new()),
          CTOR_SIGNAL(ActionBar, realize),
          CTOR_SIGNAL(ActionBar, unrealize),
          CTOR_SIGNAL(ActionBar, destroy),
          CTOR_SIGNAL(ActionBar, hide),
          CTOR_SIGNAL(ActionBar, show),
          CTOR_SIGNAL(ActionBar, map),
          CTOR_SIGNAL(ActionBar, unmap)
    {
        _internal = GTK_ACTION_BAR(operator NativeWidget());
        g_object_ref_sink(_internal);
    }

    ActionBar::ActionBar(detail::ActionBarInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(ActionBar, realize),
          CTOR_SIGNAL(ActionBar, unrealize),
          CTOR_SIGNAL(ActionBar, destroy),
          CTOR_SIGNAL(ActionBar, hide),
          CTOR_SIGNAL(ActionBar, show),
          CTOR_SIGNAL(ActionBar, map),
          CTOR_SIGNAL(ActionBar, unmap)
    {
        _internal = g_object_ref(_internal);
    }

    ActionBar::~ActionBar()
    {
        g_object_unref(_internal);
    }

    NativeObject ActionBar::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void ActionBar::push_back(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(ActionBar::push_back, this, ptr);
        WARN_IF_PARENT_EXISTS(ActionBar::push_back, widget);
        gtk_action_bar_pack_end(GTK_ACTION_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }

    void ActionBar::push_front(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(ActionBar::push_front, this, ptr);
        WARN_IF_PARENT_EXISTS(ActionBar::push_front, widget);
        gtk_action_bar_pack_start(GTK_ACTION_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }

    void ActionBar::remove(const Widget& widget)
    {
        gtk_action_bar_remove(GTK_ACTION_BAR(operator NativeWidget()), widget.operator GtkWidget *());
    }

    void ActionBar::set_center_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(ActionBar::set_center_widget, this, ptr);
        WARN_IF_PARENT_EXISTS(ActionBar::set_center_widget, widget);
        gtk_action_bar_set_center_widget(GTK_ACTION_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }

    void ActionBar::remove_center_child()
    {
        gtk_action_bar_set_center_widget(GTK_ACTION_BAR(operator NativeWidget()), nullptr);
    }

    void ActionBar::set_is_revealed(bool b)
    {
        gtk_action_bar_set_revealed(GTK_ACTION_BAR(operator NativeWidget()), b);
    }

    bool ActionBar::get_is_revealed() const
    {
        return gtk_action_bar_get_revealed(GTK_ACTION_BAR(operator NativeWidget()));
    }
}
