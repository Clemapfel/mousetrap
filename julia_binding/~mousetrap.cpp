//
// Created by clem on 4/16/23.
//

#include <mousetrap.hpp>
#include <jlcxx/jlcxx.hpp>

using namespace mousetrap;

// ### COMMON

static inline jl_value_t* jl_box_string(const std::string& in)
{
    static auto* string = jl_get_global(jl_base_module, jl_symbol("string"));
    return jl_call1(string, (jl_value_t*) jl_symbol(in.c_str()));
}

static void jl_println(jl_value_t* value)
{
    static auto* println = jl_get_function(jl_base_module, "println");
    jl_call1(println, value);
}

template<typename... T>
static inline jl_value_t* jl_calln(jl_function_t* function, T... args)
{
    std::array<jl_value_t*, sizeof...(T)> wrapped = {args...};
    return jl_call(function, wrapped.data(), wrapped.size());
}

template<typename... T>
static inline jl_value_t* jl_safe_call(const char* scope, jl_function_t* function, T... args)
{
    static auto* safe_call = jl_eval_string("return mousetrap.safe_call");
    std::array<jl_value_t*, sizeof...(T)+2> wrapped = {jl_box_string(scope), function, args...};
    return jl_call(safe_call, wrapped.data(), wrapped.size());
}

static inline jl_value_t* jl_get_property(jl_value_t* object, const char* property_name)
{
    static auto* get_property = jl_get_function(jl_base_module, "getproperty");
    return jl_call2(get_property, object, (jl_value_t*) jl_symbol(property_name));
}

template<typename... Ts>
static inline jl_value_t* jl_wrap(Ts... args)
{
    auto* wrap = jl_eval_string("(args...) -> args");
    return jl_calln(wrap, args...);
}

// ### BOX

static jl_value_t* box_vector2f(Vector2f in)
{
    static auto* ctor = jl_eval_string("mousetrap.Vector2f");
    return jl_calln(ctor, jl_box_float32(in.x), jl_box_float32(in.y));
}

// ### CXX WRAP COMMON

#define USE_FINALIZERS true

#define add_type(Type) add_type<Type>(std::string("_") + #Type)
#define add_type_method(Type, id) method(#id, &Type::id)
#define add_constructor(...) constructor<__VA_ARGS__>(USE_FINALIZERS)

#define declare_is_subtype_of(A, B) template<> struct jlcxx::SuperType<A> { typedef B type; };
#define make_not_mirrored(Name) template<> struct jlcxx::IsMirroredType<Name> : std::false_type {};
#define add_enum_value(EnumName, PREFIX, VALUE_NAME) set_const(std::string(#PREFIX) + "_" + std::string(#VALUE_NAME), (int) EnumName::VALUE_NAME)

// ### SIGNAL COMPONENTS

#define _DEFINE_ADD_SIGNAL_INVARIANT(snake_case) \
    method("disconnect_signal_" + std::string(#snake_case), [](T& instance) { \
        instance.disconnect_signal_##snake_case(); \
    }) \
    .method("set_signal_" + std::string(#snake_case) + "_blocked", [](T& instance, bool b){ \
        instance.set_signal_##snake_case##_blocked(b); \
    }) \
    .method("get_signal_" + std::string(#snake_case) + "_blocked", [](T& instance) -> bool { \
        return instance.get_signal_##snake_case##_blocked(); \
    });

