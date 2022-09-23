// 
// Copyright 2022 Clemens Cords
// Created on 9/23/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    struct _ReorderableListItem;
    static void on_reorderable_list_item_click_pressed(ClickEventController*, int n_press, double x, double y, _ReorderableListItem* instance);

    class ReorderableListView : public WidgetImplementation<GtkOverlay>
    {
        friend void on_reorderable_list_item_click_pressed(ClickEventController*, int, double, double, _ReorderableListItem*);

        public:
            ReorderableListView(GtkOrientation orientation, GtkSelectionMode mode = GTK_SELECTION_SINGLE);
            operator GtkListView*();

            void push_back(Widget*);
            
        private:
            static void on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance);

            GtkListView* _native;
            GtkSignalListItemFactory* _factory;
            GListStore* _list_store;

            GtkSelectionModel* _selection_model;
            GtkSelectionMode _selection_mode;
            GtkOrientation _orientation;

            static void on_motion_move(MotionEventController*, double, double, ReorderableListView*);
            MotionEventController _motion_event_controller;

            static void on_click_released(ClickEventController*, int, double, double, ReorderableListView*);
            ClickEventController _click_event_controller;

            GtkFixed* _fixed;
            GtkBox* _fixed_overlay_box;
            GtkOverlay* _overlay;

            void set_drag_active(bool);
            _ReorderableListItem* _current_item;
            size_t _to_insert_i = 0;
    };
}

namespace mousetrap
{
    #define G_TYPE_REORDERABLE_LIST_ITEM (reorderable_list_item_get_type())

    G_DECLARE_FINAL_TYPE (ReorderableListItem, reorderable_list_item, G, REORDERABLE_LIST_ITEM, GObject)

    struct _ReorderableListItem
    {
        GObject parent_instance;
        
        Widget* widget;
        GtkWidget* widget_ref;

        GtkBox* box;
        ClickEventController* click_event_controller;
        ReorderableListView* owner;
    };

    struct _ReorderableListItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (ReorderableListItem, reorderable_list_item, G_TYPE_OBJECT)

    static void reorderable_list_item_finalize (GObject *object)
    {
        auto* self = G_REORDERABLE_LIST_ITEM(object);
        g_object_unref(self->widget_ref);

        delete self->box;
        delete self->click_event_controller;

        G_OBJECT_CLASS (reorderable_list_item_parent_class)->finalize(object);
    }

    static void reorderable_list_item_init(ReorderableListItem* item)
    {
        item->widget = nullptr;
        item->widget_ref = nullptr;
        item->click_event_controller = nullptr;
        item->box = nullptr;
    }

    static void reorderable_list_item_class_init(ReorderableListItemClass* klass)
    {
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
        gobject_class->finalize = reorderable_list_item_finalize;
    }

    static void on_reorderable_list_item_click_pressed(ClickEventController*, int n_press, double x, double y, ReorderableListItem* instance)
    {
        instance->owner->set_drag_active(true);
        gtk_box_remove(instance->box, instance->widget->operator GtkWidget*());
        gtk_box_append(GTK_BOX(instance->owner->_fixed_overlay_box), instance->widget->operator GtkWidget*());
        instance->owner->_current_item = instance;
        instance->owner->_to_insert_i = 0;
    }

    static ReorderableListItem* reorderable_list_item_new(Widget* in, ReorderableListView* owner)
    {
        auto* item = (ReorderableListItem*) g_object_new(G_TYPE_REORDERABLE_LIST_ITEM, nullptr);
        reorderable_list_item_init(item);
        item->widget = in;
        item->widget_ref = g_object_ref(in->operator GtkWidget*());
        item->box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        gtk_box_append(item->box, item->widget->operator GtkWidget*());
        item->owner = owner;
        item->click_event_controller = new ClickEventController();
        item->click_event_controller->connect_signal_click_pressed(on_reorderable_list_item_click_pressed, item);

        gtk_widget_add_controller(GTK_WIDGET(item->box), item->click_event_controller->operator GtkEventController*());
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
        g_signal_connect(_factory, "unbind", G_CALLBACK(on_list_item_factory_unbind), this);
        g_signal_connect(_factory, "setup", G_CALLBACK(on_list_item_factory_setup), this);
        g_signal_connect(_factory, "teardown", G_CALLBACK(on_list_item_factory_teardown), this);

        if (mode == GTK_SELECTION_MULTIPLE)
            _selection_model = GTK_SELECTION_MODEL(gtk_multi_selection_new(G_LIST_MODEL(_list_store)));
        else if (mode == GTK_SELECTION_SINGLE or mode == GTK_SELECTION_BROWSE)
            _selection_model = GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(_list_store)));
        else if (mode == GTK_SELECTION_NONE)
            _selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(G_LIST_MODEL(_list_store)));

        _native = GTK_LIST_VIEW(gtk_list_view_new(_selection_model, GTK_LIST_ITEM_FACTORY(_factory)));
        gtk_orientable_set_orientation(GTK_ORIENTABLE(_native), orientation);

        _fixed = GTK_FIXED(gtk_fixed_new());
        _fixed_overlay_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        _overlay = GTK_OVERLAY(gtk_overlay_new());
        gtk_overlay_set_child(_overlay, GTK_WIDGET(_native));
        gtk_overlay_add_overlay(_overlay, GTK_WIDGET(_fixed));
        gtk_widget_set_visible(GTK_WIDGET(_fixed), false);
        gtk_fixed_put(_fixed, GTK_WIDGET(_fixed_overlay_box), 0, 0);

        return _overlay;
    }())
    {
        _motion_event_controller.connect_signal_motion(on_motion_move, this);
        gtk_widget_add_controller(GTK_WIDGET(_overlay), _motion_event_controller.operator GtkEventController *());

        _click_event_controller.connect_signal_click_released(on_click_released, this);
        gtk_widget_add_controller(GTK_WIDGET(_overlay), _click_event_controller.operator GtkEventController *());
    }

    void ReorderableListView::on_motion_move(MotionEventController*, double x, double y, ReorderableListView* instance)
    {
        gtk_fixed_move(instance->_fixed, GTK_WIDGET(instance->_fixed_overlay_box), x, y);
    }

    void ReorderableListView::on_click_released(ClickEventController*, int, double, double, ReorderableListView* instance)
    {
        instance->set_drag_active(false);
        gtk_box_remove(instance->_fixed_overlay_box, instance->_current_item->widget->operator GtkWidget*());
        gtk_box_append(instance->_current_item->box, instance->_current_item->widget->operator GtkWidget*());
        g_list_store_insert(instance->_list_store, instance->_to_insert_i, instance->_current_item);
    }

    ReorderableListView::operator GtkListView*()
    {
        return _native;
    }

    void ReorderableListView::set_drag_active(bool b)
    {
        gtk_widget_set_visible(GTK_WIDGET(_fixed), b);
    }

    void ReorderableListView::on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* object_in = G_REORDERABLE_LIST_ITEM(gtk_list_item_get_item(list_item));

        gtk_list_item_set_child(list_item, GTK_WIDGET(object_in->box));
    }

    void ReorderableListView::on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance)
    {}

    void ReorderableListView::on_list_item_factory_setup(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance)
    {}

    void ReorderableListView::on_list_item_factory_teardown(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance)
    {}

    void ReorderableListView::push_back(Widget* widget)
    {
        auto* item = reorderable_list_item_new(widget, this);
        g_list_store_append(_list_store, item);
    }
}