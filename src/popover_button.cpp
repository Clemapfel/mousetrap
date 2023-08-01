// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/popover_button.hpp>
#include <mousetrap/log.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(PopoverButtonInternal, popover_button_internal, POPOVER_BUTTON_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(PopoverButtonInternal, popover_button_internal, POPOVER_BUTTON_INTERNAL)

        static void popover_button_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_POPOVER_BUTTON_INTERNAL(object);
            G_OBJECT_CLASS(popover_button_internal_parent_class)->finalize(object);

            if (self->menu != nullptr)
                g_object_unref(self->menu);

            if (self->popover != nullptr)
                g_object_unref(self->popover);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(PopoverButtonInternal, popover_button_internal, POPOVER_BUTTON_INTERNAL)

        static PopoverButtonInternal* popover_button_internal_new(GtkMenuButton* native)
        {
            auto* self = (PopoverButtonInternal*) g_object_new(popover_button_internal_get_type(), nullptr);
            popover_button_internal_init(self);

            self->native = native;
            self->menu = nullptr;

            return self;
        }
    }
    
    PopoverButton::PopoverButton()
        : Widget(gtk_menu_button_new()),
          CTOR_SIGNAL(PopoverButton, activate),
          CTOR_SIGNAL(PopoverButton, realize),
          CTOR_SIGNAL(PopoverButton, unrealize),
          CTOR_SIGNAL(PopoverButton, destroy),
          CTOR_SIGNAL(PopoverButton, hide),
          CTOR_SIGNAL(PopoverButton, show),
          CTOR_SIGNAL(PopoverButton, map),
          CTOR_SIGNAL(PopoverButton, unmap)
    {
        _internal = detail::popover_button_internal_new(GTK_MENU_BUTTON(Widget::operator NativeWidget()));
        g_object_ref(_internal);
        
        gtk_menu_button_set_always_show_arrow(_internal->native, true);
    }

    PopoverButton::PopoverButton(detail::PopoverButtonInternal* internal) 
        : Widget(GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(PopoverButton, activate),
          CTOR_SIGNAL(PopoverButton, realize),
          CTOR_SIGNAL(PopoverButton, unrealize),
          CTOR_SIGNAL(PopoverButton, destroy),
          CTOR_SIGNAL(PopoverButton, hide),
          CTOR_SIGNAL(PopoverButton, show),
          CTOR_SIGNAL(PopoverButton, map),
          CTOR_SIGNAL(PopoverButton, unmap)
    {
        _internal = g_object_ref(internal);
    }

    PopoverButton::PopoverButton(const Popover& popover)
        : PopoverButton()
    {
        set_popover(popover);
    }

    PopoverButton::PopoverButton(const PopoverMenu& menu)
        : PopoverButton()
    {
        set_popover_menu(menu);
    }
    
    PopoverButton::~PopoverButton() 
    {
        g_object_unref(_internal);
    }

    NativeObject PopoverButton::get_internal() const 
    {
        return G_OBJECT(_internal);
    }
    
    void PopoverButton::set_child(const Widget& child)
    {
        auto* ptr = &child;
        WARN_IF_SELF_INSERTION(PopoverButton::set_child, this, ptr);
        WARN_IF_PARENT_EXISTS(PopoverButton::set_child, child);

        gtk_menu_button_set_child(_internal->native, child.operator GtkWidget*());

        if (_internal->menu != nullptr)
            PopoverMenu(_internal->menu).refresh_widgets();
    }

    void PopoverButton::set_icon(const Icon& icon)
    {
        gtk_menu_button_set_child(_internal->native, gtk_image_new_from_gicon(icon.operator GIcon*()));
    }

    void PopoverButton::remove_child()
    {
        gtk_menu_button_set_child(_internal->native, nullptr);
    }

    void PopoverButton::set_relative_position(RelativePosition type)
    {
        auto* popover = gtk_menu_button_get_popover(_internal->native);
        if (GTK_IS_POPOVER(popover))
            gtk_popover_set_position(popover, (GtkPositionType) type);
        else
            log::critical("In PopoverButton::set_relative_position: No popover connected.", MOUSETRAP_DOMAIN);
    }

    RelativePosition PopoverButton::get_relative_position() const
    {
        auto* popover = gtk_menu_button_get_popover(_internal->native);

        if (GTK_IS_POPOVER(popover))
            return (RelativePosition) gtk_popover_get_position(popover);
        else
        {
            log::critical("In PopoverButton::get_relative_position: No popover connected.", MOUSETRAP_DOMAIN);
            return RelativePosition::ABOVE;
        }
    }

    void PopoverButton::set_popover(const Popover& popover)
    {
        _internal->menu = nullptr;
        _internal->popover = (detail::PopoverInternal*) popover.get_internal();
        gtk_menu_button_set_popover(_internal->native, popover.operator GtkWidget*());
    }

    void PopoverButton::set_popover_menu(const PopoverMenu& popover_menu)
    {
        _internal->menu = (detail::PopoverMenuInternal*) popover_menu.get_internal();;
        _internal->popover = nullptr;

        gtk_menu_button_set_popover(_internal->native, popover_menu.operator GtkWidget*());
        PopoverMenu(_internal->menu).refresh_widgets();
    }

    void PopoverButton::remove_popover()
    {
        _internal->menu = nullptr;
        _internal->popover = nullptr;
        gtk_menu_button_set_popover(_internal->native, nullptr);
    }

    void PopoverButton::popup()
    {
        gtk_menu_button_popup(_internal->native);
    }

    void PopoverButton::popdown()
    {
        gtk_menu_button_popdown(_internal->native);
    }

    void PopoverButton::set_always_show_arrow(bool b)
    {
        gtk_menu_button_set_always_show_arrow(_internal->native, b);
    }

    bool PopoverButton::get_always_show_arrow() const
    {
        return gtk_menu_button_get_always_show_arrow(_internal->native);
    }

    void PopoverButton::set_has_frame(bool b)
    {
        gtk_menu_button_set_has_frame(_internal->native, b);
    }

    bool PopoverButton::get_has_frame() const
    {
        return gtk_menu_button_get_has_frame(_internal->native);
    }

    void PopoverButton::set_is_circular(bool b)
    {
        if (b and not get_is_circular())
            gtk_widget_add_css_class(GTK_WIDGET(_internal->native), "circular");
        else if (not b and get_is_circular())
            gtk_widget_remove_css_class(GTK_WIDGET(_internal->native), "circular");
    }

    bool PopoverButton::get_is_circular() const
    {
        return gtk_widget_has_css_class(GTK_WIDGET(_internal->native), "circular");
    }
}