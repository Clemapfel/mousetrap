//
// Copyright 2022 Clemens Cords
// Created on 9/23/22 by clem (mail@clemens-cords.com)
//

#pragma once

/*

#include <include/widget.hpp>
#include <include/geometry.hpp>

namespace mousetrap
{
    namespace detail { struct _ReorderableListItem; }

    class ReorderableListView : public WidgetImplementation<GtkOverlay>
    {
        public:
            ReorderableListView(GtkOrientation orientation, GtkSelectionMode mode = GTK_SELECTION_SINGLE);

            operator GtkListView*();

            void push_back(Widget*);

        private:
            static void
            on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance);

            GtkListView* _native;
            GtkSignalListItemFactory* _factory;
            GListStore* _list_store;

            GtkSelectionModel* _selection_model;
            GtkSelectionMode _selection_mode;
            GtkOrientation _orientation;

            GtkFixed* _fixed;
            GtkBox* _fixed_box;
            GtkOverlay* _overlay;
    };
}

namespace mousetrap::detail
{
    #define G_TYPE_REORDERABLE_LIST_ITEM (reorderable_list_item_get_type())

    G_DECLARE_FINAL_TYPE (ReorderableListItem, reorderable_list_item, G, REORDERABLE_LIST_ITEM, GObject)

    struct _ReorderableListItem
    {
        GObject parent_instance;

        Widget* widget;
        GtkWidget* widget_ref;

        GtkBox* box;
        GtkLabel* drag_handle;

        ClickEventController* click_event_controller;

        static void on_click_pressed(ClickEventController*, int n_press, double x, double y, _ReorderableListItem* instance);
    };

    struct _ReorderableListItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (ReorderableListItem, reorderable_list_item, G_TYPE_OBJECT)

    static void reorderable_list_item_finalize(GObject* object)
    {
        auto* self = G_REORDERABLE_LIST_ITEM(object);
        g_object_unref(self->widget_ref);
        delete self->box;
        delete self->click_event_controller;

        G_OBJECT_CLASS(reorderable_list_item_parent_class)->finalize(object);
    }

    static void reorderable_list_item_init(ReorderableListItem* item)
    {
        item->widget = nullptr;
        item->widget_ref = nullptr;
        item->box = nullptr;
        item->drag_handle = nullptr;
        item->click_event_controller = nullptr;
    }

    static void reorderable_list_item_class_init(ReorderableListItemClass* klass)
    {
        GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
        gobject_class->finalize = reorderable_list_item_finalize;
    }

    static ReorderableListItem* reorderable_list_item_new(Widget* in)
    {
        auto* item = (ReorderableListItem*)
        g_object_new(G_TYPE_REORDERABLE_LIST_ITEM, nullptr);
        reorderable_list_item_init(item);
        item->widget = in;
        item->widget_ref = g_object_ref(in->operator GtkWidget*());
        item->box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

        item->drag_handle = GTK_LABEL(gtk_label_new("<tt>&#8857;</tt>"));
        gtk_label_set_use_markup(item->drag_handle, true);
        gtk_widget_set_cursor_from_name(GTK_WIDGET(item->drag_handle), "pointer");

        gtk_box_append(item->box, GTK_WIDGET(item->drag_handle));
        gtk_box_append(item->box, item->widget->operator GtkWidget*());

        item->click_event_controller = new ClickEventController();
        item->click_event_controller->connect_signal_click_pressed(ReorderableListItem::on_click_pressed, item);

        return item;
    }
}

namespace mousetrap
{
    ReorderableListView::ReorderableListView(GtkOrientation orientation, GtkSelectionMode mode)
            : _orientation(orientation), _selection_mode(mode), WidgetImplementation<GtkOverlay>([&]() -> GtkOverlay* {

        _list_store = g_list_store_new(G_TYPE_OBJECT);
        _factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        g_signal_connect(_factory, "bind", G_CALLBACK(on_list_item_factory_bind), this);

        if (mode == GTK_SELECTION_MULTIPLE)
            _selection_model = GTK_SELECTION_MODEL(gtk_multi_selection_new(G_LIST_MODEL(_list_store)));
        else if (mode == GTK_SELECTION_SINGLE or mode == GTK_SELECTION_BROWSE)
            _selection_model = GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(_list_store)));
        else if (mode == GTK_SELECTION_NONE)
            _selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(G_LIST_MODEL(_list_store)));

        _native = GTK_LIST_VIEW(gtk_list_view_new(_selection_model, GTK_LIST_ITEM_FACTORY(_factory)));
        gtk_orientable_set_orientation(GTK_ORIENTABLE(_native), orientation);

        _fixed = GTK_FIXED(gtk_fixed_new());
        _fixed_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        _overlay = GTK_OVERLAY(gtk_overlay_new());
        gtk_overlay_set_child(_overlay, GTK_WIDGET(_native));
        gtk_overlay_add_overlay(_overlay, GTK_WIDGET(_fixed));
        gtk_fixed_put(_fixed, GTK_WIDGET(_fixed_box), 0, 0);
        gtk_widget_set_can_target(GTK_WIDGET(_fixed), false);

        gtk_widget_set_cursor_from_name(GTK_WIDGET(_overlay), "grab");

        return _overlay;
    }())
    {}

    void ReorderableListView::on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* object_in = detail::G_REORDERABLE_LIST_ITEM(gtk_list_item_get_item(list_item));

        gtk_list_item_set_child(list_item, GTK_WIDGET(object_in->box));
    }

    void ReorderableListView::push_back(Widget* widget)
    {
        auto* item = detail::reorderable_list_item_new(widget);
        g_list_store_append(_list_store, item);
    }

    void detail::ReorderableListItem::on_click_pressed(ClickEventController*, int n_press, double x, double y,
                                                       _ReorderableListItem* instance)
    {}
}
 */