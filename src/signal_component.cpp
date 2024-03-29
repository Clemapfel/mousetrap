#include <mousetrap/signal_component.hpp>
#include <mousetrap/log.hpp>

#include <iostream>

#define DEFINE_SIGNAL(CamelCase, snake_case, CAPS_CASE, g_signal_id, return_t, ...) \
DECLARE_NEW_TYPE(HasSignal##CamelCase##Internal, has_signal_##snake_case##_internal, HAS_SIGNAL_##CAPS_CASE##_INTERNAL) \
DEFINE_NEW_TYPE_TRIVIAL_INIT(HasSignal##CamelCase##Internal, has_signal_##snake_case##_internal, HAS_SIGNAL_##CAPS_CASE##_INTERNAL) \
\
static void has_signal_##snake_case##_internal_finalize(GObject* object) \
{ \
    auto* self = MOUSETRAP_HAS_SIGNAL_##CAPS_CASE##_INTERNAL(object); \
    G_OBJECT_CLASS(has_signal_##snake_case##_internal_parent_class)->finalize(object); \
} \
\
DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(HasSignal##CamelCase##Internal, has_signal_##snake_case##_internal, HAS_SIGNAL_##CAPS_CASE##_INTERNAL) \
\
HasSignal##CamelCase##Internal* has_signal_##snake_case##_internal_new(NativeObject instance) \
{ \
    auto* self = (HasSignal##CamelCase##Internal*) g_object_new(has_signal_##snake_case##_internal_get_type(), nullptr); \
    has_signal_##snake_case##_internal_init(self); \
    \
    self->instance = instance; \
    self->function = nullptr; \
    self->is_blocked = false; \
    return self; \
} \

namespace mousetrap::detail
{
    DEFINE_SIGNAL(Activate, activate, ACTIVATE, "activate", void);
    DEFINE_SIGNAL(Startup, startup, STARTUP, "startup", void);
    DEFINE_SIGNAL(Shutdown, shutdown, SHUTDOWN, "shutdown", void);
    DEFINE_SIGNAL(Update, update, UPDATE, "update", void);
    DEFINE_SIGNAL(Paint, paint, PAINT, "paint", void);
    DEFINE_SIGNAL(Realize, realize, REALIZE, "realize", void);
    DEFINE_SIGNAL(Unrealize, unrealize, UNREALIZE, "unrealize", void);
    DEFINE_SIGNAL(Destroy, destroy, DESTROY, "destroy", void);
    DEFINE_SIGNAL(Hide, hide, HIDE, "hide", void);
    DEFINE_SIGNAL(Show, show, SHOW, "show", void);
    DEFINE_SIGNAL(Map, map, MAP, "map", void);
    DEFINE_SIGNAL(Unmap, unmap, UNMAP, "unmap", void);
    DEFINE_SIGNAL(CloseRequest, close_request, CLOSE_REQUEST, "close-request", WindowCloseRequestResult);
    DEFINE_SIGNAL(ActivateDefaultWidget, activate_default_widget, ACTIVATE_DEFAULT_WIDGET, "activate-default", void);
    DEFINE_SIGNAL(ActivateFocusedWidget, activate_focused_widget, ACTIVATE_FOCUSED_WIDGET, "activate-focus", void);
    DEFINE_SIGNAL(Clicked, clicked, CLICKED, "clicked", void);
    DEFINE_SIGNAL(Toggled, toggled, TOGGLED, "toggled", void);
    DEFINE_SIGNAL(TextChanged, text_changed, TEXT_CHANGED, "changed", void);

    /*
    DEFINE_SIGNAL(TextInserted, text_inserted, TEXT_INSERTED, "insert-text", void,
                              SPLAT(int32_t start_pos, int32_t end_pos),
                              SPLAT(start_pos, end_pos)
        );

    DEFINE_SIGNAL(TextDeleted, text_deleted, TEXT_DELETED, "delete-text", void,
                          SPLAT(int32_t start_pos, int32_t end_pos),
                          SPLAT(start_pos, end_pos)
    );
     */

    DEFINE_SIGNAL(Undo, undo, UNDO, "undo", void);
    DEFINE_SIGNAL(Redo, redo, REDO, "redo", void);
    DEFINE_SIGNAL(SelectionChanged, selection_changed, SELECTION_CHANGED, "selection-changed", void,
                              SPLAT(int32_t position, int32_t n_items),
                              SPLAT(position, n_items)
        );

    DEFINE_SIGNAL(ActivateItem, activate_item, ACTIVATE_ITEM, "activate", void,
                          SPLAT(guint position),
                          SPLAT(position)
    );

    DEFINE_SIGNAL(KeyPressed, key_pressed, KEY_PRESSED, "key-pressed", bool,
                          SPLAT(KeyValue keyval, KeyCode keycode, ModifierState modifier),
                          SPLAT(keyval, keycode, modifier)
    );


    DEFINE_SIGNAL(KeyReleased, key_released, KEY_RELEASED, "key-released", void,
                          SPLAT(KeyValue keyval, KeyCode keycode, ModifierState modifier),
                          SPLAT(keyval, keycode, modifier)
    );DEFINE_SIGNAL(ModifiersChanged, modifiers_changed, MODIFIERS_CHANGED, "modifiers", bool,
                          SPLAT(KeyValue keyval, KeyCode keycode, ModifierState modifier),
                          SPLAT(keyval, keycode, modifier)
    );


    DEFINE_SIGNAL(MotionEnter, motion_enter, MOTION_ENTER, "enter", void,
                          SPLAT(double x, double y),
                          SPLAT(x, y)
    );

    DEFINE_SIGNAL(Motion, motion, MOTION, "motion", void,
                          SPLAT(double x, double y),
                          SPLAT(x, y)
    );

    DEFINE_SIGNAL(MotionLeave, motion_leave, MOTION_LEAVE, "leave", void);
    DEFINE_SIGNAL(ClickPressed, click_pressed, CLICK_PRESSED, "pressed", void,
                          SPLAT(int32_t n_press, double x, double y),
                          SPLAT(n_press, x, y)
    );
    
    DEFINE_SIGNAL(ClickReleased, click_released, CLICK_RELEASED, "released", void,
                          SPLAT(int32_t n_press, double x, double y),
                          SPLAT(n_press, x, y)
    );


    DEFINE_SIGNAL(ClickStopped, click_stopped, CLICK_STOPPED, "stopped", void);
        DEFINE_SIGNAL(KineticScrollDecelerate, kinetic_scroll_decelerate, KINETIC_SCROLL_DECELERATE, "decelerate", void,
                          SPLAT(double x_velocity, double y_velocity),
                          SPLAT(x_velocity, y_velocity)
    );

    DEFINE_SIGNAL(ScrollBegin, scroll_begin, SCROLL_BEGIN, "scroll-begin", void);
DEFINE_SIGNAL(Scroll, scroll, SCROLL, "scroll", bool,
                          SPLAT(double delta_x, double delta_y),
                          SPLAT(delta_x, delta_y)
    );

    DEFINE_SIGNAL(ScrollEnd, scroll_end, SCROLL_END, "scroll-end", void);
    DEFINE_SIGNAL(FocusGained, focus_gained, FOCUS_GAINED, "enter", void);
    DEFINE_SIGNAL(FocusLost, focus_lost, FOCUS_LOST, "leave", void);
    DEFINE_SIGNAL(DragBegin, drag_begin, DRAG_BEGIN, "drag-begin", void,
                          SPLAT(double start_x, double start_y),
                          SPLAT(start_x, start_y)
    );

    DEFINE_SIGNAL(Drag, drag, DRAG, "drag-update", void,
                          SPLAT(double offset_x, double offset_y),
                          SPLAT(offset_x, offset_y)
    );

    DEFINE_SIGNAL(DragEnd, drag_end, DRAG_END, "drag-end", void,
                          SPLAT(double offset_x, double offest_y),
                          SPLAT(offset_x, offest_y)
    );

    DEFINE_SIGNAL(ScaleChanged, scale_changed, SCALE_CHANGED, "scale-changed", void,
                          double scale,
                          scale
    );
    DEFINE_SIGNAL(RotationChanged, rotation_changed, ROTATION_CHANGED, "angle-changed", void,
                          SPLAT(double angle_absolute_radians, double angle_delta_radians),
                          SPLAT(angle_absolute_radians, angle_delta_radians)
    );

    DEFINE_SIGNAL(PropertiesChanged, properties_changed, PROPERTIES_CHANGED, "changed", void);    
    DEFINE_SIGNAL(ValueChanged, value_changed, VALUE_CHANGED, "value-changed", void);
    DEFINE_SIGNAL(Render, render, RENDER, "render", bool, GdkGLContext* context, context);
    DEFINE_SIGNAL(Resize, resize, RESIZE, "resize", void,
                          SPLAT(int32_t width, int32_t height),
                          SPLAT(width, height)
    );

    DEFINE_SIGNAL(PageAdded, page_added, PAGE_ADDED, "page-added", void,
                          SPLAT(GtkWidget* _, uint32_t page_index),
                          SPLAT(_, page_index)
    );

    DEFINE_SIGNAL(PageRemoved, page_removed, PAGE_REMOVED, "page-removed", void,
                          SPLAT(GtkWidget* _, uint32_t page_index),
                          SPLAT(_, page_index)
    );

    DEFINE_SIGNAL(PageReordered, page_reordered, PAGE_REORDERED, "page-reordered", void,
                          SPLAT(GtkWidget* widget, uint32_t page_index),
                          SPLAT(widget, page_index)
    );

    DEFINE_SIGNAL(PageSelectionChanged, page_selection_changed, PAGE_SELECTION_CHANGED, "switch-page", void,
                          SPLAT(GtkWidget* _, uint32_t page_index),
                          SPLAT(_, page_index)
    );

    DEFINE_SIGNAL(Wrapped, wrapped, WRAPPED, "wrapped", void);
    DEFINE_SIGNAL(PressCancelled, press_cancelled, PRESS_CANCELLED, "cancelled", void);
    DEFINE_SIGNAL(Pressed, pressed, PRESSED, "pressed", void,
                          SPLAT(double x, double y),
                          SPLAT(x, y)
    )

    DEFINE_SIGNAL(Pan, pan, PAN, "pan", void,
                          SPLAT(PanDirection direction, double offset),
                          SPLAT(direction, offset)
    );

    DEFINE_SIGNAL(Swipe, swipe, SWIPE, "swipe", void,
                          SPLAT(double x_velocity, double y_velocity),
                          SPLAT(x_velocity, y_velocity)
    );

    DEFINE_SIGNAL(StylusDown, stylus_down, STYLUS_DOWN, "down", void,
                          SPLAT(double x, double y),
                          SPLAT(x, y)
    );

    DEFINE_SIGNAL(StylusUp, stylus_up, STYLUS_UP, "up", void,
                          SPLAT(double x, double y),
                          SPLAT(x, y)
    );

    DEFINE_SIGNAL(Proximity, proximity, PROXIMITY, "proximity", void,
                          SPLAT(double x, double y),
                          SPLAT(x, y)
    );

    DEFINE_SIGNAL(Play, play, PLAY, "play", void);
    DEFINE_SIGNAL(Closed, closed, CLOSED, "closed", void);

    DEFINE_SIGNAL(ItemsChanged, items_changed, ITEMS_CHANGED, "items-changed", void,
          SPLAT(gint position, gint n_removed, gint n_added),
          SPLAT(position, n_removed, n_added)
    );

    DEFINE_SIGNAL(Revealed, revealed, REVEALED, "notify::reveal-child", void, UnusedArgument_t _, _);
    DEFINE_SIGNAL(Switched, switched, SWITCHED, "notify::active", void, UnusedArgument_t _, _);
    DEFINE_SIGNAL(Activated, activated, ACTIVATED, "activate", void, UnusedArgument_t _, _);

    DEFINE_SIGNAL(ScrollChild, scroll_child, SCROLL_CHILD, "scroll-child", void,
      SPLAT(ScrollType scroll_type, bool is_horizontal),
      SPLAT(scroll_type, is_horizontal)
    );

    DEFINE_SIGNAL(Dismissed, dismissed, DISMISSED, "dismissed", void);
    DEFINE_SIGNAL(ButtonClicked, button_clicked, BUTTON_CLICKED, "dismissed", void);
}
