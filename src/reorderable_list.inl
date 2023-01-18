//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/18/23
//
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
    inline ReorderableListView::ReorderableListView(GtkOrientation orientation)
            : WidgetImplementation<GtkListView>([&]() -> GtkListView*
                                                {
                                                    _orientation = orientation;
                                                    _list_store = g_list_store_new(G_TYPE_OBJECT);
                                                    _factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
                                                    g_signal_connect(_factory, "bind", G_CALLBACK(on_list_item_factory_bind), this);
                                                    g_signal_connect(_factory, "unbind", G_CALLBACK(on_list_item_factory_unbind), this);

                                                    _selection_model = new SingleSelectionModel(G_LIST_MODEL(_list_store));
                                                    _native = GTK_LIST_VIEW(gtk_list_view_new(_selection_model->operator GtkSelectionModel*(), GTK_LIST_ITEM_FACTORY(_factory)));
                                                    gtk_orientable_set_orientation(GTK_ORIENTABLE(_native), orientation);

                                                    return _native;

                                                }()), HasReorderedSignal<ReorderableListView>(this), HasListItemActivateSignal<ReorderableListView>(this)
    {
        _motion_event_controller.connect_signal_motion_leave(on_motion_leave, this);
        gtk_widget_add_controller(GTK_WIDGET(_native), _motion_event_controller.operator GtkEventController*());

        _click_event_controller.connect_signal_click_released(on_click_released, this);
        _click_event_controller.set_propagation_phase(GTK_PHASE_CAPTURE);
        gtk_widget_add_controller(GTK_WIDGET(_native), _click_event_controller.operator GtkEventController*());

        _drag_event_controller.connect_signal_drag_begin(on_drag_begin, this);
        _drag_event_controller.connect_signal_drag_update(on_drag_update, this);
        _drag_event_controller.set_propagation_phase(GTK_PHASE_BUBBLE);
        gtk_widget_add_controller(GTK_WIDGET(_native), _drag_event_controller.operator GtkEventController*());
    }

    inline size_t ReorderableListView::position_to_item_index(double x, double y)
    {
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_list_store)); ++i)
        {
            auto* item = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(_list_store), i);
            double top_left_x, top_left_y;
            gtk_widget_translate_coordinates(GTK_WIDGET(item->box), GTK_WIDGET(_native), 0, 0, &top_left_x, &top_left_y);

            double height = gtk_widget_get_allocated_height(GTK_WIDGET(item->box));
            double width = gtk_widget_get_allocated_width(GTK_WIDGET(item->box));

            auto rect = mousetrap::Rectangle{{top_left_x, top_left_y}, {width, height}};
            if (is_point_in_rectangle(Vector2f(x, y), rect))
                return i;
        }

        return size_t(-1);
    }

    inline SelectionModel* ReorderableListView::get_selection_model()
    {
        return _selection_model;
    }

    inline void ReorderableListView::on_drag_begin(DragEventController* controller, double x, double y, ReorderableListView* instance)
    {
        instance->_drag_started = true;
    }

    inline void ReorderableListView::start_drag(size_t item_i)
    {
        _drag_started = false;
        _drag_active = true;

        auto* item = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(_list_store), item_i);
        item->widget->set_opacity(0.25);

        _currently_being_dragged_item_i = item_i;

        _selection_model->unselect_all();
        gtk_list_view_set_single_click_activate(_native, true);
    }

    inline void ReorderableListView::on_drag_update(DragEventController* controller, double x, double y, ReorderableListView* instance)
    {
        auto start = controller->get_start_position();
        auto offset = controller->get_current_offset();

        if (instance->_drag_started and not instance->_drag_active)
        {
            if (gtk_drag_check_threshold(GTK_WIDGET(instance->_native), start.x, start.y, start.x + offset.x, start.y + offset.y))
            {
                auto item_i = instance->position_to_item_index(start.x + offset.x, start.y + offset.y);
                if (item_i == size_t(-1))
                {
                    return; // try again when in bounds
                }

                instance->start_drag(item_i);
            }
        }

        if (instance->_drag_active)
        {
            size_t item_i = instance->position_to_item_index(start.x + offset.x, start.y + offset.y);
            if (item_i != size_t(-1))
                instance->_selection_model->select(item_i, true);
        }
    }

    inline void ReorderableListView::end_drag()
    {
        auto* current = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(_list_store), _currently_being_dragged_item_i);

        gtk_box_remove(current->box, current->widget->operator GtkWidget*());
        auto* item = detail::reorderable_list_item_new(current->widget);
        item->widget->set_opacity(1);

        auto selected = _selection_model->get_selection();
        size_t target_i;
        if (selected.size() == 0) // triggered if drag initiated but cursor isn't moved to a select a new item
            target_i = _currently_being_dragged_item_i;
        else
            target_i = selected.at(0);

        g_list_store_remove(_list_store, _currently_being_dragged_item_i);
        g_list_store_insert(_list_store, target_i, item);

        emit_signal_reordered(item->widget, _currently_being_dragged_item_i, target_i);

        _currently_being_dragged_item_i = -1;
        _selection_model->select(target_i, true);
        gtk_list_view_set_single_click_activate(_native, false);

        _drag_active = false;
        _drag_started = false;
    }

    inline void ReorderableListView::on_click_released(ClickEventController*, int n_press, double x, double y,
            ReorderableListView* instance)
    {
        if (instance->_drag_started)
        {
            instance->_drag_started = false;
            return;
        }

        if (instance->_drag_active)
        {
            instance->end_drag();
            return;
        }
    }

    inline void ReorderableListView::on_motion_leave(MotionEventController*, ReorderableListView* instance)
    {
        if (instance->_drag_started)
        {
            instance->_drag_started = false;
            return;
        }

        if (instance->_drag_active)
        {
            instance->end_drag();
            return;
        }
    }

    inline void ReorderableListView::on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, ReorderableListView* instance)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* object_in = detail::G_REORDERABLE_LIST_ITEM(gtk_list_item_get_item(list_item));

        gtk_list_item_set_child(list_item, GTK_WIDGET(object_in->box));
    }

    inline void ReorderableListView::on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object,
            ReorderableListView* instance)
    {}

    inline void ReorderableListView::push_back(Widget* widget)
    {
        if (_drag_active)
            end_drag();

        auto* item = detail::reorderable_list_item_new(widget);
        g_list_store_append(_list_store, item);
    }

    inline void ReorderableListView::push_front(Widget* widget)
    {
        if (_drag_active)
            end_drag();

        auto* item = detail::reorderable_list_item_new(widget);
        g_list_store_insert(_list_store, 0, item);
    }

    inline void ReorderableListView::insert(Widget* widget, size_t i)
    {
        if (_drag_active)
            end_drag();

        auto* item = detail::reorderable_list_item_new(widget);
        g_list_store_insert(_list_store, i, item);
    }

    inline void ReorderableListView::remove(size_t i)
    {
        if (_drag_active)
            end_drag();

        g_list_store_remove(_list_store, i);
    }

    inline Widget* ReorderableListView::get_widget_at(size_t i)
    {
        auto* item = (detail::ReorderableListItem*) g_list_model_get_item(G_LIST_MODEL(_list_store), i);
        return item->widget;
    }

    inline void ReorderableListView::set_show_separators(bool b)
    {
        gtk_list_view_set_show_separators(_native, b);
    }

    inline void ReorderableListView::set_orientation(GtkOrientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(_native), orientation);
    }
}
