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
        DECLARE_NEW_TYPE(PopoverMenuInternal, popover_menu_internal, POPOVER_MENU_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(PopoverMenuInternal, popover_menu_internal, POPOVER_MENU_INTERNAL)

        static void popover_menu_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_POPOVER_MENU_INTERNAL(object);
            G_OBJECT_CLASS(popover_menu_internal_parent_class)->finalize(object);
            g_object_unref(self->model);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(PopoverMenuInternal, popover_menu_internal, POPOVER_MENU_INTERNAL)

        static PopoverMenuInternal* popover_menu_internal_new(GtkPopoverMenu* native, detail::MenuModelInternal* model)
        {
            auto* self = (PopoverMenuInternal*) g_object_new(popover_menu_internal_get_type(), nullptr);
            popover_menu_internal_init(self);

            self->native = native;
            self->model = model;

            g_object_ref(self->model);
            return self;
        }
    }
    
    PopoverMenu::PopoverMenu(const MenuModel& model)
        : Widget(gtk_popover_menu_new_from_model(model.operator GMenuModel*())),
        CTOR_SIGNAL(PopoverMenu, closed)
    {
        _internal = detail::popover_menu_internal_new(
            GTK_POPOVER_MENU(operator NativeWidget()),
            (detail::MenuModelInternal*) model.get_internal()
        );
        refresh_widgets();
    }

    PopoverMenu::PopoverMenu(detail::PopoverMenuInternal* internal)
        : Widget(GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(PopoverMenu, closed)
    {
        _internal = g_object_ref(internal);
    }

    PopoverMenu::~PopoverMenu()
    {
        g_object_unref(_internal);
    }

    NativeObject PopoverMenu::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void PopoverMenu::refresh_widgets()
    {
        auto model = MenuModel(_internal->model);
        for (auto& pair:model.get_widgets())
        {
            if (gtk_popover_menu_add_child(GTK_POPOVER_MENU(operator NativeWidget()), pair.second, pair.first.c_str()) != TRUE)
            {
                continue; // TODO Why is triggered but widget insertion seem to be working fine?

                std::stringstream str;
                str << "In PopoverMenu::refresh_widgets: Failed to add Widget of type " << G_STRINGIFY(pair.second) << " to submenu." << std::endl;
                log::critical(str.str(), MOUSETRAP_DOMAIN);
            }
        }
    }
}