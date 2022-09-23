// 
// Copyright 2022 Clemens Cords
// Created on 9/23/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    namespace detail { struct _ReorderableListItem; }

    class ReorderableListView : public WidgetImplementation<GtkOverlay>
    {
        friend detail::_ReorderableListItem;

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

            void update_list_item_indices();
            MotionEventController _motion_event_controller;
            static void on_motion(MotionEventController*, double x, double y, ReorderableListView* instance);

            size_t _currently_being_moved = -1;
            size_t _current_insert_i = -1;
            bool _drag_active = false;
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

        ReorderableListView* owner;
        size_t position;

        ClickEventController* click_event_controller;
        MotionEventController* motion_event_controller;

        static void on_click_pressed(ClickEventController*, int n_press, double x, double y, ReorderableListItem* instance);
        static void on_click_released(ClickEventController*, int n_press, double x, double y, ReorderableListItem* instance);
        static void on_motion_enter(MotionEventController*, double x, double y, ReorderableListItem* instance);
        static void on_motion(MotionEventController*, double x, double y, ReorderableListItem* instance);
        static void on_motion_leave(MotionEventController*, ReorderableListItem* instance);
    };

    struct _ReorderableListItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (ReorderableListItem, reorderable_list_item, G_TYPE_OBJECT
    )

    static void reorderable_list_item_finalize(GObject* object)
    {
        auto* self = G_REORDERABLE_LIST_ITEM(object);
        g_object_unref(self->widget_ref);

        delete self->box;
        delete self->click_event_controller;
        delete self->motion_event_controller;

        G_OBJECT_CLASS(reorderable_list_item_parent_class)->finalize(object);
    }

    static void reorderable_list_item_init(ReorderableListItem* item)
    {
        item->widget = nullptr;
        item->widget_ref = nullptr;
        item->click_event_controller = nullptr;
        item->motion_event_controller = nullptr;
        item->owner = nullptr;
        item->box = nullptr;

        item->position = size_t(-1);
    }

    static void reorderable_list_item_class_init(ReorderableListItemClass* klass)
    {
        GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
        gobject_class->finalize = reorderable_list_item_finalize;
    }

    static ReorderableListItem* reorderable_list_item_new(Widget* in, ReorderableListView* owner)
    {
        auto* item = (ReorderableListItem*)
        g_object_new(G_TYPE_REORDERABLE_LIST_ITEM, nullptr);
        reorderable_list_item_init(item);
        item->widget = in;
        item->widget_ref = g_object_ref(in->operator GtkWidget*());
        item->box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        gtk_box_append(item->box, item->widget->operator GtkWidget*());
        item->owner = owner;

        item->click_event_controller = new ClickEventController();
        item->click_event_controller->connect_signal_click_pressed(ReorderableListItem::on_click_pressed, item);
        item->click_event_controller->connect_signal_click_released(ReorderableListItem::on_click_released, item);

        item->motion_event_controller = new MotionEventController();
        item->motion_event_controller->connect_signal_motion_enter(ReorderableListItem::on_motion_enter, item);
        item->motion_event_controller->connect_signal_motion_leave(ReorderableListItem::on_motion_leave, item);
        item->motion_event_controller->connect_signal_motion(ReorderableListItem::on_motion, item);

        gtk_widget_add_controller(GTK_WIDGET(item->box), item->click_event_controller->operator GtkEventController*());
        gtk_widget_add_controller(GTK_WIDGET(item->box), item->motion_event_controller->operator GtkEventController*());
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
        gtk_widget_set_visible(GTK_WIDGET(_fixed), false);
        gtk_fixed_put(_fixed, GTK_WIDGET(_fixed_box), 0, 0);

        gtk_widget_set_can_target(GTK_WIDGET(_fixed), false);

        return _overlay;
    }())
    {
        _motion_event_controller.connect_signal_motion(on_motion, this);
        //gtk_widget_add_controller(GTK_WIDGET(_fixed), _motion_event_controller.operator GtkEventController*());
    }

    void ReorderableListView::on_motion(MotionEventController*, double x, double y, ReorderableListView* instance)
    {
        gtk_fixed_move(instance->_fixed, GTK_WIDGET(instance->_fixed_box), x, y);
    }

    void ReorderableListView::on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* object_in = detail::G_REORDERABLE_LIST_ITEM(gtk_list_item_get_item(list_item));

        gtk_list_item_set_child(list_item, GTK_WIDGET(object_in->box));
    }

    void ReorderableListView::push_back(Widget* widget)
    {
        auto* item = detail::reorderable_list_item_new(widget, this);
        g_list_store_append(_list_store, item);
        update_list_item_indices();
    }

    void ReorderableListView::update_list_item_indices()
    {
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_list_store)); ++i)
        {
            auto* item = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(_list_store), i);
            item->position = i;
        }
    }

    void detail::ReorderableListItem::on_click_pressed(ClickEventController*, int n_press, double x, double y,
                                                       ReorderableListItem* instance)
    {
        if (not instance->owner->_drag_active)
        {
            instance->owner->_drag_active = true;
            instance->owner->_currently_being_moved = instance->position;
            gtk_widget_set_visible(GTK_WIDGET(instance->owner->_fixed), true);

            gtk_box_remove(instance->box, instance->widget->operator GtkWidget*());
            gtk_box_append(instance->owner->_fixed_box, instance->widget->operator GtkWidget*());
        }
        else
        {
            auto* list = instance->owner;

            auto* current = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(list->_list_store), list->_currently_being_moved);
            auto* item = detail::reorderable_list_item_new(current->widget, list);

            g_list_store_remove(list->_list_store, list->_currently_being_moved);
            g_list_store_insert(list->_list_store, list->_current_insert_i, item);
            list->update_list_item_indices();

            list->_drag_active = false;
            list->_current_insert_i = size_t(-1);
            list->_currently_being_moved = size_t(-1);
            gtk_widget_set_visible(GTK_WIDGET(instance->owner->_fixed), false);
        }
    }

    void detail::ReorderableListItem::on_click_released(ClickEventController*, int n_press, double x, double y, ReorderableListItem* instance)
    {
        return;

        if (not instance->owner->_drag_active)
            return;

        auto* list = instance->owner;

        auto* current = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(list->_list_store), list->_currently_being_moved);
        auto* item = detail::reorderable_list_item_new(current->widget, list);

        g_list_store_remove(list->_list_store, list->_currently_being_moved);
        g_list_store_insert(list->_list_store, list->_current_insert_i, item);
        list->update_list_item_indices();

        list->_drag_active = false;
        list->_current_insert_i = size_t(-1);
        list->_currently_being_moved = size_t(-1);
        gtk_widget_set_visible(GTK_WIDGET(instance->owner->_fixed), false);
    }

    void detail::ReorderableListItem::on_motion_enter(MotionEventController*, double x, double y,
                                                      ReorderableListItem* instance)
    {
        instance->owner->_current_insert_i = instance->position;
        gtk_fixed_move(instance->owner->_fixed, GTK_WIDGET(instance->owner->_fixed_box), x, y);
    }

    void detail::ReorderableListItem::on_motion_leave(MotionEventController*, ReorderableListItem* instance)
    {}

    void detail::ReorderableListItem::on_motion(MotionEventController*, double x, double y, ReorderableListItem* instance)
    {
        double translated_x;
        double translated_y;

        gtk_widget_translate_coordinates(GTK_WIDGET(instance->box), GTK_WIDGET(instance->owner->_overlay), x, y, &translated_x, &translated_y);
        gtk_fixed_move(instance->owner->_fixed, GTK_WIDGET(instance->owner->_fixed_box), translated_x, translated_y);
    }
}