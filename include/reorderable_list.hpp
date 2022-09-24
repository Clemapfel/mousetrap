// 
// Copyright 2022 Clemens Cords
// Created on 9/23/22 by clem (mail@clemens-cords.com)
//

#pragma once

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

            MotionEventController _motion_event_controller;
            static void on_motion_enter(MotionEventController*, double x, double y, ReorderableListView* instance);
            static void on_motion(MotionEventController*, double x, double y, ReorderableListView* instance);
            static void on_motion_leave(MotionEventController*, ReorderableListView* instance);

            ClickEventController _click_event_controller;
            static void on_click_pressed(ClickEventController*, int n_press, double x, double y, ReorderableListView* instance);
            static void on_click_released(ClickEventController*, int n_press, double x, double y, ReorderableListView* instance);

            DragEventController _drag_event_controller;
            static void on_drag_begin(DragEventController*, double x, double y, ReorderableListView* instance);
            static void on_drag_end(DragEventController*, double x, double y, ReorderableListView* instance);
            static void on_drag_update(DragEventController*, double x, double y, ReorderableListView* instance);

            size_t position_to_item_index(double x, double y);
            size_t _currently_being_dragged_index;

            void deposit_dragged_item();

            bool _drag_active = false;
            bool _drag_started = false;
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
        G_OBJECT_CLASS(reorderable_list_item_parent_class)->finalize(object);
    }

    static void reorderable_list_item_init(ReorderableListItem* item)
    {
        item->widget = nullptr;
        item->widget_ref = nullptr;
        item->box = nullptr;
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
        item->box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        gtk_box_append(item->box, item->widget->operator GtkWidget*());
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
    {
        _motion_event_controller.connect_signal_motion_leave(on_motion_leave, this);
        gtk_widget_add_controller(GTK_WIDGET(_native), _motion_event_controller.operator GtkEventController*());

        _click_event_controller.connect_signal_click_released(on_click_released, this);
        gtk_widget_add_controller(GTK_WIDGET(_native), _click_event_controller.operator GtkEventController*());

        _drag_event_controller.connect_signal_drag_begin(on_drag_begin, this);
        _drag_event_controller.connect_signal_drag_update(on_drag_update, this);
        gtk_widget_add_controller(GTK_WIDGET(_native), _drag_event_controller.operator GtkEventController*());
    }

    size_t ReorderableListView::position_to_item_index(double x, double y)
    {
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_list_store)); ++i)
        {
            auto* item = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(_list_store), i);
            double top_left_x, top_left_y;
            gtk_widget_translate_coordinates(GTK_WIDGET(item->box), GTK_WIDGET(_overlay), 0, 0, &top_left_x, &top_left_y);

            double height = gtk_widget_get_allocated_height(GTK_WIDGET(item->box));
            double width = gtk_widget_get_allocated_width(GTK_WIDGET(item->box));

            auto rect = mousetrap::Rectangle{{top_left_x, top_left_y}, {width, height}};
            if (is_point_in_rectangle(Vector2f(x, y), rect))
                return i;
        }

        return size_t(-1);
    }

    /*
    void ReorderableListView::on_motion_enter(MotionEventController*, double x, double y, ReorderableListView* instance)
    {}

    void ReorderableListView::on_motion(MotionEventController*, double x, double y, ReorderableListView* instance)
    {
        gtk_fixed_move(instance->_fixed, GTK_WIDGET(instance->_fixed_box), x, y);
    }

    void ReorderableListView::on_click_pressed(ClickEventController*, int n_press, double x, double y,
                                               ReorderableListView* instance)
    {
        std::cout << "pressed" << std::endl;

        if (instance->_drag_active)
            instance->deposit_dragged_item();

        instance->_drag_active = true;
        gtk_widget_set_cursor_from_name(GTK_WIDGET(instance->_overlay), "grabbing");

        size_t item_index = instance->position_to_item_index(x, y);

        if (item_index == size_t(-1))
            return;

        auto* item = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(instance->_list_store), item_index);
        gtk_box_remove(item->box, item->widget->operator GtkWidget*());
        gtk_box_append(instance->_fixed_box, item->widget->operator GtkWidget*());
        gtk_list_view_set_single_click_activate(instance->_native, true);
        instance->_currently_being_dragged_index = item_index;
    }

    void ReorderableListView::on_click_released(ClickEventController*, int n_press, double x, double y,
                                                ReorderableListView* instance)
    {
        std::cout << "released" << std::endl;
        instance->deposit_dragged_item();
    }
    */

    void ReorderableListView::deposit_dragged_item()
    {
        gtk_widget_set_cursor_from_name(GTK_WIDGET(_overlay), "pointer");

        size_t to_insert_i = 0;
        for (; to_insert_i < g_list_model_get_n_items(G_LIST_MODEL(_list_store)); ++to_insert_i)
            if (gtk_selection_model_is_selected(_selection_model, to_insert_i))
                break;

        auto* current = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(_list_store), _currently_being_dragged_index);

        gtk_box_remove(_fixed_box, current->widget->operator GtkWidget*());
        gtk_widget_unparent(current->widget->operator GtkWidget*());

        auto* item = detail::reorderable_list_item_new(current->widget);

        g_list_store_remove(_list_store, _currently_being_dragged_index);
        g_list_store_insert(_list_store, to_insert_i, item);

        gtk_list_view_set_single_click_activate(_native, false);
        _drag_active = false;
    }


    void ReorderableListView::on_drag_begin(DragEventController* controller, double x, double y, ReorderableListView* instance)
    {
        if (instance->_drag_active)
            instance->deposit_dragged_item();

        instance->_drag_started = true;
    }

    void ReorderableListView::on_drag_end(DragEventController* controller, double x, double y, ReorderableListView* instance)
    {}

    void ReorderableListView::on_click_released(ClickEventController*, int n_press, double x, double y,
                                                ReorderableListView* instance)
    {
        if (instance->_drag_started)
            instance->_drag_started = false;

        else if (instance->_drag_active)
        {
            std::cout << "drag end" << std::endl;
            instance->deposit_dragged_item();
            instance->_drag_active = false;
        }
    }

    void ReorderableListView::on_motion_leave(MotionEventController*, ReorderableListView* instance)
    {
        if (instance->_drag_active)
            instance->deposit_dragged_item();
    }

    void ReorderableListView::on_drag_update(DragEventController* controller, double x, double y, ReorderableListView* instance)
    {
        if (not instance->_drag_started)
            return;

        auto start = controller->get_start_position();
        if (not gtk_drag_check_threshold(GTK_WIDGET(instance->_native), start.x, start.y, x, y))
            return;

        if (not instance->_drag_active)
        {
            instance->_drag_active = true;
            gtk_widget_set_cursor_from_name(GTK_WIDGET(instance->_overlay), "grabbing");

            size_t item_index = instance->position_to_item_index(x, y);

            if (item_index == size_t(-1))
                return;

            auto* item = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(instance->_list_store),
                                                                              item_index);
            gtk_box_remove(item->box, item->widget->operator GtkWidget*());
            gtk_box_append(instance->_fixed_box, item->widget->operator GtkWidget*());
            gtk_list_view_set_single_click_activate(instance->_native, true);
            instance->_currently_being_dragged_index = item_index;
        }
        else
        {
            auto position = controller->get_start_position() + controller->get_current_offset();
            gtk_fixed_move(instance->_fixed, GTK_WIDGET(instance->_fixed_box), position.x, position.y);
        }
    }

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
}