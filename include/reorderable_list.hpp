// 
// Copyright 2022 Clemens Cords
// Created on 9/23/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/geometry.hpp>
#include <include/selection_model.hpp>
#include <include/signal_component.hpp>

namespace mousetrap
{
    namespace detail { struct _ReorderableListItem; }

    class ReorderableListView : public WidgetImplementation<GtkListView>,
        public HasReorderedSignal<ReorderableListView>,
        public HasListItemActivateSignal<ReorderableListView>
    {
        public:
            ReorderableListView(GtkOrientation orientation);

            void push_back(Widget*);
            void push_front(Widget*);
            void insert(Widget*, size_t);
            void remove(size_t);

            Widget* get_widget_at(size_t i);
            void set_widget_at(size_t i, Widget*);

            void set_show_separators(bool);
            void set_orientation(GtkOrientation);

            SelectionModel* get_selection_model();

        private:
            static void on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance);
            static void on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance);

            GtkListView* _native;
            GtkSignalListItemFactory* _factory;
            GListStore* _list_store;

            SelectionModel* _selection_model;
            GtkOrientation _orientation;

            MotionEventController _motion_event_controller;
            static void on_motion_leave(MotionEventController*, ReorderableListView* instance);

            ClickEventController _click_event_controller;
            static void on_click_released(ClickEventController*, int n_press, double x, double y, ReorderableListView* instance);

            DragEventController _drag_event_controller;
            static void on_drag_begin(DragEventController*, double x, double y, ReorderableListView* instance);
            static void on_drag_update(DragEventController*, double x, double y, ReorderableListView* instance);

            size_t position_to_item_index(double x, double y);

            bool _drag_started = false;
            bool _drag_active = false;

            void start_drag(size_t to_drag_i);
            void end_drag();

            size_t _currently_being_dragged_item_i = -1;
    };
}


