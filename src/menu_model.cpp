//
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/menu_model.hpp>
#include <mousetrap/log.hpp>

#include <iostream>
#include <map>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(MenuModelInternal, menu_model_internal, MENU_MODEL_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(MenuModelInternal, menu_model_internal, MENU_MODEL_INTERNAL)

        static void menu_model_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_MENU_MODEL_INTERNAL(object);
            G_OBJECT_CLASS(menu_model_internal_parent_class)->finalize(object);

            for (auto& pair : *self->id_to_widget)
                g_object_unref(pair.second);

            delete self->id_to_widget;
            delete self->submenus;
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(MenuModelInternal, menu_model_internal, MENU_MODEL_INTERNAL)

        static MenuModelInternal* menu_model_internal_new()
        {
            auto* self = (MenuModelInternal*) g_object_new(menu_model_internal_get_type(), nullptr);
            menu_model_internal_init(self);
            self->native = g_menu_new();
            self->id_to_widget = new std::map<std::string, GtkWidget*>();
            self->submenus = new std::set<const MenuModel*>();
            self->has_widget_in_toplevel = false;
            return self;
        }
    }
    
    MenuModel::MenuModel()
        : CTOR_SIGNAL(MenuModel, items_changed)
    {
        _internal = detail::menu_model_internal_new();
        detail::attach_ref_to(G_OBJECT(_internal->native), _internal);
        g_object_ref(_internal);
    }

    MenuModel::MenuModel(detail::MenuModelInternal* internal)
        : CTOR_SIGNAL(MenuModel, items_changed)
    {
        _internal = g_object_ref(internal);
    }

    MenuModel::~MenuModel()
    {
        g_object_unref(_internal);
    }

    NativeObject MenuModel::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void MenuModel::add_action(const std::string& label, const Action& action)
    {
        if (action.get_is_stateful())
        {
            auto* item = g_menu_item_new(label.c_str(), ("app." + action.get_id()).c_str());
            g_menu_item_set_attribute_value(item, "use-markup", g_variant_new_string("yes"));
            g_menu_item_set_attribute_value(item, "target", g_variant_new_boolean(action.get_state()));
            g_menu_append_item(_internal->native, item);
            g_object_unref(item);
        }
        else
        {
            auto* item = g_menu_item_new(label.c_str(), ("app." + action.get_id()).c_str());
            g_menu_item_set_attribute_value(item, "use-markup", g_variant_new_string("yes"));
            g_menu_append_item(_internal->native, item);
            g_object_unref(item);
        }
    }

    void MenuModel::add_widget(const Widget& widget)
    {
        auto id = std::to_string(current_id++);
        auto* item = g_menu_item_new(id.c_str(), id.c_str());
        g_menu_item_set_attribute_value(item, "custom", g_variant_new_string(id.c_str()));
        g_menu_append_item(_internal->native, item);

        _internal->id_to_widget->insert({std::string(id), g_object_ref(widget.operator NativeWidget())});
        _internal->has_widget_in_toplevel = true;
        g_object_unref(item);
    }

    void MenuModel::add_icon(const Icon& icon, const Action& action)
    {
        auto* item = g_menu_item_new(nullptr, ("app." + action.get_id()).c_str());

        if (icon.operator GIcon*() == nullptr)
        {
            //log::warning("In MenuModel::add_icon: Icon was not yet initialized, this will cause the menu item to appear empty");
            g_menu_item_set_attribute_value(item, "verb-icon", nullptr);
            g_menu_item_set_attribute_value(item, "icon", nullptr);
        }
        else
        {
            g_menu_item_set_attribute_value(item, "verb-icon", g_icon_serialize(icon.operator GIcon*()));
            g_menu_item_set_attribute_value(item, "icon", g_icon_serialize(icon.operator GIcon*()));
        }

        g_menu_append_item(_internal->native, item);
        g_object_unref(item);
    }

    void MenuModel::add_section(const std::string& label, const MenuModel& model, SectionFormat format)
    {
        auto* item = g_menu_item_new_section((label).c_str(), G_MENU_MODEL(model._internal->native));

        if (format == HORIZONTAL_BUTTONS)
        {
            g_menu_item_set_attribute_value(item, "display-hint", g_variant_new_string("horizontal-buttons"));
        }
        else if (format == HORIZONTAL_BUTTONS_LEFT_TO_RIGHT)
        {
            g_menu_item_set_attribute_value(item, "display-hint", g_variant_new_string("horizontal-buttons"));
            g_menu_item_set_attribute_value(item, "text-direction", g_variant_new_string("ltr"));
        }
        else if (format == HORIZONTAL_BUTTONS_RIGHT_TO_LEFT)
        {
            g_menu_item_set_attribute_value(item, "display-hint", g_variant_new_string("horizontal-buttons"));
            g_menu_item_set_attribute_value(item, "text-direction", g_variant_new_string("rtl"));
        }
        else if (format == CIRCULAR_BUTTONS)
        {
            g_menu_item_set_attribute_value(item, "display-hint", g_variant_new_string("circular-buttons"));
        }
        else if (format == INLINE_BUTTONS)
        {
            g_menu_item_set_attribute_value(item, "display-hint", g_variant_new_string("inline-buttons"));
        }
        else
        {
            // noop
        }

        _internal->submenus->insert(&model);
        g_menu_append_item(_internal->native, item);
    }

    void MenuModel::add_submenu(const std::string& label, const MenuModel& model)
    {
        if (model._internal->submenus->find(this) != model._internal->submenus->end())
        {
            std::stringstream str;
            str << "In MenuModel::add_submenu: Trying to add menu " << &model << " to " << this << ", even though " << this << " is already a submenu of " << &model << ". This will create an infinite loop on initialization." << std::endl;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
        }

        _internal->submenus->insert(&model);

        auto* item = g_menu_item_new_submenu(label.c_str(), G_MENU_MODEL(model._internal->native));
        g_menu_append_item(_internal->native, item);
    }

    MenuModel::operator GMenuModel*() const
    {
        return G_MENU_MODEL(_internal->native);
    }

    MenuModel::operator NativeObject() const
    {
        return G_OBJECT(_internal->native);
    }

    std::map<std::string, GtkWidget*> MenuModel::get_widgets() const
    {
        auto out = std::map<std::string, GtkWidget*>(*_internal->id_to_widget);

        for (auto* submenu : (*_internal->submenus))
            for (auto& pair : submenu->get_widgets())
                out.insert(pair);

        return out;
    }
}