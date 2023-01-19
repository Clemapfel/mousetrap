// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#include <include/dropdown.hpp>

namespace mousetrap
{
    namespace detail
    {
        #define G_TYPE_DROP_DOWN_ITEM (drop_down_item_get_type())
        G_DECLARE_FINAL_TYPE (DropDownItem, drop_down_item, G, DROP_DOWN_ITEM, GObject)

        struct _DropDownItem
        {
            GObject parent_instance;

            Widget* widget;
            GtkWidget* widget_ref;

            Widget* label_widget;
            GtkWidget* label_widget_ref;

            DropDown* owner;
            size_t function_id;
        };

        struct _DropDownItemClass
        {
            GObjectClass parent_class;
        };

        G_DEFINE_TYPE (DropDownItem, drop_down_item, G_TYPE_OBJECT)

        static void drop_down_item_finalize (GObject *object)
        {
            auto* self = G_DROP_DOWN_ITEM(object);
            g_object_unref(self->widget_ref);
            g_object_unref(self->label_widget_ref);

            self->owner->_functions.erase(self->function_id);

            G_OBJECT_CLASS(drop_down_item_parent_class)->finalize(object);
        }

        static void drop_down_item_init(DropDownItem* item)
        {
            item->widget = nullptr;
            item->widget_ref = nullptr;
            item->label_widget = nullptr;
            item->label_widget_ref = nullptr;
            item->function_id = -1;
            item->owner = nullptr;
        }

        static void drop_down_item_class_init(DropDownItemClass* klass)
        {
            GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
            gobject_class->finalize = drop_down_item_finalize;
        }

        // sic, not static because forward declared in dropdown.inl
        DropDownItem* drop_down_item_new(Widget* in, Widget* label, DropDown* owner, size_t function_id)
        {
            auto* item = (DropDownItem*) g_object_new(G_TYPE_DROP_DOWN_ITEM, nullptr);
            drop_down_item_init(item);
            item->widget = in;
            item->widget_ref = g_object_ref(in->operator GtkWidget*());
            item->label_widget = label;
            item->label_widget_ref = g_object_ref(label->operator GtkWidget*());
            item->owner = owner;
            item->function_id = function_id;

            return item;
        }
    }

    DropDown::DropDown()
            : WidgetImplementation<GtkDropDown>([&](){

        _model = g_list_store_new(G_TYPE_OBJECT);

        _list_factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        g_signal_connect(_list_factory, "bind", G_CALLBACK(on_list_factory_bind), this);

        _label_factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        g_signal_connect(_label_factory, "bind", G_CALLBACK(on_label_factory_bind), this);

        auto* out = GTK_DROP_DOWN(gtk_drop_down_new(G_LIST_MODEL(_model), nullptr));
        gtk_drop_down_set_factory(out, GTK_LIST_ITEM_FACTORY(_label_factory));
        gtk_drop_down_set_list_factory(out, GTK_LIST_ITEM_FACTORY(_list_factory));

        return out;
    }()), HasActivateSignal<DropDown>(this)
    {
        add_reference(_list_factory);
        add_reference(_label_factory);
        add_reference(_model);
    }

    void DropDown::on_list_factory_bind(GtkSignalListItemFactory* self, void* object, void* data)
    {
        auto* item = GTK_LIST_ITEM(object);
        auto* dropdown_item = detail::G_DROP_DOWN_ITEM(gtk_list_item_get_item(item));
        gtk_list_item_set_child(item, dropdown_item->widget->operator GtkWidget*());
    }

    void DropDown::on_label_factory_bind(GtkSignalListItemFactory* self, void* object, void* data)
    {
        auto* item = GTK_LIST_ITEM(object);
        auto* dropdown_item = detail::G_DROP_DOWN_ITEM(gtk_list_item_get_item(item));
        gtk_list_item_set_child(item, dropdown_item->label_widget->operator GtkWidget*());

        auto& f = dropdown_item->owner->_functions.at(dropdown_item->function_id);
        if (f != nullptr)
            f();
    }

    size_t DropDown::get_selected()
    {
        return gtk_drop_down_get_selected(get_native());
    }

    void DropDown::set_selected(size_t i)
    {
        gtk_drop_down_set_selected(get_native(), i);
    }
}