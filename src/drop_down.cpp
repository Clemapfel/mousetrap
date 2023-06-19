//
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/drop_down.hpp>
#include <mousetrap/log.hpp>

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

            GtkWidget* list_widget;
            GtkWidget* label_widget;

            std::function<void(DropDown&)>* function;
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
        }

        static void drop_down_item_finalize (GObject *object)
        {
            auto* self = G_DROP_DOWN_ITEM(object);
            G_OBJECT_CLASS(drop_down_item_parent_class)->finalize(object);

            delete self->function;
            g_object_unref(self->list_widget);
            g_object_unref(self->label_widget);
        }

        static void drop_down_item_class_init(DropDownItemClass* klass)
        {
            GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
            gobject_class->finalize = drop_down_item_finalize;
        }

        // sic, not static because forward declared in dropdown.inl
        DropDownItem* drop_down_item_new(size_t id, const Widget* in, const Widget* label, std::function<void(DropDown&)> f)
        {
            auto* item = (DropDownItem*) g_object_new(G_TYPE_DROP_DOWN_ITEM, nullptr);
            drop_down_item_init(item);

            item->id = id;
            item->list_widget = g_object_ref(in->operator NativeWidget());
            item->label_widget = g_object_ref(label->operator NativeWidget());
            item->function = new std::function<void(DropDown&)>(std::move(f));

            return item;
        }
    }

    namespace detail
    {
        DECLARE_NEW_TYPE(DropDownInternal, drop_down_internal, DROP_DOWN_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(DropDownInternal, drop_down_internal, DROP_DOWN_INTERNAL)

        static void drop_down_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_DROP_DOWN_INTERNAL(object);
            G_OBJECT_CLASS(drop_down_internal_parent_class)->finalize(object);

            g_object_unref(self->list_factory);
            g_object_unref(self->label_factory);
            g_object_unref(self->model);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(DropDownInternal, drop_down_internal, DROP_DOWN_INTERNAL)

        static DropDownInternal* drop_down_internal_new(GtkDropDown* native)
        {
            auto* self = (DropDownInternal*) g_object_new(drop_down_internal_get_type(), nullptr);
            drop_down_internal_init(self);

            self->native = native;
            self->list_factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
            self->label_factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());;
            self->model = g_list_store_new(G_TYPE_OBJECT);

            g_object_ref(self->list_factory);
            g_object_ref(self->label_factory);
            g_object_ref(self->model);

            return self;
        }
    }

    DropDown::DropDown()
        : Widget(gtk_drop_down_new(nullptr, nullptr))
    {
        _internal = detail::drop_down_internal_new(GTK_DROP_DOWN(Widget::operator NativeWidget()));
        detail::attach_ref_to(G_OBJECT(_internal->native), _internal);
        g_object_ref(_internal);

        g_signal_connect(_internal->list_factory, "bind", G_CALLBACK(on_list_factory_bind), _internal);
        g_signal_connect(_internal->list_factory, "teardown", G_CALLBACK(on_list_factory_teardown), _internal);

        g_signal_connect(_internal->label_factory, "bind", G_CALLBACK(on_label_factory_bind), _internal);
        g_signal_connect(_internal->label_factory, "teardown", G_CALLBACK(on_label_factory_teardown), _internal);

        gtk_drop_down_set_model(_internal->native, G_LIST_MODEL(_internal->model));
        gtk_drop_down_set_factory(_internal->native, GTK_LIST_ITEM_FACTORY(_internal->label_factory));
        gtk_drop_down_set_list_factory(_internal->native, GTK_LIST_ITEM_FACTORY(_internal->list_factory));

        g_signal_connect(_internal->native, "notify::selected-item", G_CALLBACK(on_selected_item_changed), _internal);
    }
    
    DropDown::DropDown(detail::DropDownInternal* internal)
        : Widget(GTK_WIDGET(internal->native))
    {
        _internal = g_object_ref(internal);
    }

    DropDown::~DropDown()
    {
        g_object_unref(_internal);
    }

    NativeObject DropDown::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void DropDown::on_list_factory_bind(GtkSignalListItemFactory* self, void* object, detail::DropDownInternal* internal)
    {
        auto* item = GTK_LIST_ITEM(object);
        auto* dropdown_item = detail::G_DROP_DOWN_ITEM(gtk_list_item_get_item(item));
        gtk_list_item_set_child(item, dropdown_item->list_widget);
    }

    void DropDown::on_list_factory_teardown(GtkSignalListItemFactory* self, void* object, detail::DropDownInternal* internal)
    {
        // noop
    }

    void DropDown::on_label_factory_bind(GtkSignalListItemFactory* self, void* object, detail::DropDownInternal* internal)
    {
        auto* item = GTK_LIST_ITEM(object);
        auto* dropdown_item = detail::G_DROP_DOWN_ITEM(gtk_list_item_get_item(item));
        gtk_list_item_set_child(item, dropdown_item->label_widget);
    }

    void DropDown::on_label_factory_teardown(GtkSignalListItemFactory* self, void* object, detail::DropDownInternal* internal)
    {
        // noop
    }

    void DropDown::on_selected_item_changed(GtkDropDown* self, void*, detail::DropDownInternal* internal)
    {
        if (not gtk_widget_get_realized(GTK_WIDGET(self)))
            return;

        auto i = gtk_drop_down_get_selected(self);
        auto* item = detail::G_DROP_DOWN_ITEM(g_list_model_get_item(G_LIST_MODEL(internal->model), i));
        if (item->function != nullptr)
        {
            auto dropdown = DropDown(internal);
            (*item->function)(dropdown);
        }
    }

    void DropDown::remove(ItemID id)
    {
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_internal->model)); ++i)
        {
            auto* item = detail::G_DROP_DOWN_ITEM(g_list_model_get_item(G_LIST_MODEL(_internal->model), i));
            if (item->id == id)
            {
                g_list_store_remove(_internal->model, i);
                return;
            }
        }
    }

    void DropDown::set_always_show_arrow(bool b)
    {
        gtk_drop_down_set_show_arrow(_internal->native, b);
    }

    bool DropDown::get_always_show_arrow() const
    {
        return gtk_drop_down_get_show_arrow(_internal->native);
    }

    void DropDown::set_selected(DropDown::ItemID id)
    {
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_internal->model)); ++i)
        {
            auto* item = detail::G_DROP_DOWN_ITEM(g_list_model_get_item(G_LIST_MODEL(_internal->model), i));
            if (item->id == id)
            {
                gtk_drop_down_set_selected(_internal->native, i);
                return;
            }
        }

        log::critical("In DropDown::set_selected: No item with ID " + std::to_string(id), MOUSETRAP_DOMAIN);
    }

    DropDown::ItemID DropDown::get_selected() const
    {
        auto* item = g_list_model_get_item(G_LIST_MODEL(_internal->model), gtk_drop_down_get_selected(_internal->native));
        if (item == nullptr)
            return DropDown::ItemID{size_t(-1)};
        else
            return DropDown::ItemID{detail::G_DROP_DOWN_ITEM(item)->id};
    }

    DropDown::ItemID DropDown::get_item_at(size_t index) const
    {
        auto* item = g_list_model_get_item(G_LIST_MODEL(_internal->model), index);
        if (item == nullptr)
            return DropDown::ItemID{size_t(-1)};
        else
            return DropDown::ItemID{detail::G_DROP_DOWN_ITEM(item)->id};
    }

    bool DropDown::assert_label_is_not_self(const std::string& scope, const Widget& label01, const Widget& label02)
    {
        bool log_warning = false;
        if (label01.operator NativeWidget() == this->operator NativeWidget())
            log_warning = true;

        if (label02.operator NativeWidget() == this->operator NativeWidget())
            log_warning = true;

        if (log_warning)
        {
            log::critical("In DropDown::" + scope + ": Attempting to add DropDown parent as its own child, this would cause an infinite loop.");
            return true;
        }

        return false;
    }
}