//
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#include <include/drop_down.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        #define G_TYPE_DROP_DOWN_ITEM (drop_down_item_get_type())
        G_DECLARE_FINAL_TYPE (DropDownItem, drop_down_item, G, DROP_DOWN_ITEM, GObject)

        struct _DropDownItem
        {
            GObject parent_instance;

            size_t id;

            Widget* list_widget;
            Widget* label_widget;

            DropDown* owner;
            std::function<void()>* function;
        };

        struct _DropDownItemClass
        {
            GObjectClass parent_class;
        };

        G_DEFINE_TYPE (DropDownItem, drop_down_item, G_TYPE_OBJECT)

        static void drop_down_item_init(DropDownItem* item)
        {
            item->id = size_t(-1);
            item->list_widget = nullptr;
            item->label_widget = nullptr;
            item->function = nullptr;
            item->owner = nullptr;
        }

        static void drop_down_item_finalize (GObject *object)
        {
            auto* self = G_DROP_DOWN_ITEM(object);
            delete self->function;
            G_OBJECT_CLASS(drop_down_item_parent_class)->finalize(object);
        }

        static void drop_down_item_class_init(DropDownItemClass* klass)
        {
            GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
            gobject_class->finalize = drop_down_item_finalize;
        }

        // sic, not static because forward declared in dropdown.inl
        DropDownItem* drop_down_item_new(size_t id, Widget* in, Widget* label, DropDown* owner, std::function<void()> f)
        {
            auto* item = (DropDownItem*) g_object_new(G_TYPE_DROP_DOWN_ITEM, nullptr);
            drop_down_item_init(item);

            item->id = id;
            item->list_widget = in;
            item->label_widget = label;
            item->owner = owner;
            item->function = new std::function<void()>(f);

            return item;
        }
    }

    DropDown::DropDown()
        : WidgetImplementation<GtkDropDown>(GTK_DROP_DOWN(gtk_drop_down_new(nullptr, nullptr)))
    {
        _list_factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        g_signal_connect(_list_factory, "bind", G_CALLBACK(on_list_factory_bind), this);
        g_signal_connect(_list_factory, "unbind", G_CALLBACK(on_list_factory_unbind), this);

        _label_factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        g_signal_connect(_label_factory, "bind", G_CALLBACK(on_label_factory_bind), this);
        g_signal_connect(_label_factory, "unbind", G_CALLBACK(on_label_factory_unbind), this);

        _model = g_list_store_new(G_TYPE_OBJECT);

        gtk_drop_down_set_model(get_native(), G_LIST_MODEL(_model));
        gtk_drop_down_set_factory(get_native(), GTK_LIST_ITEM_FACTORY(_label_factory));
        gtk_drop_down_set_list_factory(get_native(), GTK_LIST_ITEM_FACTORY(_list_factory));

        g_object_ref(_model);
        g_object_ref(_list_factory);
        g_object_ref(_label_factory);

        g_signal_connect(get_native(), "notify::selected-item", G_CALLBACK(on_selected_item_changed), this);
    }

    DropDown::~DropDown()
    {
        g_object_unref(_model);
        g_object_unref(_list_factory);
        g_object_unref(_label_factory);
    }

    void DropDown::on_list_factory_bind(GtkSignalListItemFactory* self, void* object, DropDown*)
    {
        auto* item = GTK_LIST_ITEM(object);
        auto* dropdown_item = detail::G_DROP_DOWN_ITEM(gtk_list_item_get_item(item));
        gtk_list_item_set_child(item, dropdown_item->list_widget == nullptr ? nullptr : dropdown_item->list_widget->operator GtkWidget *());
    }

    void DropDown::on_list_factory_unbind(GtkSignalListItemFactory* self, void* object, DropDown*)
    {
        auto* item = GTK_LIST_ITEM(object);
        gtk_list_item_set_child(item, nullptr);
    }

    void DropDown::on_label_factory_bind(GtkSignalListItemFactory* self, void* object, DropDown*)
    {
        auto* item = GTK_LIST_ITEM(object);
        auto* dropdown_item = detail::G_DROP_DOWN_ITEM(gtk_list_item_get_item(item));
        gtk_list_item_set_child(item, dropdown_item->label_widget == nullptr ? nullptr : dropdown_item->label_widget->operator GtkWidget *());
    }

    void DropDown::on_label_factory_unbind(GtkSignalListItemFactory* self, void* object, DropDown*)
    {
        auto* item = GTK_LIST_ITEM(object);
        gtk_list_item_set_child(item, nullptr);
    }

    void DropDown::on_selected_item_changed(GtkDropDown* self, void*, DropDown* instance)
    {
        auto i = gtk_drop_down_get_selected(self);
        auto* item = detail::G_DROP_DOWN_ITEM(g_list_model_get_item(G_LIST_MODEL(instance->_model), i));
        if (item->function != nullptr)
            (*item->function)();
    }

    void DropDown::remove(ItemID id)
    {
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_model)); ++i)
        {
            auto* item = detail::G_DROP_DOWN_ITEM(g_list_model_get_item(G_LIST_MODEL(_model), i));
            if (item->id == id._value)
            {
                g_list_store_remove(_model, i);
                return;
            }
        }
    }

    void DropDown::set_show_arrow(bool b)
    {
        gtk_drop_down_set_show_arrow(get_native(), b);
    }

    bool DropDown::get_show_arrow() const
    {
        return gtk_drop_down_get_show_arrow(get_native());
    }

    void DropDown::set_selected(DropDown::ItemID id)
    {
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_model)); ++i)
        {
            auto* item = detail::G_DROP_DOWN_ITEM(g_list_model_get_item(G_LIST_MODEL(_model), i));
            if (item->id == id._value)
            {
                gtk_drop_down_set_selected(get_native(), i);
                return;
            }
        }

        log::critical("In DropDown::set_selected: No item with ID " + std::to_string(id._value), MOUSETRAP_DOMAIN);
    }

    DropDown::ItemID DropDown::get_selected() const
    {
        auto* item = g_list_model_get_item(G_LIST_MODEL(_model), gtk_drop_down_get_selected(get_native()));
        if (item == nullptr)
            return DropDown::ItemID{size_t(-1)};
        else
            return DropDown::ItemID{detail::G_DROP_DOWN_ITEM(item)->id};
    }

    DropDown::ItemID DropDown::get_item_at(size_t index) const
    {
        auto* item = g_list_model_get_item(G_LIST_MODEL(_model), index);
        if (item == nullptr)
            return DropDown::ItemID{size_t(-1)};
        else
            return DropDown::ItemID{detail::G_DROP_DOWN_ITEM(item)->id};
    }

    void DropDown::assert_label_is_not_self(const std::string& scope, Widget* label01, Widget* label02)
    {
        bool log_warning = false;
        if (label01 != nullptr and (label01->operator GtkWidget*() == this->operator GtkWidget*()))
        {
            label01 = nullptr;
            log_warning = true;
        }

        if (label02 != nullptr and (label02->operator GtkWidget*() == this->operator GtkWidget*()))
        {
            label02 = nullptr;
            log_warning = true;
        }

        if (log_warning)
            log::critical("In DropDown::" + scope + ": Attempting to add DropDown parent as its own child, this would cause an infinite loop.");
    }
}