#define DEFINE_ADD_SIGNAL_ARG0(snake_case, return_t) \
template<typename T, typename Arg_t> \
void add_signal_##snake_case(Arg_t type) \
{ \
    type.method("connect_signal_" + std::string(#snake_case), [](T& instance, jl_function_t* task) \
    { \
        instance.connect_signal_##snake_case([](T& instance, jl_function_t* task) -> return_t { \
            return jlcxx::unbox<return_t>( \
                jl_safe_call( \
                    ("emit_signal_" + std::string(#snake_case)).c_str(), \
                    task,                                   \
                    jl_wrap(jlcxx::box<T&>(instance))\
                ) \
            ); \
        }, task); \
    }) \
    .method("emit_signal_" + std::string(#snake_case), [](T& instance) -> return_t { \
        return instance.emit_signal_##snake_case(); \
    }); \
    type._DEFINE_ADD_SIGNAL_INVARIANT(snake_case)      \
}

#define DEFINE_ADD_SIGNAL_ARG1(snake_case, return_t, arg1_t, arg1_name) \
template<typename T, typename Arg_t> \
void add_signal_##snake_case(Arg_t type) \
{ \
    type.method("connect_signal_" + std::string(#snake_case), [](T& instance, jl_function_t* task) \
    { \
        instance.connect_signal_##snake_case([](T& instance, arg1_t arg1_name, jl_function_t* task) -> return_t { \
            return jlcxx::unbox<return_t>( \
                jl_safe_call( \
                    ("emit_signal_" + std::string(#snake_case)).c_str(), \
                    task, \
                    jl_wrap(jlcxx::box<T&>(instance), jlcxx::box<arg1_t>(arg1_name)) \
                ) \
            ); \
        }, task); \
    }) \
    .method("emit_signal_" + std::string(#snake_case), [](T& instance, arg1_t arg1_name) -> return_t { \
        return instance.emit_signal_##snake_case(arg1_name); \
    }); \
    type._DEFINE_ADD_SIGNAL_INVARIANT(snake_case)      \
}

#define DEFINE_ADD_SIGNAL_ARG2(snake_case, return_t, arg1_t, arg1_name, arg2_t, arg2_name) \
template<typename T, typename Arg_t> \
void add_signal_##snake_case(Arg_t type) \
{ \
    type.method("connect_signal_" + std::string(#snake_case), [](T& instance, jl_function_t* task) \
    { \
        instance.connect_signal_##snake_case([](T& instance, arg1_t arg1_name, arg2_t arg2_name, jl_function_t* task) -> return_t { \
            return jlcxx::unbox<return_t>( \
                jl_safe_call( \
                    ("emit_signal_" + std::string(#snake_case)).c_str(), \
                    task, \
                    jl_wrap(jlcxx::box<T&>(instance), jlcxx::box<arg1_t>(arg1_name), jlcxx::box<arg2_t>(arg2_name)) \
                ) \
            ); \
        }, task); \
    }); \
    type._DEFINE_ADD_SIGNAL_INVARIANT(snake_case)      \
}

#define DEFINE_ADD_SIGNAL_ARG3(snake_case, return_t, arg1_t, arg1_name, arg2_t, arg2_name, arg3_t, arg3_name) \
template<typename T, typename Arg_t> \
void add_signal_##snake_case(Arg_t type) \
{ \
    type.method("connect_signal_" + std::string(#snake_case), [](T& instance, jl_function_t* task) \
    { \
        instance.connect_signal_##snake_case([](T& instance, arg1_t arg1_name, arg2_t arg2_name, arg3_t arg3_name, jl_function_t* task) -> return_t { \
            return jlcxx::unbox<return_t>( \
                jl_safe_call( \
                    ("emit_signal_" + std::string(#snake_case)).c_str(), \
                    task, \
                    jl_wrap(jlcxx::box<T&>(instance), jlcxx::box<arg1_t>(arg1_name), jlcxx::box<arg2_t>(arg2_name), jlcxx::box<arg3_t>(arg3_name)) \
                ) \
            ); \
        }, task); \
    }) \
    .method("emit_signal_" + std::string(#snake_case), [](T& instance, arg1_t arg1_name, arg2_t arg2_name, arg3_t arg3_name) -> return_t { \
        return instance.emit_signal_##snake_case(arg1_name, arg2_name, arg3_name); \
    }); \
    type._DEFINE_ADD_SIGNAL_INVARIANT(snake_case)      \
}

#define DEFINE_ADD_SIGNAL_ARG4(snake_case, return_t, arg1_t, arg1_name, arg2_t, arg2_name, arg3_t, arg3_name, arg4_t, arg4_name) \
template<typename T, typename Arg_t> \
void add_signal_##snake_case(Arg_t type) \
{ \
    type.method("connect_signal_" + std::string(#snake_case), [](T& instance, jl_function_t* task) \
    { \
        instance.connect_signal_##snake_case([](T& instance, arg1_t arg1_name, arg2_t arg2_name, arg3_t arg3_name, arg4_t arg4_name, jl_function_t* task) -> return_t { \
            return jlcxx::unbox<return_t>( \
                jl_safe_call( \
                    ("emit_signal_" + std::string(#snake_case)).c_str(), \
                    task, \
                    jl_wrap(jlcxx::box<T&>(instance), jlcxx::box<arg1_t>(arg1_name), jlcxx::box<arg2_t>(arg2_name), jlcxx::box<arg3_t>(arg3_name), jlcxx::box<arg4_t>(arg4_name)) \
                ) \
            ); \
        }, task); \
    }) \
    .method("emit_signal_" + std::string(#snake_case), [](T& instance, arg1_t arg1_name, arg2_t arg2_name, arg3_t arg3_name, arg4_t arg4_name) -> return_t { \
        return instance.emit_signal_##snake_case(arg1_name, arg2_name, arg3_name, arg4_name); \
    }); \
    type._DEFINE_ADD_SIGNAL_INVARIANT(snake_case)      \
}

#define DEFINE_ADD_WIDGET_SIGNAL(...) ;

DEFINE_ADD_WIDGET_SIGNAL(realize)
DEFINE_ADD_WIDGET_SIGNAL(unrealize)
DEFINE_ADD_WIDGET_SIGNAL(destroy)
DEFINE_ADD_WIDGET_SIGNAL(hide)
DEFINE_ADD_WIDGET_SIGNAL(show)
DEFINE_ADD_WIDGET_SIGNAL(map)
DEFINE_ADD_WIDGET_SIGNAL(unmap)

template<typename Widget_t, typename T>
void add_widget_signals(T& type)
{
    // TODO
}

DEFINE_ADD_SIGNAL_ARG0(activate, void)
DEFINE_ADD_SIGNAL_ARG0(startup, void)
DEFINE_ADD_SIGNAL_ARG0(shutdown, void)
DEFINE_ADD_SIGNAL_ARG0(update, void)
DEFINE_ADD_SIGNAL_ARG0(paint, void)
DEFINE_ADD_SIGNAL_ARG0(close_request, WindowCloseRequestResult)
DEFINE_ADD_SIGNAL_ARG0(activate_default_widget, void)
DEFINE_ADD_SIGNAL_ARG0(activate_focused_widget, void)
DEFINE_ADD_SIGNAL_ARG0(clicked, void)
DEFINE_ADD_SIGNAL_ARG0(toggled, void)
DEFINE_ADD_SIGNAL_ARG0(text_changed, void)
DEFINE_ADD_SIGNAL_ARG0(undo, void)
DEFINE_ADD_SIGNAL_ARG0(redo, void)
DEFINE_ADD_SIGNAL_ARG2(selection_changed, void, gint, position, gint, n_items)
DEFINE_ADD_SIGNAL_ARG3(key_pressed, void, guint, keyval, guint, keycode, ModifierState, modifier) // TODO
DEFINE_ADD_SIGNAL_ARG3(key_released, void, guint, keyval, guint, keycode, ModifierState, modifier) // TODO
DEFINE_ADD_SIGNAL_ARG3(modifiers_changed, void, guint, keyval, guint, keycode, ModifierState, modifier) // TODO
DEFINE_ADD_SIGNAL_ARG2(motion_enter, void, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG2(motion, void, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG0(motion_leave, void)
DEFINE_ADD_SIGNAL_ARG3(click_pressed, void, gint, n_press, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG3(click_released, void, gint, n_press, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG0(click_stopped, void)
DEFINE_ADD_SIGNAL_ARG2(kinetic_scroll_delecerate, void, double, x_velocity, double, y_velocity)
DEFINE_ADD_SIGNAL_ARG0(scroll_begin, void)
DEFINE_ADD_SIGNAL_ARG2(scroll, void, double, delta_x, double, delta_y)
DEFINE_ADD_SIGNAL_ARG0(scroll_end, void)
DEFINE_ADD_SIGNAL_ARG0(focus_gained, void)
DEFINE_ADD_SIGNAL_ARG0(focus_lost, void)
DEFINE_ADD_SIGNAL_ARG2(drag_begin, void, double, start_x, double, start_y)
DEFINE_ADD_SIGNAL_ARG2(drag, void, double, offset_x, double, offset_y)
DEFINE_ADD_SIGNAL_ARG2(drag_end, void, double, offset_x, double, offset_y)
DEFINE_ADD_SIGNAL_ARG1(scale_changed, void, double, scale)
DEFINE_ADD_SIGNAL_ARG2(rotation_changed, void, double, angle_absolute_radians, double, angle_delta_radians)
DEFINE_ADD_SIGNAL_ARG0(properties_changed, void)
DEFINE_ADD_SIGNAL_ARG0(value_changed, void)
DEFINE_ADD_SIGNAL_ARG1(render, bool, GdkGLContext*, context) // TODO
DEFINE_ADD_SIGNAL_ARG2(resize, void, gint, width, gint, height)
DEFINE_ADD_SIGNAL_ARG2(page_added, void, void*, _, guint, page_index) // TODO
DEFINE_ADD_SIGNAL_ARG2(page_removed, void, void*, _, guint, page_index) // TODO
DEFINE_ADD_SIGNAL_ARG2(page_reordered, void, void*, _, guint, page_index) // TODO
DEFINE_ADD_SIGNAL_ARG2(page_selection_changed, void, void*, _, guint, page_index) // TODO
DEFINE_ADD_SIGNAL_ARG0(wrapped, void)
DEFINE_ADD_SIGNAL_ARG2(pressed, void, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG0(press_cancelled, void)
DEFINE_ADD_SIGNAL_ARG2(pan, void, PanDirection, direction, double, offset)
DEFINE_ADD_SIGNAL_ARG2(swipe, void, double, x_velocity, double, y_velocity)
DEFINE_ADD_SIGNAL_ARG2(stylus_down, void, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG2(stylus_up, void, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG2(proximity, void, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG2(scroll_child, void, ScrollType, scroll_type, bool, is_horizontal)
DEFINE_ADD_SIGNAL_ARG0(closed, void)
DEFINE_ADD_SIGNAL_ARG0(play, void)
DEFINE_ADD_SIGNAL_ARG0(stop, void)
DEFINE_ADD_SIGNAL_ARG3(items_changed, void, gint, position, gint, n_removed, gint, n_added)
DEFINE_ADD_SIGNAL_ARG0(revealed, void)
DEFINE_ADD_SIGNAL_ARG1(activated, void, void*, _) // TODO

// ### WIDGET

static void implement_widget(jlcxx::Module& m)
{
    #define widget_method(name, arg_list, arg_name_list) \
    method(#name, [](void* widget, arg_list) { \
        return ((Widget*) widget)->name(arg_name_list); \
    })

    #define widget_method_no_args(name) \
    method(#name, [](void* widget) { \
        return ((Widget*) widget)->name(); \
    })

    m.widget_method_no_args(activate);

    m.widget_method(set_margin_top, float margin, margin);
    m.widget_method(set_margin_bottom, float margin, margin);
    m.widget_method(set_margin_start, float margin, margin);
    m.widget_method(set_margin_end, float margin, margin);
    m.widget_method(set_margin_horizontal, float margin, margin);
    m.widget_method(set_margin_vertical, float margin, margin);
    m.widget_method(set_margin, float margin, margin);
    m.widget_method_no_args(get_margin_top);
    m.widget_method_no_args(get_margin_bottom);
    m.widget_method_no_args(get_margin_start);
    m.widget_method_no_args(get_margin_end);

    m.widget_method(set_expand_horizontally, bool b, b);
    m.widget_method(set_expand_vertically, bool b, b);
    m.widget_method(set_expand, bool b, b);
    m.widget_method_no_args(get_expand_horizontally);
    m.widget_method_no_args(get_expand_vertically);

    m.add_enum_value(Alignment, ALIGNMENT, START);
    m.add_enum_value(Alignment, ALIGNMENT, CENTER);
    m.add_enum_value(Alignment, ALIGNMENT, END);

    m.widget_method(set_horizontal_alignment, int alignment, (Alignment) alignment);
    m.widget_method(set_vertical_alignment, int alignment, (Alignment) alignment);
    m.widget_method(set_alignment, int alignment, (Alignment) alignment);

    m.method("get_horizontal_alignment", [](void* widget) -> int{
        return (int) ((Widget*) widget)->get_horizontal_alignment();
    });

    m.method("get_vertical_alignment", [](void* widget) -> int {
        return (int) ((Widget*) widget)->get_vertical_alignment();
    });

    m.widget_method(set_opacity, float opacity, opacity);
    m.widget_method_no_args(get_opacity);

    m.widget_method(set_is_visible, bool b, b);
    m.widget_method_no_args(get_is_visible);

    m.widget_method(set_tooltip_text, const std::string& text, text);
    m.method("set_tooltip_widget", [](void* parent, void* tooltip){
        ((Widget*) parent)->set_tooltip_widget(*((Widget*) tooltip));
    });
    m.widget_method_no_args(remove_tooltip_widget);

    #define set_cursor_type(VALUE) add_enum_value(CursorType, CURSOR_TYPE, VALUE)

    m.set_cursor_type(NONE);
    m.set_cursor_type(DEFAULT);
    m.set_cursor_type(HELP);
    m.set_cursor_type(POINTER);
    m.set_cursor_type(CONTEXT_MENU);
    m.set_cursor_type(PROGRESS);
    m.set_cursor_type(WAIT);
    m.set_cursor_type(CELL);
    m.set_cursor_type(CROSSHAIR);
    m.set_cursor_type(TEXT);
    m.set_cursor_type(MOVE);
    m.set_cursor_type(NOT_ALLOWED);
    m.set_cursor_type(GRAB);
    m.set_cursor_type(GRABBING);
    m.set_cursor_type(ALL_SCROLL);
    m.set_cursor_type(ZOOM_IN);
    m.set_cursor_type(ZOOM_OUT);
    m.set_cursor_type(COLUMN_RESIZE);
    m.set_cursor_type(ROW_RESIZE);
    m.set_cursor_type(NORTH_RESIZE);
    m.set_cursor_type(NORTH_EAST_RESIZE);
    m.set_cursor_type(EAST_RESIZE);
    m.set_cursor_type(SOUTH_EAST_RESIZE);
    m.set_cursor_type(SOUTH_RESIZE);
    m.set_cursor_type(SOUTH_WEST_RESIZE);
    m.set_cursor_type(WEST_RESIZE);
    m.set_cursor_type(NORTH_WEST_RESIZE);
    m.set_cursor_type(HORIZONTAL_RESIZE);
    m.set_cursor_type(VERTICAL_RESIZE);
    
    m.widget_method(set_cursor, int type, (CursorType) type);
    // TODO: set_cursor_from_image

    m.widget_method_no_args(hide);
    m.widget_method_no_args(show);
    
    m.method("add_controller", [](void* widget, void* controller) {
        ((Widget*) widget)->add_controller(*((EventController*) controller));
    });

    m.method("remove_controller", [](void* widget, void* controller) {
        ((Widget*) widget)->remove_controller(*((EventController*) controller));
    });
    
    m.widget_method(set_is_focusable, bool b, b);
    m.widget_method_no_args(get_is_focusable);
    m.widget_method_no_args(grab_focus);
    m.widget_method_no_args(get_has_focus);
    m.widget_method(set_focus_on_click, bool b, b);
    m.widget_method_no_args(get_focus_on_click);
    m.widget_method_no_args(get_is_realized);

    m.method("get_minimum_size", [](void* widget) {
        return box_vector2f(((Widget*) widget)->get_minimum_size());
    });

    m.method("get_natural_size", [](void* widget) {
        return box_vector2f(((Widget*) widget)->get_natural_size());
    });

    m.method("get_position", [](void* widget) {
        return box_vector2f(((Widget*) widget)->get_position());
    });

    m.method("get_allocated_size", [](void* widget) {
        return box_vector2f(((Widget*) widget)->get_allocated_size());
    });

    m.widget_method_no_args(unparent);

    m.widget_method(set_can_respond_to_input, bool b, b);
    m.widget_method_no_args(get_can_respond_to_input);

    m.widget_method(set_hide_on_overflow, bool b, b);
    m.widget_method_no_args(get_hide_on_overflow);

    m.add_type(FrameClock)
        .constructor([](void* instance){
            return new FrameClock((GdkFrameClock*) instance);
        }, USE_FINALIZERS)
        .method("get_frame_time", [](FrameClock& instance){
            return instance.get_frame_time().as_microseconds();
        })
        .method("get_time_since_last_frame", [](FrameClock& instance){
            return instance.get_time_since_last_frame().as_microseconds();
        })
        .add_type_method(FrameClock, get_fps)
    ;

    m.widget_method_no_args(get_frame_clock);


    m.add_enum_value(TickCallbackResult, TICK_CALLBACK_RESULT, CONTINUE);
    m.add_enum_value(TickCallbackResult, TICK_CALLBACK_RESULT, DISCONTINUE);

    m.method("set_tick_callback", [](void* widget, jl_function_t* task) {

        ((Widget*) widget)->set_tick_callback([](FrameClock clock, jl_function_t* task) -> TickCallbackResult {

            auto* out = jl_safe_call("Widget::tick_callback", task, jlcxx::box<FrameClock&>(clock));
            if (out == nullptr)
                return TickCallbackResult::DISCONTINUE;
            else
                return (TickCallbackResult) jl_unbox_int32(out);
        }, task);
    });
    m.widget_method_no_args(remove_tick_callback);

    // TODO: get clipboard
}

// ### APPLICATION

static void implement_application(jlcxx::Module& module)
{
    auto application = module.add_type(Application)
        .add_constructor(const std::string&)
        .add_type_method(Application, run)
        .add_type_method(Application, quit)
        .add_type_method(Application, hold)
        .add_type_method(Application, release)
        .add_type_method(Application, mark_as_busy)
        .add_type_method(Application, unmark_as_busy)
        .add_type_method(Application, get_id)

        .add_type_method(Application, add_action)
        .add_type_method(Application, remove_action)
        .add_type_method(Application, has_action)
        .method("get_action", [](Application& instance, const std::string& id) {

            static auto* action_ctor = jl_eval_string("return mousetrap.Action");
            return jl_safe_call("Application::get_action", action_ctor, jlcxx::box<Action>(instance.get_action(id)));
        })
    ;

    add_signal_activate<Application>(application);
    add_signal_shutdown<Application>(application);
}

// ### ACTION

static void implement_action(jlcxx::Module& module)
{
    auto action = module.add_type(Action)
        .constructor( [](const std::string& id, void* app_ptr){
            return new Action(id, *((Application*) app_ptr));
        }, USE_FINALIZERS)
        .add_type_method(Action, get_id)
        .add_type_method(Action, set_state)
        .add_type_method(Action, get_state)
        .add_type_method(Action, activate)
        .add_type_method(Action, add_shortcut)
        .add_type_method(Action, get_shortcuts)
        .add_type_method(Action, clear_shortcuts)
        .add_type_method(Action, set_enabled)
        .add_type_method(Action, get_enabled)
        .add_type_method(Action, get_is_stateful)
    ;

    action.method("set_function", [](Action& action, jl_function_t* task) {
        action.set_function([](Action& action, jl_function_t* task){
            jl_safe_call("Action::activate", task, jlcxx::box<Action&>(action));
        }, task);
    });

    action.method("set_stateful_function", [](Action& action, jl_function_t* task, bool initial_state) {
        action.set_stateful_function([](Action& action, bool state, jl_function_t* task) -> bool {
            return jl_unbox_bool(jl_safe_call("Action::activate", task, jlcxx::box<Action&>(action), jl_box_bool(state)));
        }, task);
    });

    // TODO add_signal_activated<Action>(action);
}

// ### ADJUSTMENT

void implement_adjustment(jlcxx::Module& module)
{
    auto adjustment = module.add_type(Adjustment)
        .add_constructor(float, float, float, float)
        .add_type_method(Adjustment, get_lower)
        .add_type_method(Adjustment, get_upper)
        .add_type_method(Adjustment, get_value)
        .add_type_method(Adjustment, get_increment)
        .add_type_method(Adjustment, set_lower)
        .add_type_method(Adjustment, set_upper)
        .add_type_method(Adjustment, set_value)
        .add_type_method(Adjustment, set_increment)
    ;

    add_signal_value_changed<Adjustment>(adjustment);
    add_signal_properties_changed<Adjustment>(adjustment);
}

// ### ASPECT FRAME
void implement_aspect_frame(jlcxx::Module& module)
{
    auto aspect_frame = module.add_type(AspectFrame)
        .add_constructor(float, float, float)
        .add_type_method(AspectFrame, set_ratio)
        .add_type_method(AspectFrame, get_ratio)
        .add_type_method(AspectFrame, set_child_x_alignment)
        .add_type_method(AspectFrame, set_child_y_alignment)
        .add_type_method(AspectFrame, get_child_x_alignment)
        .add_type_method(AspectFrame, get_child_y_alignment)
        .method("add_child", [](AspectFrame& instance, void* widget) {
            instance.set_child(*((Widget*) widget));
        })
        .add_type_method(AspectFrame, remove_child)
    ;
}

// ### WINDOW

void implement_window(jlcxx::Module& module)
{
    auto window = module.add_type(Window)
        .constructor([](Application& app){
            return new Window(app);
        }, USE_FINALIZERS)
        .add_type_method(Window, set_maximized)
        .add_type_method(Window, set_fullscreen)
        .add_type_method(Window, present)
        .add_type_method(Window, close)
        .add_type_method(Window, set_hide_on_close)
        .method("set_child", [](Window& window, void* widget) {
            window.set_child(*((Widget*) widget));
        })
        .add_type_method(Window, remove_child)
        .add_type_method(Window, set_destroy_with_parent)
        .add_type_method(Window, get_destroy_with_parent)
        .add_type_method(Window, set_title)
        .add_type_method(Window, get_title)
        .method("set_titlebar_widget", [](Window& window, void* widget){
            window.set_titlebar_widget(*((Widget*) widget));
        })
        .add_type_method(Window, remove_titlebar_widget)
        .add_type_method(Window, set_is_modal)
        .add_type_method(Window, get_is_modal)
        .add_type_method(Window, set_is_decorated)
        .add_type_method(Window, get_is_decorated)
        .add_type_method(Window, set_has_close_button)
        .add_type_method(Window, get_has_close_button)
        .add_type_method(Window, set_startup_notification_identifier)
        .add_type_method(Window, set_focus_visible)
        .add_type_method(Window, get_focus_visible)
        .method("set_default_widget", [](Window& window, void* widget) {
            window.set_default_widget(*((Widget*) widget));
        });

        add_widget_signals<Window>(window);
        add_signal_close_request<Window>(window);
        add_signal_activate_default_widget<Window>(window);
        add_signal_activate_focused_widget<Window>(window);
}

// ### LOG

void implement_log(jlcxx::Module& module)
{
    #define implement_log_method(level) \
        module.method("log_" + std::string(#level), [](const std::string& domain, const std::string& message){ \
            log::level(message, (LogDomain) domain.c_str()); \
        });

    implement_log_method(debug)
    implement_log_method(info)
    implement_log_method(warning)
    implement_log_method(critical)
    implement_log_method(fatal)

    module.method("log_set_surpress_debug", [](const std::string& domain, bool b) {
        log::set_surpress_debug((LogDomain) domain.c_str(), b);
    });

    module.method("log_set_surpress_info", [](const std::string& domain, bool b) {
        log::set_surpress_info((LogDomain) domain.c_str(), b);
    });

    module.method("log_set_file", [](const std::string& path) -> bool{
        return log::set_file(path);
    });
}

// ### BLEND MODE
void implement_blend_mode(jlcxx::Module& module)
{
    module.add_enum_value(BlendMode, BLEND_MODE, NONE);
    module.add_enum_value(BlendMode, BLEND_MODE, NORMAL);
    module.add_enum_value(BlendMode, BLEND_MODE, ADD);
    module.add_enum_value(BlendMode, BLEND_MODE, SUBTRACT);
    module.add_enum_value(BlendMode, BLEND_MODE, REVERSE_SUBTRACT);
    module.add_enum_value(BlendMode, BLEND_MODE, MULTIPLY);
    module.add_enum_value(BlendMode, BLEND_MODE, MIN);
    module.add_enum_value(BlendMode, BLEND_MODE, MAX);
}

// ### ORIENTATION

void implement_orientation(jlcxx::Module& module)
{
    module.add_enum_value(Orientation, ORIENTATION, HORIZONTAL);
    module.add_enum_value(Orientation, ORIENTATION, VERTICAL);
}

// ### BOX

void implement_box(jlcxx::Module& module)
{
    auto box = module.add_type(Box)
        .constructor([](int orientation){
            return new Box((Orientation) orientation);
        }, USE_FINALIZERS)
        .method("push_back", [](Box& box, void* widget) {
            box.push_back(*((Widget*) widget));
        })
        .method("push_front", [](Box& box, void* widget) {
            box.push_front(*((Widget*) widget));
        })
        .method("insert_after", [](Box& box, void* to_append, void* after) {
            box.insert_after(*((Widget*) to_append), *((Widget*) after));
        })
        .method("remove", [](Box& box, void* widget){
            box.remove(*((Widget*) widget));
        })
        .add_type_method(Box, clear)
        .add_type_method(Box, set_homogeneous)
        .add_type_method(Box, get_homogeneous)
        .add_type_method(Box, set_spacing)
        .add_type_method(Box, get_spacing)
        .add_type_method(Box, get_n_items)
        .method("get_orientation", [](Box& box) -> int{
            return (int) box.get_orientation();
        })
        .method("set_orientation", [](Box& box, int orientation){
           box.set_orientation((Orientation) orientation);
        })
    ;

    add_widget_signals<Box>(box);
}

// ### BUTTON

void implement_button(jlcxx::Module& module)
{
    auto button = module.add_type(Button)
        .constructor()
        .add_type_method(Button, set_has_frame)
        .add_type_method(Button, get_has_frame)
        .add_type_method(Button, set_is_circular)
        .add_type_method(Button, get_is_circular)
        .method("set_child", [](Button& button, void* widget){
            button.set_child(*((Widget*) widget));
        })
        .add_type_method(Button, remove_child)
        .add_type_method(Button, set_action)
    ;

    add_signal_activate<Button>(button);
    add_signal_clicked<Button>(button);
    add_widget_signals<Button>(button);
}

// ### CENTER BOX

void implement_center_box(jlcxx::Module& module)
{
    auto center_box = module.add_type(CenterBox)
        .constructor([](int orientation){
            return new CenterBox((Orientation) orientation);
        }, USE_FINALIZERS)
        .method("set_start_child", [](CenterBox& instance, void* widget){
            instance.set_start_child(*((Widget*) widget));
        })
        .method("set_center_child", [](CenterBox& instance, void* widget){
            instance.set_center_child(*((Widget*) widget));
        })
        .method("set_end_child", [](CenterBox& instance, void* widget){
            instance.set_end_child(*((Widget*) widget));
        })
        .add_type_method(CenterBox, remove_start_widget)
        .add_type_method(CenterBox, remove_center_widget)
        .add_type_method(CenterBox, remove_end_widget)
        .method("get_orientation", [](CenterBox& instance){
            return (int) instance.get_orientation();
        })
        .method("set_orientation", [](CenterBox& instance, int orientation) {
            instance.set_orientation((Orientation) orientation);
        })
    ;

    add_widget_signals<CenterBox>(center_box);
}

// ### CHECK BUTTON

void implement_check_button(jlcxx::Module& module)
{
    module.add_enum_value(CheckButtonState, CHECK_BUTTON_STATE, ACTIVE);
    module.add_enum_value(CheckButtonState, CHECK_BUTTON_STATE, INACTIVE);
    module.add_enum_value(CheckButtonState, CHECK_BUTTON_STATE, INCONSISTENT);

    auto check_button = module.add_type(CheckButton)
        .constructor()
        .method("set_state", [](CheckButton& button, int state){
            button.set_state((CheckButtonState) state);
        })
        .method("get_state", [](CheckButton& button) -> int {
            return (int) button.get_state();
        })
        .add_type_method(CheckButton, get_active)
    ;

    #if GTK_MINOR_VERSION >= 8
        check_button.method("set_child", [](CheckButton& button, void* widget) {
            button.set_child(*((Widget*) widget));
        })
        .add_type_method(CheckButton, remove_child);
    #endif

    add_widget_signals<CheckButton>(check_button);
    add_signal_toggled<CheckButton>(check_button);
}

// ### COLOR

void implement_colors(jlcxx::Module& module)
{
    module.method("rgba_to_hsva", [](jl_value_t* rgba_in) -> jl_value_t*
    {
        static auto* hsva_ctor = jl_eval_string("return mousetrap.HSVA");
        auto as_rgba = RGBA(
            jl_unbox_float32(jl_get_property(rgba_in, "r")),
            jl_unbox_float32(jl_get_property(rgba_in, "g")),
            jl_unbox_float32(jl_get_property(rgba_in, "b")),
            jl_unbox_float32(jl_get_property(rgba_in, "a"))
        );

        auto as_hsva = rgba_to_hsva(as_rgba);
        return jl_calln(hsva_ctor,
            jl_box_float32(as_hsva.h),
            jl_box_float32(as_hsva.s),
            jl_box_float32(as_hsva.v),
            jl_box_float32(as_hsva.a)
        );
    });

    module.method("hsva_to_rgba", [](jl_value_t* rgba_in) -> jl_value_t*
    {
        static auto* rgba_ctor = jl_eval_string("return mousetrap.RGBA");
        auto as_hsva = HSVA(
            jl_unbox_float32(jl_get_property(rgba_in, "h")),
            jl_unbox_float32(jl_get_property(rgba_in, "s")),
            jl_unbox_float32(jl_get_property(rgba_in, "v")),
            jl_unbox_float32(jl_get_property(rgba_in, "a"))
        );

        auto as_rgba = hsva_to_rgba(as_hsva);
        return jl_calln(rgba_ctor,
            jl_box_float32(as_rgba.r),
            jl_box_float32(as_rgba.g),
            jl_box_float32(as_rgba.b),
            jl_box_float32(as_rgba.a)
        );
    });
}

// ### EVENT CONTROLLER

void implement_event_controller(jlcxx::Module& module)
{
    module.add_enum_value(PropagationPhase, PROPAGATION_PHASE, NONE);
    module.add_enum_value(PropagationPhase, PROPAGATION_PHASE, CAPTURE);
    module.add_enum_value(PropagationPhase, PROPAGATION_PHASE, BUBBLE);
    module.add_enum_value(PropagationPhase, PROPAGATION_PHASE, TARGET);
}

void implement_single_click_gesture(jlcxx::Module& module)
{
    module.add_enum_value(ButtonID, BUTTON_ID, NONE);
    module.add_enum_value(ButtonID, BUTTON_ID, ANY);
    module.add_enum_value(ButtonID, BUTTON_ID, BUTTON_01);
    module.add_enum_value(ButtonID, BUTTON_ID, BUTTON_02);
    module.add_enum_value(ButtonID, BUTTON_ID, BUTTON_03);
    module.add_enum_value(ButtonID, BUTTON_ID, BUTTON_04);
    module.add_enum_value(ButtonID, BUTTON_ID, BUTTON_05);
    module.add_enum_value(ButtonID, BUTTON_ID, BUTTON_06);
    module.add_enum_value(ButtonID, BUTTON_ID, BUTTON_07);
    module.add_enum_value(ButtonID, BUTTON_ID, BUTTON_08);
    module.add_enum_value(ButtonID, BUTTON_ID, BUTTON_09);
}

template<typename T, typename Arg_t>
void make_event_controller(Arg_t& type)
{
    type.method("set_propagation_phase", [](T& instance, int32_t propagation_phase)
    {
        instance.set_propagation_phase((PropagationPhase) propagation_phase);
    });
    type.method("get_propagation_phase", [](T& instance) -> int32_t
    {
        return (int32_t) instance.get_propagation_phase();
    });
}

template<typename T, typename Arg_t>
void make_single_click_gesture(Arg_t& type)
{
    type.method("get_current_button", [](T& instance) -> int32_t {
        return (int32_t) instance.get_current_button();
    });
    type.method("set_only_listens_to_button", [](T& instance, int32_t button_id) {
        instance.set_only_listens_to_button(instance, (ButtonID) button_id);
    });
    type.method("get_only_listens_to_button", [](T& instance, int32_t button_id) -> int32_t {
        (int32_t) instance.get_only_listens_to_button(instance);
    });
    type.method("set_touch_only", [](T& instance, bool b){
        instance.set_touch_only(b);
    });
    type.method("get_touch_only", [](T& instance) -> bool {
        return instance.get_touch_only();
    });
}

DEFINE_ADD_SIGNAL_ARG2(motion_enter, void, double, x, double, y);
DEFINE_ADD_SIGNAL_ARG2(motion, void, double, x, double, y);
DEFINE_ADD_SIGNAL_ARG0(motion_leave, void);

void implement_motion_event_controller(jlcxx::Module& module)
{
    auto motion = module.add_type(MotionEventController);

    add_signal_motion_enter<MotionEventController>(motion);
    add_signal_motion<MotionEventController>(motion);
    add_signal_motion_leave<MotionEventController>(motion);

    make_event_controller<MotionEventController>(motion);
}

// ### MAIN

JLCXX_MODULE define_julia_module(jlcxx::Module& module)
{
    module.set_const("GTK_MAJOR_VERSION", (int) GTK_MAJOR_VERSION);
    module.set_const("GTK_MINOR_VERSION", (int) GTK_MINOR_VERSION);

    implement_log(module);
    implement_widget(module);
    implement_action(module);
    implement_application(module);
    implement_window(module);
    implement_adjustment(module);
    implement_aspect_frame(module);
    implement_blend_mode(module);
    implement_orientation(module);
    implement_box(module);
    implement_button(module);
    implement_center_box(module);
    implement_check_button(module);
    implement_colors(module);

    implement_event_controller(module);
    implement_single_click_gesture(module);
    implement_motion_event_controller(module);

    // TODO:
    // click event controller
    // clip board
    // column view
    // darg event controller
    // drop down

}
