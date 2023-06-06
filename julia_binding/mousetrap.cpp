//
// Created by clem on 4/16/23.
//

#include <julia.h>
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

// ### BOX / UNBOX

static jl_value_t* box_vector2f(Vector2f in)
{
    static auto* ctor = jl_eval_string("mousetrap.Vector2f");
    return jl_calln(ctor, jl_box_float32(in.x), jl_box_float32(in.y));
}

static Vector2f unbox_vector2f(jl_value_t* in)
{
    return Vector2f {
        jl_unbox_float32(jl_get_property(in, "x")),
        jl_unbox_float32(jl_get_property(in, "y"))
    };
}

static jl_value_t* box_vector2i(Vector2i in)
{
    static auto* ctor = jl_eval_string("mousetrap.Vector2i");
    return jl_calln(ctor, jl_box_int64(in.x), jl_box_int64(in.y));
}

static Vector2f unbox_vector2i(jl_value_t* in)
{
    return Vector2i {
        jl_unbox_int64(jl_get_property(in, "x")),
        jl_unbox_int64(jl_get_property(in, "y"))
    };
}

static jl_value_t* box_rgba(RGBA in)
{
    static auto* ctor = jl_eval_string("return mousetrap.RGBA");
    return jl_calln(
        ctor,
        jl_box_float32(in.r),
        jl_box_float32(in.g),
        jl_box_float32(in.b),
        jl_box_float32(in.a)
    );
}

static RGBA unbox_rgba(jl_value_t* in)
{
    return RGBA {
        jl_unbox_float32(jl_get_property(in, "r")),
        jl_unbox_float32(jl_get_property(in, "g")),
        jl_unbox_float32(jl_get_property(in, "b")),
        jl_unbox_float32(jl_get_property(in, "a"))
    };
}

static jl_value_t* box_hsva(HSVA in)
{
    static auto* ctor = jl_eval_string("return mousetrap.RGBA");
    return jl_calln(
        ctor,
        jl_box_float32(in.h),
        jl_box_float32(in.s),
        jl_box_float32(in.v),
        jl_box_float32(in.a)
    );
}

static HSVA unbox_hsva(jl_value_t* in)
{
    return HSVA {
        jl_unbox_float32(jl_get_property(in, "h")),
        jl_unbox_float32(jl_get_property(in, "s")),
        jl_unbox_float32(jl_get_property(in, "v")),
        jl_unbox_float32(jl_get_property(in, "a"))
    };
}

// ### CXX WRAP COMMON

#define USE_FINALIZERS true

#define declare_is_subtype_of(A, B) template<> struct jlcxx::SuperType<A> { typedef B type; };
#define make_not_mirrored(Name) template<> struct jlcxx::IsMirroredType<Name> : std::false_type {};

#define add_type(Type) add_type<Type>(std::string("_") + #Type)
#define add_type_method(Type, id, ...) method(#id + std::string(#__VA_ARGS__), &Type::id)
#define add_constructor(...) constructor<__VA_ARGS__>(USE_FINALIZERS)

#define define_enum_in(module, Enum) \
    module.add_bits<Enum>(std::string("_") + #Enum, jl_int64_type); \
    module.method(std::string(#Enum) + "_to_int", [](Enum x) -> int64_t { return (int64_t) x; });

#define add_enum_value(Enum, PREFIX, VALUE) set_const(std::string(#PREFIX) + "_" + std::string(#VALUE), Enum::VALUE)

// ### SIGNAL COMPONENTS

#define _DEFINE_ADD_SIGNAL_INVARIANT(snake_case) \
    method("disconnect_signal_" + std::string(#snake_case) + "!", [](T& instance) { \
        instance.disconnect_signal_##snake_case(); \
    }) \
    .method("set_signal_" + std::string(#snake_case) + "_blocked!", [](T& instance, bool b){ \
        instance.set_signal_##snake_case##_blocked(b); \
    }) \
    .method("get_signal_" + std::string(#snake_case) + "_blocked", [](T& instance) -> bool { \
        return instance.get_signal_##snake_case##_blocked(); \
    });

#define DEFINE_ADD_SIGNAL_ARG0(snake_case, return_t) \
template<typename T, typename Arg_t> \
void add_signal_##snake_case(Arg_t type) \
{ \
    type.method("connect_signal_" + std::string(#snake_case) + "!", [](T& instance, jl_function_t* task) \
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
    type.method("connect_signal_" + std::string(#snake_case) + "!", [](T& instance, jl_function_t* task) \
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
    type.method("connect_signal_" + std::string(#snake_case) + "!", [](T& instance, jl_function_t* task) \
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
    type.method("connect_signal_" + std::string(#snake_case) + "!", [](T& instance, jl_function_t* task) \
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
    type.method("connect_signal_" + std::string(#snake_case) + "!", [](T& instance, jl_function_t* task) \
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

#define DEFINE_ADD_WIDGET_SIGNAL(snake_case) \
template<typename T, typename Arg_t>                               \
void add_signal_##snake_case(Arg_t type) {\
    type.method("connect_signal_" + std::string(#snake_case) + "!", [](T& instance, jl_function_t* task) \
    { \
        instance.connect_signal_##snake_case([](Widget& instance, jl_function_t* task) -> void { \
            jl_safe_call(("emit_signal_" + std::string(#snake_case)).c_str(), task, jlcxx::box<T&>(dynamic_cast<T&>(instance))); \
        }, task); \
    }) \
    .method("emit_signal_" + std::string(#snake_case), [](T& instance) { \
        instance.emit_signal_##snake_case(); \
    }); \
    type._DEFINE_ADD_SIGNAL_INVARIANT(snake_case); \
}

DEFINE_ADD_WIDGET_SIGNAL(realize)
DEFINE_ADD_WIDGET_SIGNAL(unrealize)
DEFINE_ADD_WIDGET_SIGNAL(destroy)
DEFINE_ADD_WIDGET_SIGNAL(hide)
DEFINE_ADD_WIDGET_SIGNAL(show)
DEFINE_ADD_WIDGET_SIGNAL(map)
DEFINE_ADD_WIDGET_SIGNAL(unmap)

template<typename T, typename Arg_t>
void add_widget_signals(Arg_t& type)
{
    add_signal_realize<T>(type);
    add_signal_unrealize<T>(type);
    add_signal_destroy<T>(type);
    add_signal_hide<T>(type);
    add_signal_show<T>(type);
    add_signal_map<T>(type);
    add_signal_unmap<T>(type);
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
DEFINE_ADD_SIGNAL_ARG3(key_pressed, bool, guint, keyval, guint, keycode, ModifierState, modifier) // TODO
DEFINE_ADD_SIGNAL_ARG3(key_released, void, guint, keyval, guint, keycode, ModifierState, modifier) // TODO
DEFINE_ADD_SIGNAL_ARG3(modifiers_changed, bool, guint, keyval, guint, keycode, ModifierState, modifier) // TODO
DEFINE_ADD_SIGNAL_ARG2(motion_enter, void, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG2(motion, void, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG0(motion_leave, void)
DEFINE_ADD_SIGNAL_ARG3(click_pressed, void, gint, n_press, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG3(click_released, void, gint, n_press, double, x, double, y)
DEFINE_ADD_SIGNAL_ARG0(click_stopped, void)
DEFINE_ADD_SIGNAL_ARG2(kinetic_scroll_decelerate, void, double, x_velocity, double, y_velocity)
DEFINE_ADD_SIGNAL_ARG0(scroll_begin, void)
DEFINE_ADD_SIGNAL_ARG2(scroll, bool, double, delta_x, double, delta_y)
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

// ### ADJUSTMENT

void implement_adjustment(jlcxx::Module& module)
{
    auto adjustment = module.add_type(Adjustment)
        .add_constructor(float, float, float, float)
        .add_type_method(Adjustment, get_lower)
        .add_type_method(Adjustment, get_upper)
        .add_type_method(Adjustment, get_value)
        .add_type_method(Adjustment, get_increment)
        .add_type_method(Adjustment, set_lower, !)
        .add_type_method(Adjustment, set_upper, !)
        .add_type_method(Adjustment, set_value, !)
        .add_type_method(Adjustment, set_increment, !)
    ;

    add_signal_value_changed<Adjustment>(adjustment);
    add_signal_properties_changed<Adjustment>(adjustment);
}

// ### ACTION

static void implement_action(jlcxx::Module& module)
{
    auto action = module.add_type(Action)
        .constructor( [](const std::string& id, void* app_ptr){
            return new Action(id, *((Application*) app_ptr));
        }, USE_FINALIZERS)
        .add_type_method(Action, get_id)
        .add_type_method(Action, set_state, !)
        .add_type_method(Action, get_state)
        .add_type_method(Action, activate)
        .add_type_method(Action, add_shortcut, !)
        .add_type_method(Action, get_shortcuts)
        .add_type_method(Action, clear_shortcuts, !)
        .add_type_method(Action, set_enabled, !)
        .add_type_method(Action, get_enabled)
        .add_type_method(Action, get_is_stateful)
    ;

    action.method("set_function!", [](Action& action, jl_function_t* task) {
        action.set_function([](Action& action, jl_function_t* task){
            jl_safe_call("Action::activate", task, jlcxx::box<Action&>(action));
        }, task);
    });

    action.method("set_stateful_function!", [](Action& action, jl_function_t* task) {
        action.set_stateful_function([](Action& action, bool b, jl_function_t* task) -> bool{
            auto* out = jl_safe_call("Action::activate", task, jlcxx::box<Action&>(action), jl_box_bool(b));
            if (out != nullptr)
                return (bool) jl_unbox_bool(out);
            else
                return false;
        }, task);
    });

}

// ### ALIGNMENT

static void implement_alignment(jlcxx::Module& module)
{
    define_enum_in(module, Alignment);
    module.add_enum_value(Alignment, ALIGNMENT, START);
    module.add_enum_value(Alignment, ALIGNMENT, CENTER);
    module.add_enum_value(Alignment, ALIGNMENT, END);
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
        .add_type_method(Application, mark_as_busy, !)
        .add_type_method(Application, unmark_as_busy, !)
        .add_type_method(Application, get_id)
        .add_type_method(Application, add_action)
        .add_type_method(Application, remove_action)
        .add_type_method(Application, has_action)
        .add_type_method(Application, get_action)
    ;

    add_signal_activate<Application>(application);
    add_signal_shutdown<Application>(application);
}

// ### ASPECT FRAME

static void implement_aspect_frame(jlcxx::Module& module)
{
    auto aspect_frame = module.add_type(AspectFrame)
        .constructor([](float ratio, float x_alignment, float y_alignment){
            return new AspectFrame(ratio, x_alignment, y_alignment);
        }, USE_FINALIZERS)
        .add_type_method(AspectFrame, set_ratio, !)
        .add_type_method(AspectFrame, get_ratio)
        .add_type_method(AspectFrame, set_child_x_alignment, !)
        .add_type_method(AspectFrame, set_child_y_alignment, !)
        .add_type_method(AspectFrame, get_child_x_alignment)
        .add_type_method(AspectFrame, get_child_y_alignment)
        .add_type_method(AspectFrame, remove_child, !)
        .method("set_child", [](AspectFrame& instance, void* widget){
            instance.set_child(*((Widget*) widget));
        });

    add_widget_signals<AspectFrame>(aspect_frame);
}

// ### BLEND MODE

static void implement_blend_mode(jlcxx::Module& module)
{
    define_enum_in(module, BlendMode);
    module.add_enum_value(BlendMode, BLEND_MODE, NONE);
    module.add_enum_value(BlendMode, BLEND_MODE, NORMAL);
    module.add_enum_value(BlendMode, BLEND_MODE, ADD);
    module.add_enum_value(BlendMode, BLEND_MODE, SUBTRACT);
    module.add_enum_value(BlendMode, BLEND_MODE, REVERSE_SUBTRACT);
    module.add_enum_value(BlendMode, BLEND_MODE, MULTIPLY);
    module.add_enum_value(BlendMode, BLEND_MODE, MIN);
    module.add_enum_value(BlendMode, BLEND_MODE, MAX);

    module.method("set_current_blend_mode", [](BlendMode mode, bool allow_alpha_blend){
        set_current_blend_mode(mode, allow_alpha_blend);
    });
}

// ### BOX

static void implement_box(jlcxx::Module& module)
{
    auto box = module.add_type(Box)
        .add_constructor(Orientation)
        .method("push_back!", [](Box& box, void* widget){
            box.push_back(*((Widget*) widget));
        })
        .method("push_front!", [](Box& box, void* widget){
            box.push_front(*((Widget*) widget));
        })
        .method("insert_after!", [](Box& box, void* to_append, void* after){
            box.insert_after(*((Widget*) to_append), *((Widget*) after));
        })
        .method("remove!", [](Box& box, void* to_remove) {
            box.remove(*((Widget*) to_remove));
        })
        .add_type_method(Box, clear)
        .add_type_method(Box, set_homogeneous, !)
        .add_type_method(Box, get_homogeneous)
        .add_type_method(Box, set_spacing, !)
        .add_type_method(Box, get_spacing)
        .add_type_method(Box, get_n_items)
        .add_type_method(Box, get_orientation)
        .add_type_method(Box, set_orientation, !)
    ;

    add_widget_signals<Box>(box);
}

// ### BUTTON

static void implement_button(jlcxx::Module& module)
{
    auto button = module.add_type(Button)
        .add_constructor()
        .add_type_method(Button, set_has_frame, !)
        .add_type_method(Button, get_has_frame)
        .add_type_method(Button, set_is_circular, !)
        .add_type_method(Button, get_is_circular)
        .method("set_child!", [](Button& button, void* widget){
            button.set_child(*((Widget*) widget));
        })
        .add_type_method(Button, remove_child, !)
        .add_type_method(Button, set_action, !)
    ;

    add_widget_signals<Button>(button);
    add_signal_activate<Button>(button);
    add_signal_clicked<Button>(button);
}

// ### CENTER BOX

static void implement_center_box(jlcxx::Module& module)
{
    auto center_box = module.add_type(CenterBox)
        .add_constructor(Orientation)
        .method("set_start_child!", [](CenterBox& box, void* widget) {
            box.set_start_child(*((Widget*) widget));
        })
        .method("set_center_child!", [](CenterBox& box, void* widget) {
            box.set_center_child(*((Widget*) widget));
        })
        .method("set_end_child!", [](CenterBox& box, void* widget) {
            box.set_end_child(*((Widget*) widget));
        })
        .add_type_method(CenterBox, remove_start_widget, !)
        .add_type_method(CenterBox, remove_center_widget, !)
        .add_type_method(CenterBox, remove_end_widget, !)
        .add_type_method(CenterBox, get_orientation)
        .add_type_method(CenterBox, set_orientation, !)
    ;

    add_widget_signals<CenterBox>(center_box);
}

// ### CHECK BUTTON

static void implement_check_button(jlcxx::Module& module)
{
    define_enum_in(module, CheckButtonState);
    module.add_enum_value(CheckButtonState, CHECK_BUTTON_STATE, ACTIVE);
    module.add_enum_value(CheckButtonState, CHECK_BUTTON_STATE, INCONSISTENT);
    module.add_enum_value(CheckButtonState, CHECK_BUTTON_STATE, INACTIVE);

    auto check_button = module.add_type(CheckButton)
        .add_constructor()
        .add_type_method(CheckButton, set_state, !)
        .add_type_method(CheckButton, get_state)
        .add_type_method(CheckButton, get_is_active)
    ;

    #if GTK_MINOR_VERSION >= 8
        check_button.method("set_child!", [](CheckButton& check_button, void* widget) {
            check_button.set_child(*((Widget*) widget));
        })
        .add_type_method(CheckButton, remove_child);
    #endif

    add_widget_signals<CheckButton>(check_button);
    add_signal_toggled<CheckButton>(check_button);
    add_signal_activate<CheckButton>(check_button);
}

// ### CLICK EVENT CONTROLLER

static void implement_click_event_controller(jlcxx::Module& module)
{
    auto click = module.add_type(ClickEventController)
        .add_constructor()
    ;

    add_signal_click_pressed<ClickEventController>(click);
    add_signal_click_released<ClickEventController>(click);
    add_signal_click_stopped<ClickEventController>(click);
}

// ### TODO

static void implement_clipboard(jlcxx::Module& module) {}

// ### COLOR

static void implement_color(jlcxx::Module& module)
{
    module.method("rgba_to_hsva", [](jl_value_t* rgba)
    {
        return box_hsva(rgba_to_hsva(unbox_rgba(rgba)));
    });

    module.method("hsva_to_rgba", [](jl_value_t* hsva)
    {
        return box_rgba(hsva_to_rgba(unbox_hsva(hsva)));
    });

    module.method("rgba_to_html_code", [](jl_value_t* rgba) -> std::string
    {
        return rgba_to_html_code(unbox_rgba(rgba));
    });

    module.method("html_code_to_rgba", [](const std::string& code) -> jl_value_t*
    {
        return box_rgba(html_code_to_rgba(code));
    });
}

// ### TODO

static void implement_column_view(jlcxx::Module& module) {}

// ### CURSOR_TYPE

static void implement_cursor_type(jlcxx::Module& module) 
{
    define_enum_in(module, CursorType);
    module.add_enum_value(CursorType, CURSOR_TYPE, NONE);
    module.add_enum_value(CursorType, CURSOR_TYPE, DEFAULT);
    module.add_enum_value(CursorType, CURSOR_TYPE, HELP);
    module.add_enum_value(CursorType, CURSOR_TYPE, POINTER);
    module.add_enum_value(CursorType, CURSOR_TYPE, CONTEXT_MENU);
    module.add_enum_value(CursorType, CURSOR_TYPE, PROGRESS);
    module.add_enum_value(CursorType, CURSOR_TYPE, WAIT);
    module.add_enum_value(CursorType, CURSOR_TYPE, CELL);
    module.add_enum_value(CursorType, CURSOR_TYPE, CROSSHAIR);
    module.add_enum_value(CursorType, CURSOR_TYPE, TEXT);
    module.add_enum_value(CursorType, CURSOR_TYPE, MOVE);
    module.add_enum_value(CursorType, CURSOR_TYPE, NOT_ALLOWED);
    module.add_enum_value(CursorType, CURSOR_TYPE, GRAB);
    module.add_enum_value(CursorType, CURSOR_TYPE, GRABBING);
    module.add_enum_value(CursorType, CURSOR_TYPE, ALL_SCROLL);
    module.add_enum_value(CursorType, CURSOR_TYPE, ZOOM_IN);
    module.add_enum_value(CursorType, CURSOR_TYPE, ZOOM_OUT);
    module.add_enum_value(CursorType, CURSOR_TYPE, COLUMN_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, ROW_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, NORTH_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, NORTH_EAST_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, EAST_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, SOUTH_EAST_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, SOUTH_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, SOUTH_WEST_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, WEST_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, NORTH_WEST_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, HORIZONTAL_RESIZE);
    module.add_enum_value(CursorType, CURSOR_TYPE, VERTICAL_RESIZE);
}

// ### DRAG EVENT CONTROLLER

static void implement_drag_event_controller(jlcxx::Module& module)
{
    auto drag = module.add_type(DragEventController)
        .constructor()
        .method("get_start_position", [](DragEventController& controller) -> jl_value_t* {
            return box_vector2f(controller.get_start_position());
        })
        .method("get_current_offset", [](DragEventController& controller) -> jl_value_t* {
            return box_vector2f(controller.get_current_offset());
        })
    ;

    add_signal_drag_begin<DragEventController>(drag);
    add_signal_drag<DragEventController>(drag);
    add_signal_drag_end<DragEventController>(drag);
}

// ### TODO

static void implement_drop_down(jlcxx::Module& module) {}

// ### ENTRY

static void implement_entry(jlcxx::Module& module)
{
    auto entry = module.add_type(Entry)
        .constructor()
        .add_type_method(Entry, get_text)
        .add_type_method(Entry, set_text, !)
        .add_type_method(Entry, set_max_length, !)
        .add_type_method(Entry, get_max_length)
        .add_type_method(Entry, set_has_frame, !)
        .add_type_method(Entry, get_has_frame)
        .add_type_method(Entry, set_text_visible, !)
        .add_type_method(Entry, get_text_visible)
        .add_type_method(Entry, set_primary_icon, !)
        .add_type_method(Entry, remove_primary_icon)
        .add_type_method(Entry, set_secondary_icon, !)
        .add_type_method(Entry, remove_secondary_icon)
    ;

    add_widget_signals<Entry>(entry);
    add_signal_activate<Entry>(entry);
    add_signal_text_changed<Entry>(entry);
}

// ### EVENT CONTROLLER

static void implement_event_controller(jlcxx::Module& module)
{
    define_enum_in(module, PropagationPhase);
    module.add_enum_value(PropagationPhase, PROPAGATION_PHASE, NONE);
    module.add_enum_value(PropagationPhase, PROPAGATION_PHASE, CAPTURE);
    module.add_enum_value(PropagationPhase, PROPAGATION_PHASE, BUBBLE);
    module.add_enum_value(PropagationPhase, PROPAGATION_PHASE, TARGET);

    module.method("set_propagation_phase!", [](void* controller, PropagationPhase phase) {
        ((EventController*) controller)->set_propagation_phase(phase);
    });
    module.method("get_propagation_phase", [](void* controller) {
        return ((EventController*) controller)->get_propagation_phase();
    });
   
    define_enum_in(module, ButtonID);
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
    
    module.method("get_current_button", [](void* gesture) -> ButtonID {
        return ((SingleClickGesture*) gesture)->get_current_button();
    });
    module.method("set_only_listens_to_button!", [](void* gesture, ButtonID id) {
        ((SingleClickGesture*) gesture)->set_only_listens_to_button(id);
    });
    module.method("get_only_listens_to_button", [](void* gesture) -> ButtonID {
        return ((SingleClickGesture*) gesture)->get_only_listens_to_button();
    });
    module.method("get_touch_only", [](void* gesture) -> bool {
        return ((SingleClickGesture*) gesture)->get_touch_only();
    });
    module.method("set_touch_only!", [](void* gesture, bool b){
        ((SingleClickGesture*) gesture)->set_touch_only(b);
    });
}

// ### EXPANDER

static void implement_expander(jlcxx::Module& module)
{
    auto expander = module.add_type(Expander)
        .method("set_child!", [](Expander& x, void* widget) {
            x.set_child(*((Widget*) widget));
        })
        .add_type_method(Expander, remove_child, !)
        .method("set_label_widget!", [](Expander& x, void* widget) {
            x.set_label_widget(*((Widget*) widget));
        })
        .add_type_method(Expander, remove_label_widget, !)
        .add_type_method(Expander, get_expanded)
        .add_type_method(Expander, set_expanded)
    ;

    add_widget_signals<Expander>(expander);
    add_signal_activate<Expander>(expander);
}

// ### TODO

static void implement_file_chooser(jlcxx::Module& module) {}

// ### TODO

static void implement_file_descriptor(jlcxx::Module& module) {}

// ### TODO

static void implement_file_monitor(jlcxx::Module& module) {}

// ### TODO

static void implement_file_system(jlcxx::Module& module) {}

// ### FIXED

static void implement_fixed(jlcxx::Module& module)
{
    auto fixed = module.add_type(Fixed)
        .add_constructor()
        .method("add_child!", [](Fixed& x, void* widget, jl_value_t* vector2f){
            x.add_child(*((Widget*) widget), unbox_vector2f(vector2f));
        })
        .method("remove_child!", [](Fixed& x, void* widget){
            x.remove_child(*((Widget*) widget));
        })
        .method("set_child_position!", [](Fixed& x, void* widget, jl_value_t* vector2f){
            x.set_child_position(*((Widget*) widget), unbox_vector2f(vector2f));
        })
        .method("get_child_position", [](Fixed& x, void* widget) -> jl_value_t* {
            return box_vector2f(x.get_child_position(*((Widget*) widget)));
        })
    ;

    add_widget_signals<Fixed>(fixed);
}

// ### FOCUS EVENT CONTROLLER

static void implement_focus_event_controller(jlcxx::Module& module)
{
    auto focus = module.add_type(FocusEventController)
        .add_constructor()
        .add_type_method(FocusEventController, self_or_child_is_focused)
        .add_type_method(FocusEventController, self_is_focused)
    ;

    add_signal_focus_gained<FocusEventController>(focus);
    add_signal_focus_lost<FocusEventController>(focus);
}

// ### FRAME

static void implement_frame(jlcxx::Module& module)
{
    auto frame = module.add_type(Frame)
        .add_constructor()
        .method("set_child!", [](Frame& x, void* child) {
            x.set_child(*((Widget*) child));
        })
        .add_type_method(Frame, remove_child)
        .method("set_label_widget!", [](Frame& x, void* label){
            x.set_label_widget(*((Widget*) label));
        })
        .add_type_method(Frame, remove_label_widget, !)
        .add_type_method(Frame, set_label_x_alignment, !)
        .add_type_method(Frame, get_label_x_alignment)
    ;

    add_widget_signals<Frame>(frame);
}

// ### TODO

static void implement_frame_clock(jlcxx::Module& module) {}

// ### TODO

static void implement_geometry(jlcxx::Module& module) {}

// ### TODO

static void implement_gl_common(jlcxx::Module& module) {}

// ### TODO

static void implement_gl_transform(jlcxx::Module& module) {}

// ### TODO

static void implement_grid(jlcxx::Module& module) {}

// ### TODO

static void implement_grid_view(jlcxx::Module& module) {}

// ### TODO

static void implement_header_bar(jlcxx::Module& module) {}

// ### ICON

static void implement_icon(jlcxx::Module& module)
{
    module.add_type(Icon)
        .add_constructor()
        .add_type_method(Icon, create_from_file, !)
        .method("create_from_theme!", [](Icon& icon, void* theme, IconID id, size_t square_resolution, size_t scale){
            return icon.create_from_theme(*((IconTheme*) theme), id, square_resolution, scale);
        })
        .add_type_method(Icon, get_name)
        .add_type_method(Icon, get_scale)
        .method("get_size", [](const Icon& icon) -> jl_value_t*{
            return box_vector2i(icon.get_size());
        })
    ;

    module.method("compare_icons", [](const Icon& a, const Icon& b) -> bool {
        return a == b;
    });

    module.add_type(IconTheme)
        .add_constructor(const Window&)
        .add_type_method(IconTheme, get_icon_names)
        .method("has_icon_icon", [](const IconTheme& theme, const Icon& icon) -> bool {
            return theme.has_icon(icon);
        })
        .method("has_icon_icon_id", [](const IconTheme& theme, const IconID& id) -> bool {
            return theme.has_icon(id);
        })
        .add_type_method(IconTheme, add_resource_path, !)
        .add_type_method(IconTheme, set_resource_path, !)
    ;
}

// ### IMAGE

static void implement_image(jlcxx::Module& module)
{
    define_enum_in(module, InterpolationType);
    module.add_enum_value(InterpolationType, INTERPOLATION_TYPE, NEAREST);
    module.add_enum_value(InterpolationType, INTERPOLATION_TYPE, TILES);
    module.add_enum_value(InterpolationType, INTERPOLATION_TYPE, BILINEAR);
    module.add_enum_value(InterpolationType, INTERPOLATION_TYPE, HYPERBOLIC);

    module.add_type(Image)
        .add_constructor()
        .add_constructor(const std::string&)
        .constructor([](size_t width, size_t height, jl_value_t* rgba){
            return new Image(width, height, unbox_rgba(rgba));
        }, USE_FINALIZERS)
        .method("create!", [](Image& image, size_t width, size_t height, jl_value_t* rgba){
            image.create(width, height, unbox_rgba(rgba));
        })
        .add_type_method(Image, create_from_file, !)
        .add_type_method(Image, save_to_file)
        .add_type_method(Image, get_n_pixels)
        .method("get_size", [](const Image& image) -> jl_value_t* {
            auto size = image.get_size();
            return box_vector2i(Vector2i(size.x, size.y));
        })
        .add_type_method(Image, as_scaled)
        .add_type_method(Image, as_cropped)
        .method("set_pixel_rgba!", [](Image& image, size_t x, size_t y, jl_value_t* rgba){
            image.set_pixel(x, y, unbox_rgba(rgba));
        })
        .method("set_pixel_hsva!", [](Image& image, size_t x, size_t y, jl_value_t* hsva){
            image.set_pixel(x, y, unbox_hsva(hsva));
        })
        .method("get_pixel", [](const Image& image, size_t x, size_t y) -> jl_value_t* {
            return box_rgba(image.get_pixel(x, y));
        })
    ;
}

// ### IMAGE DISPLAY

static void implement_image_display(jlcxx::Module& module)
{
    auto image_display = module.add_type(ImageDisplay)
        .add_constructor(const std::string&)
        .add_constructor(const Image&)
        .add_constructor(const Icon&)
        .method("get_size", [](ImageDisplay& x) -> jl_value_t* {
            return box_vector2f(x.get_size());
        })
        .add_type_method(ImageDisplay, create_from_file, !)
        .add_type_method(ImageDisplay, create_from_image, !)
        .add_type_method(ImageDisplay, create_from_icon, !)
        // TODO: .add_type_method(ImageDisplay, create_as_file_preview, !)
        .add_type_method(ImageDisplay, clear, !)
        .add_type_method(ImageDisplay, set_scale, !)
    ;

    add_widget_signals<ImageDisplay>(image_display);
}

// ### JUSTIFY MODE

static void implement_justify_mode(jlcxx::Module& module)
{
    define_enum_in(module, JustifyMode);
    module.add_enum_value(JustifyMode, JUSTIFY_MODE, LEFT);
    module.add_enum_value(JustifyMode, JUSTIFY_MODE, RIGHT);
    module.add_enum_value(JustifyMode, JUSTIFY_MODE, CENTER);
    module.add_enum_value(JustifyMode, JUSTIFY_MODE, FILL);
}

// ### KEY EVENT CONTROLLER

static void export_key_codes(jlcxx::Module& module);

static void implement_key_event_controller(jlcxx::Module& module)
{
    module.add_bits<ModifierState>("_ModifierState", jl_int32_type);
    export_key_codes(module);

    auto key = module.add_type(KeyEventController)
        .add_constructor()
        .add_type_method(KeyEventController, should_shortcut_trigger_trigger)
    ;

    add_signal_key_pressed<KeyEventController>(key);
    add_signal_key_released<KeyEventController>(key);
    add_signal_modifiers_changed<KeyEventController>(key);

    module.method("shift_pressed", [](ModifierState state) -> bool {
        return KeyEventController::shift_pressed(state);
    });

    module.method("control_pressed", [](ModifierState state) -> bool {
        return KeyEventController::shift_pressed(state);
    });

    module.method("alt_pressed", [](ModifierState state) -> bool {
        return KeyEventController::shift_pressed(state);
    });

    module.method("mouset_button_01_pressed", [](ModifierState state) -> bool {
        return KeyEventController::shift_pressed(state);
    });

    module.method("mouset_button_02_pressed", [](ModifierState state) -> bool {
        return KeyEventController::shift_pressed(state);
    });
}

// ### KEY FILE

static void implement_key_file(jlcxx::Module& module)
{
    auto key_file = module.add_type(KeyFile)
        .add_constructor()
        .add_constructor(const std::string&)
        .add_type_method(KeyFile, as_string)
        .add_type_method(KeyFile, create_from_file, !)
        .add_type_method(KeyFile, create_from_string, !)
        .add_type_method(KeyFile, save_to_file)
        .add_type_method(KeyFile, get_groups)
        .add_type_method(KeyFile, get_keys)
        .add_type_method(KeyFile, has_key)
        .add_type_method(KeyFile, has_group)
        .add_type_method(KeyFile, set_comment_above_group, !)
        .add_type_method(KeyFile, set_comment_above_key, !)
        .add_type_method(KeyFile, get_comment_above_group)
        .add_type_method(KeyFile, get_comment_above_key)
    ;

    #define define_get_value_as(type, name) \
        key_file.method("get_value_as_" + std::string(name), [](KeyFile& file, KeyFile::GroupID group_id, KeyFile::KeyID key_id) { \
            return file.get_value_as<type>(group_id, key_id); \
        });

    #define define_set_value_as(type, name) \
        key_file.method("set_value_as_" + std::string(name) + "!", [](KeyFile& file, KeyFile::GroupID group_id, KeyFile::KeyID key_id, type value) { \
            return file.set_value_as<type>(group_id, key_id, value); \
        });

    #define define_get_value_as_list(type, name)\
        key_file.method("get_value_as_" + std::string(name) + "_list", [](KeyFile& file, KeyFile::GroupID group_id, KeyFile::KeyID key_id) { \
            return file.get_value_as<std::vector<type>>(group_id, key_id); \
        });

    #define define_set_value_as_list(type, name)\
        key_file.method("set_value_as_" + std::string(name) + "_list!", [](KeyFile& file, KeyFile::GroupID group_id, KeyFile::KeyID key_id, jl_value_t* in) { \
            std::vector<type> vec; \
            for (size_t i = 0; i < jl_array_len(in); ++i) \
                vec.push_back(jlcxx::unbox<type>(jl_arrayref((jl_array_t*) in, i))); \
            file.set_value_as<std::vector<type>>(group_id, key_id, vec); \
        });

    define_get_value_as(std::string, "string");
    define_set_value_as(std::string, "string");

    define_get_value_as(bool, "bool");
    define_set_value_as(bool, "bool");

    define_get_value_as(int, "int");
    define_set_value_as(int, "int");

    define_get_value_as(size_t, "uint");
    define_set_value_as(size_t, "uint");

    define_get_value_as(float, "float");
    define_set_value_as(float, "float");

    define_get_value_as(double, "double");
    define_set_value_as(double, "double");

    define_get_value_as_list(std::string, "string");

    key_file.method("set_value_as_string_list!", [](KeyFile& file, KeyFile::GroupID group, KeyFile::KeyID key, jl_value_t* list){

        auto vec = std::vector<std::string>();
        for (size_t i = 0; i < jl_array_len(list); ++i)
        {
            auto* ptr = jl_string_ptr(jl_arrayref((jl_array_t*) list, i));
            if (ptr != nullptr)
                vec.emplace_back(ptr);
        }

        file.set_value_as<std::vector<std::string>>(group, key, vec);
    });

    define_get_value_as_list(bool, "bool");
    define_set_value_as_list(bool, "bool");

    define_get_value_as_list(int, "int");
    define_set_value_as_list(int, "int");

    define_get_value_as_list(size_t, "uint");
    define_set_value_as_list(size_t, "uint");

    define_get_value_as_list(float, "float");
    define_set_value_as_list(float, "float");

    define_get_value_as_list(double, "double");
    define_set_value_as_list(double, "double");

    // TODO _as_image
    // TODO _as_rgba

    #undef define_access_value_as
}

// ### LABEL

static void implement_label(jlcxx::Module& module)
{
    define_enum_in(module, EllipsizeMode)
    module.add_enum_value(EllipsizeMode, ELLIPSIZE_MODE, NONE);
    module.add_enum_value(EllipsizeMode, ELLIPSIZE_MODE, START);
    module.add_enum_value(EllipsizeMode, ELLIPSIZE_MODE, MIDDLE);
    module.add_enum_value(EllipsizeMode, ELLIPSIZE_MODE, END);

    define_enum_in(module, LabelWrapMode)
    module.add_enum_value(LabelWrapMode, LABEL_WRAP_MODE, NONE);
    module.add_enum_value(LabelWrapMode, LABEL_WRAP_MODE, ONLY_ON_WORD);
    module.add_enum_value(LabelWrapMode, LABEL_WRAP_MODE, ONLY_ON_CHAR);
    module.add_enum_value(LabelWrapMode, LABEL_WRAP_MODE, WORD_OR_CHAR);

    auto label = module.add_type(Label)
        .add_constructor()
        .add_constructor(const std::string&)
        .add_type_method(Label, set_text, !)
        .add_type_method(Label, get_text)
        .add_type_method(Label, set_use_markup, !)
        .add_type_method(Label, get_use_markup)
        .add_type_method(Label, set_ellipsize_mode, !)
        .add_type_method(Label, get_ellipsize_mode)
        .add_type_method(Label, set_wrap_mode, !)
        .add_type_method(Label, get_wrap_mode)
        .add_type_method(Label, set_justify_mode, !)
        .add_type_method(Label, get_justify_mode)
        .add_type_method(Label, set_max_width_chars, !)
        .add_type_method(Label, get_max_width_chars)
        .add_type_method(Label, set_x_alignment, !)
        .add_type_method(Label, get_x_alignment)
        .add_type_method(Label, set_y_alignment, !)
        .add_type_method(Label, get_y_alignment)
        .add_type_method(Label, set_selectable, !)
        .add_type_method(Label, get_selectable)
    ;

    add_widget_signals<Label>(label);
}

// ### LEVEL BAR

static void implement_level_bar(jlcxx::Module& module)
{
    define_enum_in(module, LevelBarMode);
    module.add_enum_value(LevelBarMode, LEVEL_BAR_MODE, CONTINUOUS);
    module.add_enum_value(LevelBarMode, LEVEL_BAR_MODE, DISCRETE);

    auto level_bar = module.add_type(LevelBar)
        .add_constructor(float, float)
        .add_type_method(LevelBar, add_marker, !)
        .add_type_method(LevelBar, remove_marker, !)
        .add_type_method(LevelBar, set_inverted, !)
        .add_type_method(LevelBar, get_inverted)
        .add_type_method(LevelBar, set_mode, !)
        .add_type_method(LevelBar, get_mode)
        .add_type_method(LevelBar, set_min_value, !)
        .add_type_method(LevelBar, get_min_value)
        .add_type_method(LevelBar, set_max_value, !)
        .add_type_method(LevelBar, get_max_value)
        .add_type_method(LevelBar, set_value, !)
        .add_type_method(LevelBar, get_value)
        .add_type_method(LevelBar, set_orientation, !)
        .add_type_method(LevelBar, get_orientation)
    ;

    add_widget_signals<LevelBar>(level_bar);
}

// ### TODO

static void implement_list_view(jlcxx::Module& module) {}

// ### LOG

static void implement_log(jlcxx::Module& module)
{
    define_enum_in(module, LogLevel);
    module.add_enum_value(LogLevel, LOG_LEVEL, DEBUG);
    module.add_enum_value(LogLevel, LOG_LEVEL, INFO);
    module.add_enum_value(LogLevel, LOG_LEVEL, WARNING);
    module.add_enum_value(LogLevel, LOG_LEVEL, CRITICAL);
    module.add_enum_value(LogLevel, LOG_LEVEL, FATAL);

    module.set_const("MOUSETRAP_DOMAIN", std::string(MOUSETRAP_DOMAIN));
    module.set_const("MOUSETRAP_JULIA_DOMAIN", std::string("mousetrap_jl"));

    module.method("log_debug", [](const std::string& message, LogDomain domain){
        log::debug(message, domain);
    });

    module.method("log_info", [](const std::string& message, LogDomain domain){
        log::info(message, domain);
    });

    module.method("log_warning", [](const std::string& message, LogDomain domain){
        log::warning(message, domain);
    });

    module.method("log_critical", [](const std::string& message, LogDomain domain){
        log::critical(message, domain);
    });

    module.method("log_fatal", [](const std::string& message, LogDomain domain){
        log::fatal(message, domain);
    });

    module.method("log_set_surpress_debug", [](LogDomain domain, bool b)
    {
        log::set_surpress_debug(domain, b);
    });

    module.method("log_set_surpress_info", [](LogDomain domain, bool b)
    {
        log::set_surpress_info(domain, b);
    });

    module.method("log_get_surpress_debug", [](LogDomain domain)
    {
        return log::get_surpress_debug(domain);
    });

    module.method("log_get_surpress_info", [](LogDomain domain)
    {
        return log::get_surpress_info(domain);
    });

    module.method("log_set_file", [](const std::string& path){
        log::set_file(path);
    });

    /*
    module.method("log_reset_file_formatting_function", [](){
       log::reset_file_formatting_function();
    });

    module.method("log_set_file_formatting_function", [](jl_value_t* task){
        log::set_file_formatting_function([](const std::string& message, const std::map<std::string, std::string>& fields){

            jl_gc_enable(false);

            static auto* dict_ctor = jl_eval_string("Dict{String, String}");
            static auto* setindex = jl_get_function(jl_base_module, "setindex!");

            auto* message_jl = jl_box_string(message);
            auto* dict = jl_calln(dict_ctor)
            for (auto& pair : fields)
                jl_calln(setindex, dict, jl_box_string(pair.first), jl_box_string(pair.second));

            auto* res = jl_safe_call("log::formatting_function", task, message, dict);

            std::string out;
            if (res != nullptr)
                out = jl_unbox_string(res);

            jl_gc_enable(true);
            return out;
        }, task);
    });
     */
}

// ### LONG PRESS EVENT CONTROLLER

static void implement_long_press_event_controller(jlcxx::Module& module)
{
    auto long_press = module.add_type(LongPressEventController)
        .add_type_method(LongPressEventController, set_delay_factor)
        .add_type_method(LongPressEventController, get_delay_factor)
    ;

    add_signal_pressed<LongPressEventController>(long_press);
    add_signal_press_cancelled<LongPressEventController>(long_press);
}

// ### MENU MODEL

enum SectionFormat
{
    NORMAL = MenuModel::SectionFormat::NORMAL,
    HORIZONTAL_BUTTONS = MenuModel::SectionFormat::HORIZONTAL_BUTTONS,
    HORIZONTAL_BUTTONS_LEFT_TO_RIGHT = MenuModel::SectionFormat::HORIZONTAL_BUTTONS_LEFT_TO_RIGHT,
    HORIZONTAL_BUTTONS_RIGHT_TO_LEFT = MenuModel::SectionFormat::HORIZONTAL_BUTTONS_RIGHT_TO_LEFT,
    CIRCULAR_BUTTONS = MenuModel::SectionFormat::CIRCULAR_BUTTONS,
    INLINE_BUTTONS = MenuModel::SectionFormat::INLINE_BUTTONS
};

static void implement_menu_model(jlcxx::Module& module)
{
    define_enum_in(module, SectionFormat);
    module.add_enum_value(SectionFormat, SECTION_FORMAT, NORMAL);
    module.add_enum_value(SectionFormat, SECTION_FORMAT, HORIZONTAL_BUTTONS);
    module.add_enum_value(SectionFormat, SECTION_FORMAT, HORIZONTAL_BUTTONS_LEFT_TO_RIGHT);
    module.add_enum_value(SectionFormat, SECTION_FORMAT, HORIZONTAL_BUTTONS_RIGHT_TO_LEFT);
    module.add_enum_value(SectionFormat, SECTION_FORMAT, CIRCULAR_BUTTONS);
    module.add_enum_value(SectionFormat, SECTION_FORMAT, INLINE_BUTTONS);

    auto menu_model = module.add_type(MenuModel)
        .add_constructor()
        .add_type_method(MenuModel, add_action, !)
        .method("add_widget!", [](MenuModel& model, void* widget){
            model.add_widget(*((Widget*) widget));
        })
        .method("add_section!", [](MenuModel& model, const std::string& label, const MenuModel& other, SectionFormat format){
            model.add_section(label, other, (MenuModel::SectionFormat) format);
        })
        .add_type_method(MenuModel, add_submenu, !)
        .add_type_method(MenuModel, add_icon, !)
    ;

    add_signal_items_changed<MenuModel>(menu_model);
}

// ### MENU BAR

static void implement_menu_bar(jlcxx::Module& module) 
{
    auto menubar = module.add_type(MenuBar)
        .add_constructor(const MenuModel&)
    ;

    add_widget_signals<MenuBar>(menubar);
}

// ### MOTION EVENT CONTROLLER

static void implement_motion_event_controller(jlcxx::Module& module)
{
    auto motion = module.add_type(MotionEventController)
        .add_constructor()
    ;

    add_signal_motion_enter<MotionEventController>(motion);
    add_signal_motion<MotionEventController>(motion);
    add_signal_motion_leave<MotionEventController>(motion);
}

// ### TODO

static void implement_msaa_render_texture(jlcxx::Module& module) {}

// ### TODO

static void implement_music(jlcxx::Module& module) {}

// ### TODO

static void implement_notebook(jlcxx::Module& module) {}

// ### ORIENTATION

static void implement_orientation(jlcxx::Module& module)
{
    define_enum_in(module, Orientation);
    module.add_enum_value(Orientation, ORIENTATION, HORIZONTAL);
    module.add_enum_value(Orientation, ORIENTATION, VERTICAL);
}

// ### OVERLAY

static void implement_overlay(jlcxx::Module& module)
{
    auto overlay = module.add_type(Overlay)
        .add_constructor()
        .method("set_child!", [](Overlay& overlay, void* child){
            overlay.set_child(*((Widget*) child));
        })
        .add_type_method(Overlay, remove_child)
        .method("add_overlay!", [](Overlay& overlay, void* widget, bool include_in_measurement, bool clip){
            overlay.add_overlay(*((Widget*) widget), include_in_measurement, clip);
        })
        .method("remove_overlay!", [](Overlay& overlay, void* child){
            overlay.remove_overlay(*((Widget*) child));
        })
    ;

    add_widget_signals<Overlay>(overlay);
}

// ### PAN EVENT CONTROLLER

static void implement_pan_event_controller(jlcxx::Module& module)
{
    define_enum_in(module, PanDirection);
    module.add_enum_value(PanDirection, PAN_DIRECTION, LEFT);
    module.add_enum_value(PanDirection, PAN_DIRECTION, RIGHT);
    module.add_enum_value(PanDirection, PAN_DIRECTION, UP);
    module.add_enum_value(PanDirection, PAN_DIRECTION, DOWN);

    auto pan = module.add_type(PanEventController)
        .add_constructor(Orientation)
        .add_type_method(PanEventController, set_orientation, !)
        .add_type_method(PanEventController, get_orientation)
    ;

    add_signal_pan<PanEventController>(pan);
}

// ### TODO

static void implement_paned(jlcxx::Module& module) {}

// ### PINCH ZOOM EVENT CONTROLLER

static void implement_pinch_zoom_event_controller(jlcxx::Module& module)
{
    auto pinch = module.add_type(PinchZoomEventController)
        .add_constructor()
        .add_type_method(PinchZoomEventController, get_scale_delta)
    ;

    add_signal_scale_changed<PinchZoomEventController>(pinch);
}

// ### POPOVER

static void implement_popover(jlcxx::Module& module)
{
    auto popover = module.add_type(Popover)
        .add_type_method(Popover, popup, !)
        .add_type_method(Popover, popdown, !)
        .add_type_method(Popover, present, !)
        .method("set_child!", [](Popover& popover, void* widget){
            popover.set_child(*((Widget*) widget));
        })
        .add_type_method(Popover, remove_child, !)
        .method("attach_to!", [](Popover& popover, void* widget){
            popover.attach_to(*((Widget*) widget));
        })
        .add_type_method(Popover, set_relative_position, !)
        .add_type_method(Popover, get_relative_position)
        .add_type_method(Popover, set_autohide, !)
        .add_type_method(Popover, get_autohide)
        .add_type_method(Popover, set_has_base_arrow, !)
        .add_type_method(Popover, get_has_base_arrow)
    ;

    add_widget_signals<Popover>(popover);
    add_signal_closed<Popover>(popover);
}

// ### POPOVER BUTTON

static void implement_popover_button(jlcxx::Module& module)
{
    auto popover_button = module.add_type(PopoverButton)
        .add_constructor()
        .method("set_child!", [](PopoverButton& x, void* widget){
            x.set_child(*((Widget*) widget));
        })
        .add_type_method(PopoverButton, remove_child, !)
        .add_type_method(PopoverButton, set_popover, !)
        .add_type_method(PopoverButton, set_popover_menu, !)
        .add_type_method(PopoverButton, set_popover_position, !)
        .add_type_method(PopoverButton, get_popover_position)
        .add_type_method(PopoverButton, remove_popover, !)
        .add_type_method(PopoverButton, set_always_show_arrow, !)
        .add_type_method(PopoverButton, get_always_show_arrow)
        .add_type_method(PopoverButton, set_has_frame, !)
        .add_type_method(PopoverButton, get_has_frame)
        .add_type_method(PopoverButton, popup, !)
        .add_type_method(PopoverButton, popdown, !)
        .add_type_method(PopoverButton, set_is_circular, !)
        .add_type_method(PopoverButton, get_is_circular)
    ;

    add_widget_signals<PopoverButton>(popover_button);
    add_signal_activate<PopoverButton>(popover_button);
}

// ### POPOVER MENU

static void implement_popover_menu(jlcxx::Module& module)
{
    auto popover_menu = module.add_type(PopoverMenu)
        .add_constructor(const MenuModel&)
    ;

    add_widget_signals<PopoverMenu>(popover_menu);
    add_signal_closed<PopoverMenu>(popover_menu);
}

// ### TODO

static void implement_progress_bar(jlcxx::Module& module) {}

// ### RELATIVE POSITION

static void implement_relative_position(jlcxx::Module& module)
{
    define_enum_in(module, RelativePosition);
    module.add_enum_value(RelativePosition, RELATIVE_POSITION, ABOVE);
    module.add_enum_value(RelativePosition, RELATIVE_POSITION, LEFT_OF);
    module.add_enum_value(RelativePosition, RELATIVE_POSITION, RIGHT_OF);
    module.add_enum_value(RelativePosition, RELATIVE_POSITION, BELOW);
}

// ### TODO

static void implement_render_area(jlcxx::Module& module) {}

// ### TODO

static void implement_render_task(jlcxx::Module& module) {}

// ### TODO

static void implement_render_texture(jlcxx::Module& module) {}

// ### TODO

static void implement_revealer(jlcxx::Module& module) {}

// ### ROTATE EVENT CONTROLLER

static void implement_rotate_event_controller(jlcxx::Module& module)
{
    auto rotate = module.add_type(RotateEventController)
        .add_constructor()
        .method("get_angle_deta", [](RotateEventController& x){
            return x.get_angle_delta().as_radians();
        })
    ;

    add_signal_rotation_changed<RotateEventController>(rotate);
}

// ### TODO

static void implement_scale(jlcxx::Module& module) {}

// ### TODO

static void implement_scale_mode(jlcxx::Module& module) {}

// ### SCROLL EVENT CONTROLLER

static void implement_scroll_event_controller(jlcxx::Module& module)
{
    auto scroll = module.add_type(ScrollEventController)
        .add_constructor(bool, bool)
    ;

    add_signal_kinetic_scroll_decelerate<ScrollEventController>(scroll);
    add_signal_scroll_begin<ScrollEventController>(scroll);
    add_signal_scroll<ScrollEventController>(scroll);
    add_signal_scroll_end<ScrollEventController>(scroll);
}

// ### TODO

static void implement_scrollbar(jlcxx::Module& module) {}

// ### TODO

static void implement_selection_model(jlcxx::Module& module) {}

// ### TODO

static void implement_separator(jlcxx::Module& module) {}

// ### TODO

static void implement_shader(jlcxx::Module& module) {}

// ### TODO

static void implement_shape(jlcxx::Module& module) {}

// ### SHORTCUT EVENT CONTROLLER

static void implement_shortcut_event_controller(jlcxx::Module& module)
{
    define_enum_in(module, ShortcutScope);
    module.add_enum_value(ShortcutScope, SHORTCUT_SCOPE, LOCAL);
    module.add_enum_value(ShortcutScope, SHORTCUT_SCOPE, MANAGED);
    module.add_enum_value(ShortcutScope, SHORTCUT_SCOPE, GLOBAL);

    module.add_type(ShortcutEventController)
        .add_constructor()
        .add_type_method(ShortcutEventController, add_action, !)
        .add_type_method(ShortcutEventController, set_scope, !)
        .add_type_method(ShortcutEventController, get_scope)
    ;
}

// ### TODO

static void implement_sound(jlcxx::Module& module) {}

// ### TODO

static void implement_sound_buffer(jlcxx::Module& module) {}

// ### TODO

static void implement_spin_button(jlcxx::Module& module) {}

// ### TODO

static void implement_spinner(jlcxx::Module& module) {}

// ### TODO

static void implement_stack(jlcxx::Module& module) {}

// ### STYLUS EVENT CONTROLLER

static void implement_stylus_event_controller(jlcxx::Module& module) 
{
    define_enum_in(module, ToolType);
    module.add_enum_value(ToolType, TOOL_TYPE, UNKNOWN);
    module.add_enum_value(ToolType, TOOL_TYPE, PEN);
    module.add_enum_value(ToolType, TOOL_TYPE, ERASER);
    module.add_enum_value(ToolType, TOOL_TYPE, BRUSH);
    module.add_enum_value(ToolType, TOOL_TYPE, PENCIL);
    module.add_enum_value(ToolType, TOOL_TYPE, AIRBRUSH);
    module.add_enum_value(ToolType, TOOL_TYPE, LENS);
    module.add_enum_value(ToolType, TOOL_TYPE, MOUSE);
   
    define_enum_in(module, DeviceAxis);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, X);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, Y);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, DELTA_X);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, DELTA_Y);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, PRESSURE);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, X_TILT);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, Y_TILT);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, WHEEL);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, DISTANCE);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, ROTATION);
    module.add_enum_value(DeviceAxis, DEVICE_AXIS, SLIDER);

    module.method("device_axis_to_string", [](DeviceAxis axis) -> std::string {
        return device_axis_to_string(axis);
    });

    auto stylus = module.add_type(StylusEventController)
        .add_constructor()
        .add_type_method(StylusEventController, get_hardware_id)
        .add_type_method(StylusEventController, get_device_type)
        .add_type_method(StylusEventController, has_axis)
        .add_type_method(StylusEventController, get_axis_value)
    ;

    add_signal_stylus_down<StylusEventController>(stylus);
    add_signal_stylus_up<StylusEventController>(stylus);
    add_signal_proximity<StylusEventController>(stylus);
    add_signal_motion<StylusEventController>(stylus);
}

// ### SWIPE EVENT CONTROLLER

static void implement_swipe_event_controller(jlcxx::Module& module)
{
    auto swipe = module.add_type(SwipeEventController)
        .add_constructor(Orientation)
        .method("get_velocity", [](SwipeEventController& x) -> jl_value_t* {
            return box_vector2f(x.get_velocity());
        });

    add_signal_swipe<SwipeEventController>(swipe);
}

// ### TODO

static void implement_switch(jlcxx::Module& module) {}

// ### TEXT VIEW

static void implement_text_view(jlcxx::Module& module)
{
    auto text_view = module.add_type(TextView)
        .add_constructor()
        .add_type_method(TextView, get_text)
        .add_type_method(TextView, set_text, !)
        .add_type_method(TextView, set_cursor_visible, !)
        .add_type_method(TextView, get_cursor_visible)
        .add_type_method(TextView, undo, !)
        .add_type_method(TextView, redo, !)
        .add_type_method(TextView, set_was_modified, !)
        .add_type_method(TextView, get_was_modified)
        .add_type_method(TextView, set_editable, !)
        .add_type_method(TextView, get_editable)
        .add_type_method(TextView, set_justify_mode, !)
        .add_type_method(TextView, get_justify_mode)
        .add_type_method(TextView, set_left_margin, !)
        .add_type_method(TextView, get_left_margin)
        .add_type_method(TextView, set_right_margin, !)
        .add_type_method(TextView, get_right_margin)
        .add_type_method(TextView, set_top_margin, !)
        .add_type_method(TextView, get_top_margin)
        .add_type_method(TextView, set_bottom_margin, !)
        .add_type_method(TextView, get_bottom_margin)
    ;

    add_widget_signals<TextView>(text_view);
    add_signal_text_changed<TextView>(text_view);
    add_signal_undo<TextView>(text_view);
    add_signal_redo<TextView>(text_view);
}

// ### TODO

static void implement_texture(jlcxx::Module& module) {}

// ### TODO

static void implement_toggle_button(jlcxx::Module& module) {}

// ### TODO

static void implement_transition_type(jlcxx::Module& module) {}

// ### TODO

static void implement_viewport(jlcxx::Module& module) {}

// ### TODO

static void implement_widget(jlcxx::Module& module) {}

// ### WINDOW

static void implement_window(jlcxx::Module& module)
{
    define_enum_in(module, WindowCloseRequestResult);
    module.add_enum_value(WindowCloseRequestResult, WINDOW_CLOSE_REQUEST_RESULT, ALLOW_CLOSE);
    module.add_enum_value(WindowCloseRequestResult, WINDOW_CLOSE_REQUEST_RESULT, PREVENT_CLOSE);

    auto window = module.add_type(Window)
        .add_constructor(Application&)
        .add_type_method(Window, set_application, !)
        .add_type_method(Window, set_maximized, !)
        .add_type_method(Window, set_fullscreen, !)
        .add_type_method(Window, present, !)
        .add_type_method(Window, set_hide_on_close, !)
        .add_type_method(Window, close, !)
        .method("set_child!", [](Window& window, void* widget){
            window.set_child(*((Widget*) widget));
        })
        .add_type_method(Window, remove_child, !)
        .add_type_method(Window, set_destroy_with_parent, !)
        .add_type_method(Window, get_destroy_with_parent)
        .add_type_method(Window, set_title, !)
        .add_type_method(Window, get_title)
        .method("set_titlebar_widget!", [](Window& window, void* widget){
            window.set_titlebar_widget(*((Widget*) widget));
        })
        .add_type_method(Window, remove_titlebar_widget, !)
        .add_type_method(Window, set_is_modal, !)
        .add_type_method(Window, get_is_modal)
        .add_type_method(Window, set_transient_for, !)
        .add_type_method(Window, set_is_decorated, !)
        .add_type_method(Window, get_is_decorated)
        .add_type_method(Window, set_has_close_button, !)
        .add_type_method(Window, get_has_close_button)
        .add_type_method(Window, set_startup_notification_identifier, !)
        .add_type_method(Window, set_focus_visible, !)
        .add_type_method(Window, get_focus_visible)
        .method("set_default_widget!", [](Window& window, void* widget) {
            window.set_default_widget(*((Widget*) widget));
        })
    ;

    add_widget_signals<Window>(window);
    add_signal_close_request<Window>(window);
    add_signal_activate_default_widget<Window>(window);
    add_signal_activate_focused_widget<Window>(window);
}

// ### TODO

static void implement_wrap_mode(jlcxx::Module& module) {}

// ### MAIN

JLCXX_MODULE define_julia_module(jlcxx::Module& module)
{
    module.set_const("GTK_MAJOR_VERSION", (int) GTK_MAJOR_VERSION);
    module.set_const("GTK_MINOR_VERSION", (int) GTK_MINOR_VERSION);

    implement_adjustment(module);
    implement_action(module);
    implement_alignment(module);
    implement_application(module);

    // jump

    implement_event_controller(module);
    implement_drag_event_controller(module);
    implement_click_event_controller(module);
    implement_focus_event_controller(module);
    implement_key_event_controller(module);
    implement_long_press_event_controller(module);
    implement_motion_event_controller(module);
    implement_pinch_zoom_event_controller(module);
    implement_rotate_event_controller(module);
    implement_scroll_event_controller(module);
    implement_shortcut_event_controller(module);
    implement_stylus_event_controller(module);
    implement_swipe_event_controller(module);
    implement_pan_event_controller(module);

    implement_window(module);
    implement_aspect_frame(module);
    implement_blend_mode(module);
    implement_orientation(module);
    implement_box(module);
    implement_button(module);
    implement_center_box(module);
    implement_check_button(module);
    implement_clipboard(module);
    implement_color(module);
    implement_icon(module);
    implement_image(module);
    implement_image_display(module);
    implement_column_view(module);
    implement_cursor_type(module);

    implement_drop_down(module);
    implement_entry(module);
    implement_expander(module);
    implement_file_chooser(module);
    implement_file_descriptor(module);
    implement_file_monitor(module);
    implement_file_system(module);
    implement_fixed(module);
    implement_frame(module);
    implement_frame_clock(module);
    implement_geometry(module);
    implement_gl_common(module);
    implement_gl_transform(module);
    implement_grid(module);
    implement_grid_view(module);
    implement_header_bar(module);
    implement_key_file(module);

    implement_justify_mode(module);
    implement_label(module);
    implement_text_view(module);

    implement_level_bar(module);
    implement_list_view(module);
    implement_log(module);

    implement_msaa_render_texture(module);
    implement_music(module);
    implement_notebook(module);
    implement_overlay(module);
    implement_paned(module);


    implement_relative_position(module);
    implement_menu_model(module);
    implement_menu_bar(module);
    implement_popover_menu(module);
    implement_popover(module);
    implement_popover_button(module);
    implement_progress_bar(module);

    implement_render_area(module);
    implement_render_task(module);
    implement_render_texture(module);
    implement_revealer(module);

    implement_scale(module);
    implement_scale_mode(module);

    implement_scrollbar(module);
    implement_selection_model(module);
    implement_separator(module);
    implement_shader(module);
    implement_shape(module);

    implement_sound(module);
    implement_sound_buffer(module);
    implement_spin_button(module);
    implement_spinner(module);
    implement_stack(module);

    implement_switch(module);
    implement_texture(module);
    implement_toggle_button(module);
    implement_transition_type(module);
    implement_viewport(module);
    implement_widget(module);
}

void export_key_codes(jlcxx::Module& module)
{
    module.set_const("KEY_VoidSymbol", (guint) mousetrap::KEY_VoidSymbol);
    module.set_const("KEY_BackSpace", (guint) mousetrap::KEY_BackSpace);
    module.set_const("KEY_Tab", (guint) mousetrap::KEY_Tab);
    module.set_const("KEY_Linefeed", (guint) mousetrap::KEY_Linefeed);
    module.set_const("KEY_Clear", (guint) mousetrap::KEY_Clear);
    module.set_const("KEY_Return", (guint) mousetrap::KEY_Return);
    module.set_const("KEY_Pause", (guint) mousetrap::KEY_Pause);
    module.set_const("KEY_Scroll_Lock", (guint) mousetrap::KEY_Scroll_Lock);
    module.set_const("KEY_Sys_Req", (guint) mousetrap::KEY_Sys_Req);
    module.set_const("KEY_Escape", (guint) mousetrap::KEY_Escape);
    module.set_const("KEY_Delete", (guint) mousetrap::KEY_Delete);
    module.set_const("KEY_Multi_key", (guint) mousetrap::KEY_Multi_key);
    module.set_const("KEY_Codeinput", (guint) mousetrap::KEY_Codeinput);
    module.set_const("KEY_SingleCandidate", (guint) mousetrap::KEY_SingleCandidate);
    module.set_const("KEY_MultipleCandidate", (guint) mousetrap::KEY_MultipleCandidate);
    module.set_const("KEY_PreviousCandidate", (guint) mousetrap::KEY_PreviousCandidate);
    module.set_const("KEY_Kanji", (guint) mousetrap::KEY_Kanji);
    module.set_const("KEY_Muhenkan", (guint) mousetrap::KEY_Muhenkan);
    module.set_const("KEY_Henkan_Mode", (guint) mousetrap::KEY_Henkan_Mode);
    module.set_const("KEY_Henkan", (guint) mousetrap::KEY_Henkan);
    module.set_const("KEY_Romaji", (guint) mousetrap::KEY_Romaji);
    module.set_const("KEY_Hiragana", (guint) mousetrap::KEY_Hiragana);
    module.set_const("KEY_Katakana", (guint) mousetrap::KEY_Katakana);
    module.set_const("KEY_Hiragana_Katakana", (guint) mousetrap::KEY_Hiragana_Katakana);
    module.set_const("KEY_Zenkaku", (guint) mousetrap::KEY_Zenkaku);
    module.set_const("KEY_Hankaku", (guint) mousetrap::KEY_Hankaku);
    module.set_const("KEY_Zenkaku_Hankaku", (guint) mousetrap::KEY_Zenkaku_Hankaku);
    module.set_const("KEY_Touroku", (guint) mousetrap::KEY_Touroku);
    module.set_const("KEY_Massyo", (guint) mousetrap::KEY_Massyo);
    module.set_const("KEY_Kana_Lock", (guint) mousetrap::KEY_Kana_Lock);
    module.set_const("KEY_Kana_Shift", (guint) mousetrap::KEY_Kana_Shift);
    module.set_const("KEY_Eisu_Shift", (guint) mousetrap::KEY_Eisu_Shift);
    module.set_const("KEY_Eisu_toggle", (guint) mousetrap::KEY_Eisu_toggle);
    module.set_const("KEY_Kanji_Bangou", (guint) mousetrap::KEY_Kanji_Bangou);
    module.set_const("KEY_Zen_Koho", (guint) mousetrap::KEY_Zen_Koho);
    module.set_const("KEY_Mae_Koho", (guint) mousetrap::KEY_Mae_Koho);
    module.set_const("KEY_Home", (guint) mousetrap::KEY_Home);
    module.set_const("KEY_Left", (guint) mousetrap::KEY_Left);
    module.set_const("KEY_Up", (guint) mousetrap::KEY_Up);
    module.set_const("KEY_Right", (guint) mousetrap::KEY_Right);
    module.set_const("KEY_Down", (guint) mousetrap::KEY_Down);
    module.set_const("KEY_Prior", (guint) mousetrap::KEY_Prior);
    module.set_const("KEY_Page_Up", (guint) mousetrap::KEY_Page_Up);
    module.set_const("KEY_Next", (guint) mousetrap::KEY_Next);
    module.set_const("KEY_Page_Down", (guint) mousetrap::KEY_Page_Down);
    module.set_const("KEY_End", (guint) mousetrap::KEY_End);
    module.set_const("KEY_Begin", (guint) mousetrap::KEY_Begin);
    module.set_const("KEY_Select", (guint) mousetrap::KEY_Select);
    module.set_const("KEY_Print", (guint) mousetrap::KEY_Print);
    module.set_const("KEY_Execute", (guint) mousetrap::KEY_Execute);
    module.set_const("KEY_Insert", (guint) mousetrap::KEY_Insert);
    module.set_const("KEY_Undo", (guint) mousetrap::KEY_Undo);
    module.set_const("KEY_Redo", (guint) mousetrap::KEY_Redo);
    module.set_const("KEY_Menu", (guint) mousetrap::KEY_Menu);
    module.set_const("KEY_Find", (guint) mousetrap::KEY_Find);
    module.set_const("KEY_Cancel", (guint) mousetrap::KEY_Cancel);
    module.set_const("KEY_Help", (guint) mousetrap::KEY_Help);
    module.set_const("KEY_Break", (guint) mousetrap::KEY_Break);
    module.set_const("KEY_Mode_switch", (guint) mousetrap::KEY_Mode_switch);
    module.set_const("KEY_script_switch", (guint) mousetrap::KEY_script_switch);
    module.set_const("KEY_Num_Lock", (guint) mousetrap::KEY_Num_Lock);
    module.set_const("KEY_KP_Space", (guint) mousetrap::KEY_KP_Space);
    module.set_const("KEY_KP_Tab", (guint) mousetrap::KEY_KP_Tab);
    module.set_const("KEY_KP_Enter", (guint) mousetrap::KEY_KP_Enter);
    module.set_const("KEY_KP_F1", (guint) mousetrap::KEY_KP_F1);
    module.set_const("KEY_KP_F2", (guint) mousetrap::KEY_KP_F2);
    module.set_const("KEY_KP_F3", (guint) mousetrap::KEY_KP_F3);
    module.set_const("KEY_KP_F4", (guint) mousetrap::KEY_KP_F4);
    module.set_const("KEY_KP_Home", (guint) mousetrap::KEY_KP_Home);
    module.set_const("KEY_KP_Left", (guint) mousetrap::KEY_KP_Left);
    module.set_const("KEY_KP_Up", (guint) mousetrap::KEY_KP_Up);
    module.set_const("KEY_KP_Right", (guint) mousetrap::KEY_KP_Right);
    module.set_const("KEY_KP_Down", (guint) mousetrap::KEY_KP_Down);
    module.set_const("KEY_KP_Prior", (guint) mousetrap::KEY_KP_Prior);
    module.set_const("KEY_KP_Page_Up", (guint) mousetrap::KEY_KP_Page_Up);
    module.set_const("KEY_KP_Next", (guint) mousetrap::KEY_KP_Next);
    module.set_const("KEY_KP_Page_Down", (guint) mousetrap::KEY_KP_Page_Down);
    module.set_const("KEY_KP_End", (guint) mousetrap::KEY_KP_End);
    module.set_const("KEY_KP_Begin", (guint) mousetrap::KEY_KP_Begin);
    module.set_const("KEY_KP_Insert", (guint) mousetrap::KEY_KP_Insert);
    module.set_const("KEY_KP_Delete", (guint) mousetrap::KEY_KP_Delete);
    module.set_const("KEY_KP_Equal", (guint) mousetrap::KEY_KP_Equal);
    module.set_const("KEY_KP_Multiply", (guint) mousetrap::KEY_KP_Multiply);
    module.set_const("KEY_KP_Add", (guint) mousetrap::KEY_KP_Add);
    module.set_const("KEY_KP_Separator", (guint) mousetrap::KEY_KP_Separator);
    module.set_const("KEY_KP_Subtract", (guint) mousetrap::KEY_KP_Subtract);
    module.set_const("KEY_KP_Decimal", (guint) mousetrap::KEY_KP_Decimal);
    module.set_const("KEY_KP_Divide", (guint) mousetrap::KEY_KP_Divide);
    module.set_const("KEY_KP_0", (guint) mousetrap::KEY_KP_0);
    module.set_const("KEY_KP_1", (guint) mousetrap::KEY_KP_1);
    module.set_const("KEY_KP_2", (guint) mousetrap::KEY_KP_2);
    module.set_const("KEY_KP_3", (guint) mousetrap::KEY_KP_3);
    module.set_const("KEY_KP_4", (guint) mousetrap::KEY_KP_4);
    module.set_const("KEY_KP_5", (guint) mousetrap::KEY_KP_5);
    module.set_const("KEY_KP_6", (guint) mousetrap::KEY_KP_6);
    module.set_const("KEY_KP_7", (guint) mousetrap::KEY_KP_7);
    module.set_const("KEY_KP_8", (guint) mousetrap::KEY_KP_8);
    module.set_const("KEY_KP_9", (guint) mousetrap::KEY_KP_9);
    module.set_const("KEY_F1", (guint) mousetrap::KEY_F1);
    module.set_const("KEY_F2", (guint) mousetrap::KEY_F2);
    module.set_const("KEY_F3", (guint) mousetrap::KEY_F3);
    module.set_const("KEY_F4", (guint) mousetrap::KEY_F4);
    module.set_const("KEY_F5", (guint) mousetrap::KEY_F5);
    module.set_const("KEY_F6", (guint) mousetrap::KEY_F6);
    module.set_const("KEY_F7", (guint) mousetrap::KEY_F7);
    module.set_const("KEY_F8", (guint) mousetrap::KEY_F8);
    module.set_const("KEY_F9", (guint) mousetrap::KEY_F9);
    module.set_const("KEY_F10", (guint) mousetrap::KEY_F10);
    module.set_const("KEY_F11", (guint) mousetrap::KEY_F11);
    module.set_const("KEY_L1", (guint) mousetrap::KEY_L1);
    module.set_const("KEY_F12", (guint) mousetrap::KEY_F12);
    module.set_const("KEY_L2", (guint) mousetrap::KEY_L2);
    module.set_const("KEY_F13", (guint) mousetrap::KEY_F13);
    module.set_const("KEY_L3", (guint) mousetrap::KEY_L3);
    module.set_const("KEY_F14", (guint) mousetrap::KEY_F14);
    module.set_const("KEY_L4", (guint) mousetrap::KEY_L4);
    module.set_const("KEY_F15", (guint) mousetrap::KEY_F15);
    module.set_const("KEY_L5", (guint) mousetrap::KEY_L5);
    module.set_const("KEY_F16", (guint) mousetrap::KEY_F16);
    module.set_const("KEY_L6", (guint) mousetrap::KEY_L6);
    module.set_const("KEY_F17", (guint) mousetrap::KEY_F17);
    module.set_const("KEY_L7", (guint) mousetrap::KEY_L7);
    module.set_const("KEY_F18", (guint) mousetrap::KEY_F18);
    module.set_const("KEY_L8", (guint) mousetrap::KEY_L8);
    module.set_const("KEY_F19", (guint) mousetrap::KEY_F19);
    module.set_const("KEY_L9", (guint) mousetrap::KEY_L9);
    module.set_const("KEY_F20", (guint) mousetrap::KEY_F20);
    module.set_const("KEY_L10", (guint) mousetrap::KEY_L10);
    module.set_const("KEY_F21", (guint) mousetrap::KEY_F21);
    module.set_const("KEY_R1", (guint) mousetrap::KEY_R1);
    module.set_const("KEY_F22", (guint) mousetrap::KEY_F22);
    module.set_const("KEY_R2", (guint) mousetrap::KEY_R2);
    module.set_const("KEY_F23", (guint) mousetrap::KEY_F23);
    module.set_const("KEY_R3", (guint) mousetrap::KEY_R3);
    module.set_const("KEY_F24", (guint) mousetrap::KEY_F24);
    module.set_const("KEY_R4", (guint) mousetrap::KEY_R4);
    module.set_const("KEY_F25", (guint) mousetrap::KEY_F25);
    module.set_const("KEY_R5", (guint) mousetrap::KEY_R5);
    module.set_const("KEY_F26", (guint) mousetrap::KEY_F26);
    module.set_const("KEY_R6", (guint) mousetrap::KEY_R6);
    module.set_const("KEY_F27", (guint) mousetrap::KEY_F27);
    module.set_const("KEY_R7", (guint) mousetrap::KEY_R7);
    module.set_const("KEY_F28", (guint) mousetrap::KEY_F28);
    module.set_const("KEY_R8", (guint) mousetrap::KEY_R8);
    module.set_const("KEY_F29", (guint) mousetrap::KEY_F29);
    module.set_const("KEY_R9", (guint) mousetrap::KEY_R9);
    module.set_const("KEY_F30", (guint) mousetrap::KEY_F30);
    module.set_const("KEY_R10", (guint) mousetrap::KEY_R10);
    module.set_const("KEY_F31", (guint) mousetrap::KEY_F31);
    module.set_const("KEY_R11", (guint) mousetrap::KEY_R11);
    module.set_const("KEY_F32", (guint) mousetrap::KEY_F32);
    module.set_const("KEY_R12", (guint) mousetrap::KEY_R12);
    module.set_const("KEY_F33", (guint) mousetrap::KEY_F33);
    module.set_const("KEY_R13", (guint) mousetrap::KEY_R13);
    module.set_const("KEY_F34", (guint) mousetrap::KEY_F34);
    module.set_const("KEY_R14", (guint) mousetrap::KEY_R14);
    module.set_const("KEY_F35", (guint) mousetrap::KEY_F35);
    module.set_const("KEY_R15", (guint) mousetrap::KEY_R15);
    module.set_const("KEY_Shift_L", (guint) mousetrap::KEY_Shift_L);
    module.set_const("KEY_Shift_R", (guint) mousetrap::KEY_Shift_R);
    module.set_const("KEY_Control_L", (guint) mousetrap::KEY_Control_L);
    module.set_const("KEY_Control_R", (guint) mousetrap::KEY_Control_R);
    module.set_const("KEY_Caps_Lock", (guint) mousetrap::KEY_Caps_Lock);
    module.set_const("KEY_Shift_Lock", (guint) mousetrap::KEY_Shift_Lock);
    module.set_const("KEY_Meta_L", (guint) mousetrap::KEY_Meta_L);
    module.set_const("KEY_Meta_R", (guint) mousetrap::KEY_Meta_R);
    module.set_const("KEY_Alt_L", (guint) mousetrap::KEY_Alt_L);
    module.set_const("KEY_Alt_R", (guint) mousetrap::KEY_Alt_R);
    module.set_const("KEY_Super_L", (guint) mousetrap::KEY_Super_L);
    module.set_const("KEY_Super_R", (guint) mousetrap::KEY_Super_R);
    module.set_const("KEY_Hyper_L", (guint) mousetrap::KEY_Hyper_L);
    module.set_const("KEY_Hyper_R", (guint) mousetrap::KEY_Hyper_R);
    module.set_const("KEY_ISO_Lock", (guint) mousetrap::KEY_ISO_Lock);
    module.set_const("KEY_ISO_Level2_Latch", (guint) mousetrap::KEY_ISO_Level2_Latch);
    module.set_const("KEY_ISO_Level3_Shift", (guint) mousetrap::KEY_ISO_Level3_Shift);
    module.set_const("KEY_ISO_Level3_Latch", (guint) mousetrap::KEY_ISO_Level3_Latch);
    module.set_const("KEY_ISO_Level3_Lock", (guint) mousetrap::KEY_ISO_Level3_Lock);
    module.set_const("KEY_ISO_Level5_Shift", (guint) mousetrap::KEY_ISO_Level5_Shift);
    module.set_const("KEY_ISO_Level5_Latch", (guint) mousetrap::KEY_ISO_Level5_Latch);
    module.set_const("KEY_ISO_Level5_Lock", (guint) mousetrap::KEY_ISO_Level5_Lock);
    module.set_const("KEY_ISO_Group_Shift", (guint) mousetrap::KEY_ISO_Group_Shift);
    module.set_const("KEY_ISO_Group_Latch", (guint) mousetrap::KEY_ISO_Group_Latch);
    module.set_const("KEY_ISO_Group_Lock", (guint) mousetrap::KEY_ISO_Group_Lock);
    module.set_const("KEY_ISO_Next_Group", (guint) mousetrap::KEY_ISO_Next_Group);
    module.set_const("KEY_ISO_Next_Group_Lock", (guint) mousetrap::KEY_ISO_Next_Group_Lock);
    module.set_const("KEY_ISO_Prev_Group", (guint) mousetrap::KEY_ISO_Prev_Group);
    module.set_const("KEY_ISO_Prev_Group_Lock", (guint) mousetrap::KEY_ISO_Prev_Group_Lock);
    module.set_const("KEY_ISO_First_Group", (guint) mousetrap::KEY_ISO_First_Group);
    module.set_const("KEY_ISO_First_Group_Lock", (guint) mousetrap::KEY_ISO_First_Group_Lock);
    module.set_const("KEY_ISO_Last_Group", (guint) mousetrap::KEY_ISO_Last_Group);
    module.set_const("KEY_ISO_Last_Group_Lock", (guint) mousetrap::KEY_ISO_Last_Group_Lock);
    module.set_const("KEY_ISO_Left_Tab", (guint) mousetrap::KEY_ISO_Left_Tab);
    module.set_const("KEY_ISO_Move_Line_Up", (guint) mousetrap::KEY_ISO_Move_Line_Up);
    module.set_const("KEY_ISO_Move_Line_Down", (guint) mousetrap::KEY_ISO_Move_Line_Down);
    module.set_const("KEY_ISO_Partial_Line_Up", (guint) mousetrap::KEY_ISO_Partial_Line_Up);
    module.set_const("KEY_ISO_Partial_Line_Down", (guint) mousetrap::KEY_ISO_Partial_Line_Down);
    module.set_const("KEY_ISO_Partial_Space_Left", (guint) mousetrap::KEY_ISO_Partial_Space_Left);
    module.set_const("KEY_ISO_Partial_Space_Right", (guint) mousetrap::KEY_ISO_Partial_Space_Right);
    module.set_const("KEY_ISO_Set_Margin_Left", (guint) mousetrap::KEY_ISO_Set_Margin_Left);
    module.set_const("KEY_ISO_Set_Margin_Right", (guint) mousetrap::KEY_ISO_Set_Margin_Right);
    module.set_const("KEY_ISO_Release_Margin_Left", (guint) mousetrap::KEY_ISO_Release_Margin_Left);
    module.set_const("KEY_ISO_Release_Margin_Right", (guint) mousetrap::KEY_ISO_Release_Margin_Right);
    module.set_const("KEY_ISO_Release_Both_Margins", (guint) mousetrap::KEY_ISO_Release_Both_Margins);
    module.set_const("KEY_ISO_Fast_Cursor_Left", (guint) mousetrap::KEY_ISO_Fast_Cursor_Left);
    module.set_const("KEY_ISO_Fast_Cursor_Right", (guint) mousetrap::KEY_ISO_Fast_Cursor_Right);
    module.set_const("KEY_ISO_Fast_Cursor_Up", (guint) mousetrap::KEY_ISO_Fast_Cursor_Up);
    module.set_const("KEY_ISO_Fast_Cursor_Down", (guint) mousetrap::KEY_ISO_Fast_Cursor_Down);
    module.set_const("KEY_ISO_Continuous_Underline", (guint) mousetrap::KEY_ISO_Continuous_Underline);
    module.set_const("KEY_ISO_Discontinuous_Underline", (guint) mousetrap::KEY_ISO_Discontinuous_Underline);
    module.set_const("KEY_ISO_Emphasize", (guint) mousetrap::KEY_ISO_Emphasize);
    module.set_const("KEY_ISO_Center_Object", (guint) mousetrap::KEY_ISO_Center_Object);
    module.set_const("KEY_ISO_Enter", (guint) mousetrap::KEY_ISO_Enter);
    module.set_const("KEY_dead_grave", (guint) mousetrap::KEY_dead_grave);
    module.set_const("KEY_dead_acute", (guint) mousetrap::KEY_dead_acute);
    module.set_const("KEY_dead_circumflex", (guint) mousetrap::KEY_dead_circumflex);
    module.set_const("KEY_dead_tilde", (guint) mousetrap::KEY_dead_tilde);
    module.set_const("KEY_dead_perispomeni", (guint) mousetrap::KEY_dead_perispomeni);
    module.set_const("KEY_dead_macron", (guint) mousetrap::KEY_dead_macron);
    module.set_const("KEY_dead_breve", (guint) mousetrap::KEY_dead_breve);
    module.set_const("KEY_dead_abovedot", (guint) mousetrap::KEY_dead_abovedot);
    module.set_const("KEY_dead_diaeresis", (guint) mousetrap::KEY_dead_diaeresis);
    module.set_const("KEY_dead_abovering", (guint) mousetrap::KEY_dead_abovering);
    module.set_const("KEY_dead_doubleacute", (guint) mousetrap::KEY_dead_doubleacute);
    module.set_const("KEY_dead_caron", (guint) mousetrap::KEY_dead_caron);
    module.set_const("KEY_dead_cedilla", (guint) mousetrap::KEY_dead_cedilla);
    module.set_const("KEY_dead_ogonek", (guint) mousetrap::KEY_dead_ogonek);
    module.set_const("KEY_dead_iota", (guint) mousetrap::KEY_dead_iota);
    module.set_const("KEY_dead_voiced_sound", (guint) mousetrap::KEY_dead_voiced_sound);
    module.set_const("KEY_dead_semivoiced_sound", (guint) mousetrap::KEY_dead_semivoiced_sound);
    module.set_const("KEY_dead_belowdot", (guint) mousetrap::KEY_dead_belowdot);
    module.set_const("KEY_dead_hook", (guint) mousetrap::KEY_dead_hook);
    module.set_const("KEY_dead_horn", (guint) mousetrap::KEY_dead_horn);
    module.set_const("KEY_dead_stroke", (guint) mousetrap::KEY_dead_stroke);
    module.set_const("KEY_dead_abovecomma", (guint) mousetrap::KEY_dead_abovecomma);
    module.set_const("KEY_dead_psili", (guint) mousetrap::KEY_dead_psili);
    module.set_const("KEY_dead_abovereversedcomma", (guint) mousetrap::KEY_dead_abovereversedcomma);
    module.set_const("KEY_dead_dasia", (guint) mousetrap::KEY_dead_dasia);
    module.set_const("KEY_dead_doublegrave", (guint) mousetrap::KEY_dead_doublegrave);
    module.set_const("KEY_dead_belowring", (guint) mousetrap::KEY_dead_belowring);
    module.set_const("KEY_dead_belowmacron", (guint) mousetrap::KEY_dead_belowmacron);
    module.set_const("KEY_dead_belowcircumflex", (guint) mousetrap::KEY_dead_belowcircumflex);
    module.set_const("KEY_dead_belowtilde", (guint) mousetrap::KEY_dead_belowtilde);
    module.set_const("KEY_dead_belowbreve", (guint) mousetrap::KEY_dead_belowbreve);
    module.set_const("KEY_dead_belowdiaeresis", (guint) mousetrap::KEY_dead_belowdiaeresis);
    module.set_const("KEY_dead_invertedbreve", (guint) mousetrap::KEY_dead_invertedbreve);
    module.set_const("KEY_dead_belowcomma", (guint) mousetrap::KEY_dead_belowcomma);
    module.set_const("KEY_dead_currency", (guint) mousetrap::KEY_dead_currency);
    module.set_const("KEY_dead_lowline", (guint) mousetrap::KEY_dead_lowline);
    module.set_const("KEY_dead_aboveverticalline", (guint) mousetrap::KEY_dead_aboveverticalline);
    module.set_const("KEY_dead_belowverticalline", (guint) mousetrap::KEY_dead_belowverticalline);
    module.set_const("KEY_dead_longsolidusoverlay", (guint) mousetrap::KEY_dead_longsolidusoverlay);
    module.set_const("KEY_dead_a", (guint) mousetrap::KEY_dead_a);
    module.set_const("KEY_dead_A", (guint) mousetrap::KEY_dead_A);
    module.set_const("KEY_dead_e", (guint) mousetrap::KEY_dead_e);
    module.set_const("KEY_dead_E", (guint) mousetrap::KEY_dead_E);
    module.set_const("KEY_dead_i", (guint) mousetrap::KEY_dead_i);
    module.set_const("KEY_dead_I", (guint) mousetrap::KEY_dead_I);
    module.set_const("KEY_dead_o", (guint) mousetrap::KEY_dead_o);
    module.set_const("KEY_dead_O", (guint) mousetrap::KEY_dead_O);
    module.set_const("KEY_dead_u", (guint) mousetrap::KEY_dead_u);
    module.set_const("KEY_dead_U", (guint) mousetrap::KEY_dead_U);
    module.set_const("KEY_dead_small_schwa", (guint) mousetrap::KEY_dead_small_schwa);
    module.set_const("KEY_dead_capital_schwa", (guint) mousetrap::KEY_dead_capital_schwa);
    module.set_const("KEY_dead_greek", (guint) mousetrap::KEY_dead_greek);
    module.set_const("KEY_First_Virtual_Screen", (guint) mousetrap::KEY_First_Virtual_Screen);
    module.set_const("KEY_Prev_Virtual_Screen", (guint) mousetrap::KEY_Prev_Virtual_Screen);
    module.set_const("KEY_Next_Virtual_Screen", (guint) mousetrap::KEY_Next_Virtual_Screen);
    module.set_const("KEY_Last_Virtual_Screen", (guint) mousetrap::KEY_Last_Virtual_Screen);
    module.set_const("KEY_Terminate_Server", (guint) mousetrap::KEY_Terminate_Server);
    module.set_const("KEY_AccessX_Enable", (guint) mousetrap::KEY_AccessX_Enable);
    module.set_const("KEY_AccessX_Feedback_Enable", (guint) mousetrap::KEY_AccessX_Feedback_Enable);
    module.set_const("KEY_RepeatKeys_Enable", (guint) mousetrap::KEY_RepeatKeys_Enable);
    module.set_const("KEY_SlowKeys_Enable", (guint) mousetrap::KEY_SlowKeys_Enable);
    module.set_const("KEY_BounceKeys_Enable", (guint) mousetrap::KEY_BounceKeys_Enable);
    module.set_const("KEY_StickyKeys_Enable", (guint) mousetrap::KEY_StickyKeys_Enable);
    module.set_const("KEY_MouseKeys_Enable", (guint) mousetrap::KEY_MouseKeys_Enable);
    module.set_const("KEY_MouseKeys_Accel_Enable", (guint) mousetrap::KEY_MouseKeys_Accel_Enable);
    module.set_const("KEY_Overlay1_Enable", (guint) mousetrap::KEY_Overlay1_Enable);
    module.set_const("KEY_Overlay2_Enable", (guint) mousetrap::KEY_Overlay2_Enable);
    module.set_const("KEY_AudibleBell_Enable", (guint) mousetrap::KEY_AudibleBell_Enable);
    module.set_const("KEY_Pointer_Left", (guint) mousetrap::KEY_Pointer_Left);
    module.set_const("KEY_Pointer_Right", (guint) mousetrap::KEY_Pointer_Right);
    module.set_const("KEY_Pointer_Up", (guint) mousetrap::KEY_Pointer_Up);
    module.set_const("KEY_Pointer_Down", (guint) mousetrap::KEY_Pointer_Down);
    module.set_const("KEY_Pointer_UpLeft", (guint) mousetrap::KEY_Pointer_UpLeft);
    module.set_const("KEY_Pointer_UpRight", (guint) mousetrap::KEY_Pointer_UpRight);
    module.set_const("KEY_Pointer_DownLeft", (guint) mousetrap::KEY_Pointer_DownLeft);
    module.set_const("KEY_Pointer_DownRight", (guint) mousetrap::KEY_Pointer_DownRight);
    module.set_const("KEY_Pointer_Button_Dflt", (guint) mousetrap::KEY_Pointer_Button_Dflt);
    module.set_const("KEY_Pointer_Button1", (guint) mousetrap::KEY_Pointer_Button1);
    module.set_const("KEY_Pointer_Button2", (guint) mousetrap::KEY_Pointer_Button2);
    module.set_const("KEY_Pointer_Button3", (guint) mousetrap::KEY_Pointer_Button3);
    module.set_const("KEY_Pointer_Button4", (guint) mousetrap::KEY_Pointer_Button4);
    module.set_const("KEY_Pointer_Button5", (guint) mousetrap::KEY_Pointer_Button5);
    module.set_const("KEY_Pointer_DblClick_Dflt", (guint) mousetrap::KEY_Pointer_DblClick_Dflt);
    module.set_const("KEY_Pointer_DblClick1", (guint) mousetrap::KEY_Pointer_DblClick1);
    module.set_const("KEY_Pointer_DblClick2", (guint) mousetrap::KEY_Pointer_DblClick2);
    module.set_const("KEY_Pointer_DblClick3", (guint) mousetrap::KEY_Pointer_DblClick3);
    module.set_const("KEY_Pointer_DblClick4", (guint) mousetrap::KEY_Pointer_DblClick4);
    module.set_const("KEY_Pointer_DblClick5", (guint) mousetrap::KEY_Pointer_DblClick5);
    module.set_const("KEY_Pointer_Drag_Dflt", (guint) mousetrap::KEY_Pointer_Drag_Dflt);
    module.set_const("KEY_Pointer_Drag1", (guint) mousetrap::KEY_Pointer_Drag1);
    module.set_const("KEY_Pointer_Drag2", (guint) mousetrap::KEY_Pointer_Drag2);
    module.set_const("KEY_Pointer_Drag3", (guint) mousetrap::KEY_Pointer_Drag3);
    module.set_const("KEY_Pointer_Drag4", (guint) mousetrap::KEY_Pointer_Drag4);
    module.set_const("KEY_Pointer_Drag5", (guint) mousetrap::KEY_Pointer_Drag5);
    module.set_const("KEY_Pointer_EnableKeys", (guint) mousetrap::KEY_Pointer_EnableKeys);
    module.set_const("KEY_Pointer_Accelerate", (guint) mousetrap::KEY_Pointer_Accelerate);
    module.set_const("KEY_Pointer_DfltBtnNext", (guint) mousetrap::KEY_Pointer_DfltBtnNext);
    module.set_const("KEY_Pointer_DfltBtnPrev", (guint) mousetrap::KEY_Pointer_DfltBtnPrev);
    module.set_const("KEY_ch", (guint) mousetrap::KEY_ch);
    module.set_const("KEY_Ch", (guint) mousetrap::KEY_Ch);
    module.set_const("KEY_CH", (guint) mousetrap::KEY_CH);
    module.set_const("KEY_c_h", (guint) mousetrap::KEY_c_h);
    module.set_const("KEY_C_h", (guint) mousetrap::KEY_C_h);
    module.set_const("KEY_C_H", (guint) mousetrap::KEY_C_H);
    module.set_const("KEY_3270_Duplicate", (guint) mousetrap::KEY_3270_Duplicate);
    module.set_const("KEY_3270_FieldMark", (guint) mousetrap::KEY_3270_FieldMark);
    module.set_const("KEY_3270_Right2", (guint) mousetrap::KEY_3270_Right2);
    module.set_const("KEY_3270_Left2", (guint) mousetrap::KEY_3270_Left2);
    module.set_const("KEY_3270_BackTab", (guint) mousetrap::KEY_3270_BackTab);
    module.set_const("KEY_3270_EraseEOF", (guint) mousetrap::KEY_3270_EraseEOF);
    module.set_const("KEY_3270_EraseInput", (guint) mousetrap::KEY_3270_EraseInput);
    module.set_const("KEY_3270_Reset", (guint) mousetrap::KEY_3270_Reset);
    module.set_const("KEY_3270_Quit", (guint) mousetrap::KEY_3270_Quit);
    module.set_const("KEY_3270_PA1", (guint) mousetrap::KEY_3270_PA1);
    module.set_const("KEY_3270_PA2", (guint) mousetrap::KEY_3270_PA2);
    module.set_const("KEY_3270_PA3", (guint) mousetrap::KEY_3270_PA3);
    module.set_const("KEY_3270_Test", (guint) mousetrap::KEY_3270_Test);
    module.set_const("KEY_3270_Attn", (guint) mousetrap::KEY_3270_Attn);
    module.set_const("KEY_3270_CursorBlink", (guint) mousetrap::KEY_3270_CursorBlink);
    module.set_const("KEY_3270_AltCursor", (guint) mousetrap::KEY_3270_AltCursor);
    module.set_const("KEY_3270_KeyClick", (guint) mousetrap::KEY_3270_KeyClick);
    module.set_const("KEY_3270_Jump", (guint) mousetrap::KEY_3270_Jump);
    module.set_const("KEY_3270_Ident", (guint) mousetrap::KEY_3270_Ident);
    module.set_const("KEY_3270_Rule", (guint) mousetrap::KEY_3270_Rule);
    module.set_const("KEY_3270_Copy", (guint) mousetrap::KEY_3270_Copy);
    module.set_const("KEY_3270_Play", (guint) mousetrap::KEY_3270_Play);
    module.set_const("KEY_3270_Setup", (guint) mousetrap::KEY_3270_Setup);
    module.set_const("KEY_3270_Record", (guint) mousetrap::KEY_3270_Record);
    module.set_const("KEY_3270_ChangeScreen", (guint) mousetrap::KEY_3270_ChangeScreen);
    module.set_const("KEY_3270_DeleteWord", (guint) mousetrap::KEY_3270_DeleteWord);
    module.set_const("KEY_3270_ExSelect", (guint) mousetrap::KEY_3270_ExSelect);
    module.set_const("KEY_3270_CursorSelect", (guint) mousetrap::KEY_3270_CursorSelect);
    module.set_const("KEY_3270_PrintScreen", (guint) mousetrap::KEY_3270_PrintScreen);
    module.set_const("KEY_3270_Enter", (guint) mousetrap::KEY_3270_Enter);
    module.set_const("KEY_space", (guint) mousetrap::KEY_space);
    module.set_const("KEY_exclam", (guint) mousetrap::KEY_exclam);
    module.set_const("KEY_quotedbl", (guint) mousetrap::KEY_quotedbl);
    module.set_const("KEY_numbersign", (guint) mousetrap::KEY_numbersign);
    module.set_const("KEY_dollar", (guint) mousetrap::KEY_dollar);
    module.set_const("KEY_percent", (guint) mousetrap::KEY_percent);
    module.set_const("KEY_ampersand", (guint) mousetrap::KEY_ampersand);
    module.set_const("KEY_apostrophe", (guint) mousetrap::KEY_apostrophe);
    module.set_const("KEY_quoteright", (guint) mousetrap::KEY_quoteright);
    module.set_const("KEY_parenleft", (guint) mousetrap::KEY_parenleft);
    module.set_const("KEY_parenright", (guint) mousetrap::KEY_parenright);
    module.set_const("KEY_asterisk", (guint) mousetrap::KEY_asterisk);
    module.set_const("KEY_plus", (guint) mousetrap::KEY_plus);
    module.set_const("KEY_comma", (guint) mousetrap::KEY_comma);
    module.set_const("KEY_minus", (guint) mousetrap::KEY_minus);
    module.set_const("KEY_period", (guint) mousetrap::KEY_period);
    module.set_const("KEY_slash", (guint) mousetrap::KEY_slash);
    module.set_const("KEY_0", (guint) mousetrap::KEY_0);
    module.set_const("KEY_1", (guint) mousetrap::KEY_1);
    module.set_const("KEY_2", (guint) mousetrap::KEY_2);
    module.set_const("KEY_3", (guint) mousetrap::KEY_3);
    module.set_const("KEY_4", (guint) mousetrap::KEY_4);
    module.set_const("KEY_5", (guint) mousetrap::KEY_5);
    module.set_const("KEY_6", (guint) mousetrap::KEY_6);
    module.set_const("KEY_7", (guint) mousetrap::KEY_7);
    module.set_const("KEY_8", (guint) mousetrap::KEY_8);
    module.set_const("KEY_9", (guint) mousetrap::KEY_9);
    module.set_const("KEY_colon", (guint) mousetrap::KEY_colon);
    module.set_const("KEY_semicolon", (guint) mousetrap::KEY_semicolon);
    module.set_const("KEY_less", (guint) mousetrap::KEY_less);
    module.set_const("KEY_equal", (guint) mousetrap::KEY_equal);
    module.set_const("KEY_greater", (guint) mousetrap::KEY_greater);
    module.set_const("KEY_question", (guint) mousetrap::KEY_question);
    module.set_const("KEY_at", (guint) mousetrap::KEY_at);
    module.set_const("KEY_A", (guint) mousetrap::KEY_A);
    module.set_const("KEY_B", (guint) mousetrap::KEY_B);
    module.set_const("KEY_C", (guint) mousetrap::KEY_C);
    module.set_const("KEY_D", (guint) mousetrap::KEY_D);
    module.set_const("KEY_E", (guint) mousetrap::KEY_E);
    module.set_const("KEY_F", (guint) mousetrap::KEY_F);
    module.set_const("KEY_G", (guint) mousetrap::KEY_G);
    module.set_const("KEY_H", (guint) mousetrap::KEY_H);
    module.set_const("KEY_I", (guint) mousetrap::KEY_I);
    module.set_const("KEY_J", (guint) mousetrap::KEY_J);
    module.set_const("KEY_K", (guint) mousetrap::KEY_K);
    module.set_const("KEY_L", (guint) mousetrap::KEY_L);
    module.set_const("KEY_M", (guint) mousetrap::KEY_M);
    module.set_const("KEY_N", (guint) mousetrap::KEY_N);
    module.set_const("KEY_O", (guint) mousetrap::KEY_O);
    module.set_const("KEY_P", (guint) mousetrap::KEY_P);
    module.set_const("KEY_Q", (guint) mousetrap::KEY_Q);
    module.set_const("KEY_R", (guint) mousetrap::KEY_R);
    module.set_const("KEY_S", (guint) mousetrap::KEY_S);
    module.set_const("KEY_T", (guint) mousetrap::KEY_T);
    module.set_const("KEY_U", (guint) mousetrap::KEY_U);
    module.set_const("KEY_V", (guint) mousetrap::KEY_V);
    module.set_const("KEY_W", (guint) mousetrap::KEY_W);
    module.set_const("KEY_X", (guint) mousetrap::KEY_X);
    module.set_const("KEY_Y", (guint) mousetrap::KEY_Y);
    module.set_const("KEY_Z", (guint) mousetrap::KEY_Z);
    module.set_const("KEY_bracketleft", (guint) mousetrap::KEY_bracketleft);
    module.set_const("KEY_backslash", (guint) mousetrap::KEY_backslash);
    module.set_const("KEY_bracketright", (guint) mousetrap::KEY_bracketright);
    module.set_const("KEY_asciicircum", (guint) mousetrap::KEY_asciicircum);
    module.set_const("KEY_underscore", (guint) mousetrap::KEY_underscore);
    module.set_const("KEY_grave", (guint) mousetrap::KEY_grave);
    module.set_const("KEY_quoteleft", (guint) mousetrap::KEY_quoteleft);
    module.set_const("KEY_a", (guint) mousetrap::KEY_a);
    module.set_const("KEY_b", (guint) mousetrap::KEY_b);
    module.set_const("KEY_c", (guint) mousetrap::KEY_c);
    module.set_const("KEY_d", (guint) mousetrap::KEY_d);
    module.set_const("KEY_e", (guint) mousetrap::KEY_e);
    module.set_const("KEY_f", (guint) mousetrap::KEY_f);
    module.set_const("KEY_g", (guint) mousetrap::KEY_g);
    module.set_const("KEY_h", (guint) mousetrap::KEY_h);
    module.set_const("KEY_i", (guint) mousetrap::KEY_i);
    module.set_const("KEY_j", (guint) mousetrap::KEY_j);
    module.set_const("KEY_k", (guint) mousetrap::KEY_k);
    module.set_const("KEY_l", (guint) mousetrap::KEY_l);
    module.set_const("KEY_m", (guint) mousetrap::KEY_m);
    module.set_const("KEY_n", (guint) mousetrap::KEY_n);
    module.set_const("KEY_o", (guint) mousetrap::KEY_o);
    module.set_const("KEY_p", (guint) mousetrap::KEY_p);
    module.set_const("KEY_q", (guint) mousetrap::KEY_q);
    module.set_const("KEY_r", (guint) mousetrap::KEY_r);
    module.set_const("KEY_s", (guint) mousetrap::KEY_s);
    module.set_const("KEY_t", (guint) mousetrap::KEY_t);
    module.set_const("KEY_u", (guint) mousetrap::KEY_u);
    module.set_const("KEY_v", (guint) mousetrap::KEY_v);
    module.set_const("KEY_w", (guint) mousetrap::KEY_w);
    module.set_const("KEY_x", (guint) mousetrap::KEY_x);
    module.set_const("KEY_y", (guint) mousetrap::KEY_y);
    module.set_const("KEY_z", (guint) mousetrap::KEY_z);
    module.set_const("KEY_braceleft", (guint) mousetrap::KEY_braceleft);
    module.set_const("KEY_bar", (guint) mousetrap::KEY_bar);
    module.set_const("KEY_braceright", (guint) mousetrap::KEY_braceright);
    module.set_const("KEY_asciitilde", (guint) mousetrap::KEY_asciitilde);
    module.set_const("KEY_nobreakspace", (guint) mousetrap::KEY_nobreakspace);
    module.set_const("KEY_exclamdown", (guint) mousetrap::KEY_exclamdown);
    module.set_const("KEY_cent", (guint) mousetrap::KEY_cent);
    module.set_const("KEY_sterling", (guint) mousetrap::KEY_sterling);
    module.set_const("KEY_currency", (guint) mousetrap::KEY_currency);
    module.set_const("KEY_yen", (guint) mousetrap::KEY_yen);
    module.set_const("KEY_brokenbar", (guint) mousetrap::KEY_brokenbar);
    module.set_const("KEY_section", (guint) mousetrap::KEY_section);
    module.set_const("KEY_diaeresis", (guint) mousetrap::KEY_diaeresis);
    module.set_const("KEY_copyright", (guint) mousetrap::KEY_copyright);
    module.set_const("KEY_ordfeminine", (guint) mousetrap::KEY_ordfeminine);
    module.set_const("KEY_guillemotleft", (guint) mousetrap::KEY_guillemotleft);
    module.set_const("KEY_notsign", (guint) mousetrap::KEY_notsign);
    module.set_const("KEY_hyphen", (guint) mousetrap::KEY_hyphen);
    module.set_const("KEY_registered", (guint) mousetrap::KEY_registered);
    module.set_const("KEY_macron", (guint) mousetrap::KEY_macron);
    module.set_const("KEY_degree", (guint) mousetrap::KEY_degree);
    module.set_const("KEY_plusminus", (guint) mousetrap::KEY_plusminus);
    module.set_const("KEY_twosuperior", (guint) mousetrap::KEY_twosuperior);
    module.set_const("KEY_threesuperior", (guint) mousetrap::KEY_threesuperior);
    module.set_const("KEY_acute", (guint) mousetrap::KEY_acute);
    module.set_const("KEY_mu", (guint) mousetrap::KEY_mu);
    module.set_const("KEY_paragraph", (guint) mousetrap::KEY_paragraph);
    module.set_const("KEY_periodcentered", (guint) mousetrap::KEY_periodcentered);
    module.set_const("KEY_cedilla", (guint) mousetrap::KEY_cedilla);
    module.set_const("KEY_onesuperior", (guint) mousetrap::KEY_onesuperior);
    module.set_const("KEY_masculine", (guint) mousetrap::KEY_masculine);
    module.set_const("KEY_guillemotright", (guint) mousetrap::KEY_guillemotright);
    module.set_const("KEY_onequarter", (guint) mousetrap::KEY_onequarter);
    module.set_const("KEY_onehalf", (guint) mousetrap::KEY_onehalf);
    module.set_const("KEY_threequarters", (guint) mousetrap::KEY_threequarters);
    module.set_const("KEY_questiondown", (guint) mousetrap::KEY_questiondown);
    module.set_const("KEY_Agrave", (guint) mousetrap::KEY_Agrave);
    module.set_const("KEY_Aacute", (guint) mousetrap::KEY_Aacute);
    module.set_const("KEY_Acircumflex", (guint) mousetrap::KEY_Acircumflex);
    module.set_const("KEY_Atilde", (guint) mousetrap::KEY_Atilde);
    module.set_const("KEY_Adiaeresis", (guint) mousetrap::KEY_Adiaeresis);
    module.set_const("KEY_Aring", (guint) mousetrap::KEY_Aring);
    module.set_const("KEY_AE", (guint) mousetrap::KEY_AE);
    module.set_const("KEY_Ccedilla", (guint) mousetrap::KEY_Ccedilla);
    module.set_const("KEY_Egrave", (guint) mousetrap::KEY_Egrave);
    module.set_const("KEY_Eacute", (guint) mousetrap::KEY_Eacute);
    module.set_const("KEY_Ecircumflex", (guint) mousetrap::KEY_Ecircumflex);
    module.set_const("KEY_Ediaeresis", (guint) mousetrap::KEY_Ediaeresis);
    module.set_const("KEY_Igrave", (guint) mousetrap::KEY_Igrave);
    module.set_const("KEY_Iacute", (guint) mousetrap::KEY_Iacute);
    module.set_const("KEY_Icircumflex", (guint) mousetrap::KEY_Icircumflex);
    module.set_const("KEY_Idiaeresis", (guint) mousetrap::KEY_Idiaeresis);
    module.set_const("KEY_ETH", (guint) mousetrap::KEY_ETH);
    module.set_const("KEY_Eth", (guint) mousetrap::KEY_Eth);
    module.set_const("KEY_Ntilde", (guint) mousetrap::KEY_Ntilde);
    module.set_const("KEY_Ograve", (guint) mousetrap::KEY_Ograve);
    module.set_const("KEY_Oacute", (guint) mousetrap::KEY_Oacute);
    module.set_const("KEY_Ocircumflex", (guint) mousetrap::KEY_Ocircumflex);
    module.set_const("KEY_Otilde", (guint) mousetrap::KEY_Otilde);
    module.set_const("KEY_Odiaeresis", (guint) mousetrap::KEY_Odiaeresis);
    module.set_const("KEY_multiply", (guint) mousetrap::KEY_multiply);
    module.set_const("KEY_Oslash", (guint) mousetrap::KEY_Oslash);
    module.set_const("KEY_Ooblique", (guint) mousetrap::KEY_Ooblique);
    module.set_const("KEY_Ugrave", (guint) mousetrap::KEY_Ugrave);
    module.set_const("KEY_Uacute", (guint) mousetrap::KEY_Uacute);
    module.set_const("KEY_Ucircumflex", (guint) mousetrap::KEY_Ucircumflex);
    module.set_const("KEY_Udiaeresis", (guint) mousetrap::KEY_Udiaeresis);
    module.set_const("KEY_Yacute", (guint) mousetrap::KEY_Yacute);
    module.set_const("KEY_THORN", (guint) mousetrap::KEY_THORN);
    module.set_const("KEY_Thorn", (guint) mousetrap::KEY_Thorn);
    module.set_const("KEY_ssharp", (guint) mousetrap::KEY_ssharp);
    module.set_const("KEY_agrave", (guint) mousetrap::KEY_agrave);
    module.set_const("KEY_aacute", (guint) mousetrap::KEY_aacute);
    module.set_const("KEY_acircumflex", (guint) mousetrap::KEY_acircumflex);
    module.set_const("KEY_atilde", (guint) mousetrap::KEY_atilde);
    module.set_const("KEY_adiaeresis", (guint) mousetrap::KEY_adiaeresis);
    module.set_const("KEY_aring", (guint) mousetrap::KEY_aring);
    module.set_const("KEY_ae", (guint) mousetrap::KEY_ae);
    module.set_const("KEY_ccedilla", (guint) mousetrap::KEY_ccedilla);
    module.set_const("KEY_egrave", (guint) mousetrap::KEY_egrave);
    module.set_const("KEY_eacute", (guint) mousetrap::KEY_eacute);
    module.set_const("KEY_ecircumflex", (guint) mousetrap::KEY_ecircumflex);
    module.set_const("KEY_ediaeresis", (guint) mousetrap::KEY_ediaeresis);
    module.set_const("KEY_igrave", (guint) mousetrap::KEY_igrave);
    module.set_const("KEY_iacute", (guint) mousetrap::KEY_iacute);
    module.set_const("KEY_icircumflex", (guint) mousetrap::KEY_icircumflex);
    module.set_const("KEY_idiaeresis", (guint) mousetrap::KEY_idiaeresis);
    module.set_const("KEY_eth", (guint) mousetrap::KEY_eth);
    module.set_const("KEY_ntilde", (guint) mousetrap::KEY_ntilde);
    module.set_const("KEY_ograve", (guint) mousetrap::KEY_ograve);
    module.set_const("KEY_oacute", (guint) mousetrap::KEY_oacute);
    module.set_const("KEY_ocircumflex", (guint) mousetrap::KEY_ocircumflex);
    module.set_const("KEY_otilde", (guint) mousetrap::KEY_otilde);
    module.set_const("KEY_odiaeresis", (guint) mousetrap::KEY_odiaeresis);
    module.set_const("KEY_division", (guint) mousetrap::KEY_division);
    module.set_const("KEY_oslash", (guint) mousetrap::KEY_oslash);
    module.set_const("KEY_ooblique", (guint) mousetrap::KEY_ooblique);
    module.set_const("KEY_ugrave", (guint) mousetrap::KEY_ugrave);
    module.set_const("KEY_uacute", (guint) mousetrap::KEY_uacute);
    module.set_const("KEY_ucircumflex", (guint) mousetrap::KEY_ucircumflex);
    module.set_const("KEY_udiaeresis", (guint) mousetrap::KEY_udiaeresis);
    module.set_const("KEY_yacute", (guint) mousetrap::KEY_yacute);
    module.set_const("KEY_thorn", (guint) mousetrap::KEY_thorn);
    module.set_const("KEY_ydiaeresis", (guint) mousetrap::KEY_ydiaeresis);
    module.set_const("KEY_Aogonek", (guint) mousetrap::KEY_Aogonek);
    module.set_const("KEY_breve", (guint) mousetrap::KEY_breve);
    module.set_const("KEY_Lstroke", (guint) mousetrap::KEY_Lstroke);
    module.set_const("KEY_Lcaron", (guint) mousetrap::KEY_Lcaron);
    module.set_const("KEY_Sacute", (guint) mousetrap::KEY_Sacute);
    module.set_const("KEY_Scaron", (guint) mousetrap::KEY_Scaron);
    module.set_const("KEY_Scedilla", (guint) mousetrap::KEY_Scedilla);
    module.set_const("KEY_Tcaron", (guint) mousetrap::KEY_Tcaron);
    module.set_const("KEY_Zacute", (guint) mousetrap::KEY_Zacute);
    module.set_const("KEY_Zcaron", (guint) mousetrap::KEY_Zcaron);
    module.set_const("KEY_Zabovedot", (guint) mousetrap::KEY_Zabovedot);
    module.set_const("KEY_aogonek", (guint) mousetrap::KEY_aogonek);
    module.set_const("KEY_ogonek", (guint) mousetrap::KEY_ogonek);
    module.set_const("KEY_lstroke", (guint) mousetrap::KEY_lstroke);
    module.set_const("KEY_lcaron", (guint) mousetrap::KEY_lcaron);
    module.set_const("KEY_sacute", (guint) mousetrap::KEY_sacute);
    module.set_const("KEY_caron", (guint) mousetrap::KEY_caron);
    module.set_const("KEY_scaron", (guint) mousetrap::KEY_scaron);
    module.set_const("KEY_scedilla", (guint) mousetrap::KEY_scedilla);
    module.set_const("KEY_tcaron", (guint) mousetrap::KEY_tcaron);
    module.set_const("KEY_zacute", (guint) mousetrap::KEY_zacute);
    module.set_const("KEY_doubleacute", (guint) mousetrap::KEY_doubleacute);
    module.set_const("KEY_zcaron", (guint) mousetrap::KEY_zcaron);
    module.set_const("KEY_zabovedot", (guint) mousetrap::KEY_zabovedot);
    module.set_const("KEY_Racute", (guint) mousetrap::KEY_Racute);
    module.set_const("KEY_Abreve", (guint) mousetrap::KEY_Abreve);
    module.set_const("KEY_Lacute", (guint) mousetrap::KEY_Lacute);
    module.set_const("KEY_Cacute", (guint) mousetrap::KEY_Cacute);
    module.set_const("KEY_Ccaron", (guint) mousetrap::KEY_Ccaron);
    module.set_const("KEY_Eogonek", (guint) mousetrap::KEY_Eogonek);
    module.set_const("KEY_Ecaron", (guint) mousetrap::KEY_Ecaron);
    module.set_const("KEY_Dcaron", (guint) mousetrap::KEY_Dcaron);
    module.set_const("KEY_Dstroke", (guint) mousetrap::KEY_Dstroke);
    module.set_const("KEY_Nacute", (guint) mousetrap::KEY_Nacute);
    module.set_const("KEY_Ncaron", (guint) mousetrap::KEY_Ncaron);
    module.set_const("KEY_Odoubleacute", (guint) mousetrap::KEY_Odoubleacute);
    module.set_const("KEY_Rcaron", (guint) mousetrap::KEY_Rcaron);
    module.set_const("KEY_Uring", (guint) mousetrap::KEY_Uring);
    module.set_const("KEY_Udoubleacute", (guint) mousetrap::KEY_Udoubleacute);
    module.set_const("KEY_Tcedilla", (guint) mousetrap::KEY_Tcedilla);
    module.set_const("KEY_racute", (guint) mousetrap::KEY_racute);
    module.set_const("KEY_abreve", (guint) mousetrap::KEY_abreve);
    module.set_const("KEY_lacute", (guint) mousetrap::KEY_lacute);
    module.set_const("KEY_cacute", (guint) mousetrap::KEY_cacute);
    module.set_const("KEY_ccaron", (guint) mousetrap::KEY_ccaron);
    module.set_const("KEY_eogonek", (guint) mousetrap::KEY_eogonek);
    module.set_const("KEY_ecaron", (guint) mousetrap::KEY_ecaron);
    module.set_const("KEY_dcaron", (guint) mousetrap::KEY_dcaron);
    module.set_const("KEY_dstroke", (guint) mousetrap::KEY_dstroke);
    module.set_const("KEY_nacute", (guint) mousetrap::KEY_nacute);
    module.set_const("KEY_ncaron", (guint) mousetrap::KEY_ncaron);
    module.set_const("KEY_odoubleacute", (guint) mousetrap::KEY_odoubleacute);
    module.set_const("KEY_rcaron", (guint) mousetrap::KEY_rcaron);
    module.set_const("KEY_uring", (guint) mousetrap::KEY_uring);
    module.set_const("KEY_udoubleacute", (guint) mousetrap::KEY_udoubleacute);
    module.set_const("KEY_tcedilla", (guint) mousetrap::KEY_tcedilla);
    module.set_const("KEY_abovedot", (guint) mousetrap::KEY_abovedot);
    module.set_const("KEY_Hstroke", (guint) mousetrap::KEY_Hstroke);
    module.set_const("KEY_Hcircumflex", (guint) mousetrap::KEY_Hcircumflex);
    module.set_const("KEY_Iabovedot", (guint) mousetrap::KEY_Iabovedot);
    module.set_const("KEY_Gbreve", (guint) mousetrap::KEY_Gbreve);
    module.set_const("KEY_Jcircumflex", (guint) mousetrap::KEY_Jcircumflex);
    module.set_const("KEY_hstroke", (guint) mousetrap::KEY_hstroke);
    module.set_const("KEY_hcircumflex", (guint) mousetrap::KEY_hcircumflex);
    module.set_const("KEY_idotless", (guint) mousetrap::KEY_idotless);
    module.set_const("KEY_gbreve", (guint) mousetrap::KEY_gbreve);
    module.set_const("KEY_jcircumflex", (guint) mousetrap::KEY_jcircumflex);
    module.set_const("KEY_Cabovedot", (guint) mousetrap::KEY_Cabovedot);
    module.set_const("KEY_Ccircumflex", (guint) mousetrap::KEY_Ccircumflex);
    module.set_const("KEY_Gabovedot", (guint) mousetrap::KEY_Gabovedot);
    module.set_const("KEY_Gcircumflex", (guint) mousetrap::KEY_Gcircumflex);
    module.set_const("KEY_Ubreve", (guint) mousetrap::KEY_Ubreve);
    module.set_const("KEY_Scircumflex", (guint) mousetrap::KEY_Scircumflex);
    module.set_const("KEY_cabovedot", (guint) mousetrap::KEY_cabovedot);
    module.set_const("KEY_ccircumflex", (guint) mousetrap::KEY_ccircumflex);
    module.set_const("KEY_gabovedot", (guint) mousetrap::KEY_gabovedot);
    module.set_const("KEY_gcircumflex", (guint) mousetrap::KEY_gcircumflex);
    module.set_const("KEY_ubreve", (guint) mousetrap::KEY_ubreve);
    module.set_const("KEY_scircumflex", (guint) mousetrap::KEY_scircumflex);
    module.set_const("KEY_kra", (guint) mousetrap::KEY_kra);
    module.set_const("KEY_kappa", (guint) mousetrap::KEY_kappa);
    module.set_const("KEY_Rcedilla", (guint) mousetrap::KEY_Rcedilla);
    module.set_const("KEY_Itilde", (guint) mousetrap::KEY_Itilde);
    module.set_const("KEY_Lcedilla", (guint) mousetrap::KEY_Lcedilla);
    module.set_const("KEY_Emacron", (guint) mousetrap::KEY_Emacron);
    module.set_const("KEY_Gcedilla", (guint) mousetrap::KEY_Gcedilla);
    module.set_const("KEY_Tslash", (guint) mousetrap::KEY_Tslash);
    module.set_const("KEY_rcedilla", (guint) mousetrap::KEY_rcedilla);
    module.set_const("KEY_itilde", (guint) mousetrap::KEY_itilde);
    module.set_const("KEY_lcedilla", (guint) mousetrap::KEY_lcedilla);
    module.set_const("KEY_emacron", (guint) mousetrap::KEY_emacron);
    module.set_const("KEY_gcedilla", (guint) mousetrap::KEY_gcedilla);
    module.set_const("KEY_tslash", (guint) mousetrap::KEY_tslash);
    module.set_const("KEY_ENG", (guint) mousetrap::KEY_ENG);
    module.set_const("KEY_eng", (guint) mousetrap::KEY_eng);
    module.set_const("KEY_Amacron", (guint) mousetrap::KEY_Amacron);
    module.set_const("KEY_Iogonek", (guint) mousetrap::KEY_Iogonek);
    module.set_const("KEY_Eabovedot", (guint) mousetrap::KEY_Eabovedot);
    module.set_const("KEY_Imacron", (guint) mousetrap::KEY_Imacron);
    module.set_const("KEY_Ncedilla", (guint) mousetrap::KEY_Ncedilla);
    module.set_const("KEY_Omacron", (guint) mousetrap::KEY_Omacron);
    module.set_const("KEY_Kcedilla", (guint) mousetrap::KEY_Kcedilla);
    module.set_const("KEY_Uogonek", (guint) mousetrap::KEY_Uogonek);
    module.set_const("KEY_Utilde", (guint) mousetrap::KEY_Utilde);
    module.set_const("KEY_Umacron", (guint) mousetrap::KEY_Umacron);
    module.set_const("KEY_amacron", (guint) mousetrap::KEY_amacron);
    module.set_const("KEY_iogonek", (guint) mousetrap::KEY_iogonek);
    module.set_const("KEY_eabovedot", (guint) mousetrap::KEY_eabovedot);
    module.set_const("KEY_imacron", (guint) mousetrap::KEY_imacron);
    module.set_const("KEY_ncedilla", (guint) mousetrap::KEY_ncedilla);
    module.set_const("KEY_omacron", (guint) mousetrap::KEY_omacron);
    module.set_const("KEY_kcedilla", (guint) mousetrap::KEY_kcedilla);
    module.set_const("KEY_uogonek", (guint) mousetrap::KEY_uogonek);
    module.set_const("KEY_utilde", (guint) mousetrap::KEY_utilde);
    module.set_const("KEY_umacron", (guint) mousetrap::KEY_umacron);
    module.set_const("KEY_Wcircumflex", (guint) mousetrap::KEY_Wcircumflex);
    module.set_const("KEY_wcircumflex", (guint) mousetrap::KEY_wcircumflex);
    module.set_const("KEY_Ycircumflex", (guint) mousetrap::KEY_Ycircumflex);
    module.set_const("KEY_ycircumflex", (guint) mousetrap::KEY_ycircumflex);
    module.set_const("KEY_Babovedot", (guint) mousetrap::KEY_Babovedot);
    module.set_const("KEY_babovedot", (guint) mousetrap::KEY_babovedot);
    module.set_const("KEY_Dabovedot", (guint) mousetrap::KEY_Dabovedot);
    module.set_const("KEY_dabovedot", (guint) mousetrap::KEY_dabovedot);
    module.set_const("KEY_Fabovedot", (guint) mousetrap::KEY_Fabovedot);
    module.set_const("KEY_fabovedot", (guint) mousetrap::KEY_fabovedot);
    module.set_const("KEY_Mabovedot", (guint) mousetrap::KEY_Mabovedot);
    module.set_const("KEY_mabovedot", (guint) mousetrap::KEY_mabovedot);
    module.set_const("KEY_Pabovedot", (guint) mousetrap::KEY_Pabovedot);
    module.set_const("KEY_pabovedot", (guint) mousetrap::KEY_pabovedot);
    module.set_const("KEY_Sabovedot", (guint) mousetrap::KEY_Sabovedot);
    module.set_const("KEY_sabovedot", (guint) mousetrap::KEY_sabovedot);
    module.set_const("KEY_Tabovedot", (guint) mousetrap::KEY_Tabovedot);
    module.set_const("KEY_tabovedot", (guint) mousetrap::KEY_tabovedot);
    module.set_const("KEY_Wgrave", (guint) mousetrap::KEY_Wgrave);
    module.set_const("KEY_wgrave", (guint) mousetrap::KEY_wgrave);
    module.set_const("KEY_Wacute", (guint) mousetrap::KEY_Wacute);
    module.set_const("KEY_wacute", (guint) mousetrap::KEY_wacute);
    module.set_const("KEY_Wdiaeresis", (guint) mousetrap::KEY_Wdiaeresis);
    module.set_const("KEY_wdiaeresis", (guint) mousetrap::KEY_wdiaeresis);
    module.set_const("KEY_Ygrave", (guint) mousetrap::KEY_Ygrave);
    module.set_const("KEY_ygrave", (guint) mousetrap::KEY_ygrave);
    module.set_const("KEY_OE", (guint) mousetrap::KEY_OE);
    module.set_const("KEY_oe", (guint) mousetrap::KEY_oe);
    module.set_const("KEY_Ydiaeresis", (guint) mousetrap::KEY_Ydiaeresis);
    module.set_const("KEY_overline", (guint) mousetrap::KEY_overline);
    module.set_const("KEY_kana_fullstop", (guint) mousetrap::KEY_kana_fullstop);
    module.set_const("KEY_kana_openingbracket", (guint) mousetrap::KEY_kana_openingbracket);
    module.set_const("KEY_kana_closingbracket", (guint) mousetrap::KEY_kana_closingbracket);
    module.set_const("KEY_kana_comma", (guint) mousetrap::KEY_kana_comma);
    module.set_const("KEY_kana_conjunctive", (guint) mousetrap::KEY_kana_conjunctive);
    module.set_const("KEY_kana_middledot", (guint) mousetrap::KEY_kana_middledot);
    module.set_const("KEY_kana_WO", (guint) mousetrap::KEY_kana_WO);
    module.set_const("KEY_kana_a", (guint) mousetrap::KEY_kana_a);
    module.set_const("KEY_kana_i", (guint) mousetrap::KEY_kana_i);
    module.set_const("KEY_kana_u", (guint) mousetrap::KEY_kana_u);
    module.set_const("KEY_kana_e", (guint) mousetrap::KEY_kana_e);
    module.set_const("KEY_kana_o", (guint) mousetrap::KEY_kana_o);
    module.set_const("KEY_kana_ya", (guint) mousetrap::KEY_kana_ya);
    module.set_const("KEY_kana_yu", (guint) mousetrap::KEY_kana_yu);
    module.set_const("KEY_kana_yo", (guint) mousetrap::KEY_kana_yo);
    module.set_const("KEY_kana_tsu", (guint) mousetrap::KEY_kana_tsu);
    module.set_const("KEY_kana_tu", (guint) mousetrap::KEY_kana_tu);
    module.set_const("KEY_prolongedsound", (guint) mousetrap::KEY_prolongedsound);
    module.set_const("KEY_kana_A", (guint) mousetrap::KEY_kana_A);
    module.set_const("KEY_kana_I", (guint) mousetrap::KEY_kana_I);
    module.set_const("KEY_kana_U", (guint) mousetrap::KEY_kana_U);
    module.set_const("KEY_kana_E", (guint) mousetrap::KEY_kana_E);
    module.set_const("KEY_kana_O", (guint) mousetrap::KEY_kana_O);
    module.set_const("KEY_kana_KA", (guint) mousetrap::KEY_kana_KA);
    module.set_const("KEY_kana_KI", (guint) mousetrap::KEY_kana_KI);
    module.set_const("KEY_kana_KU", (guint) mousetrap::KEY_kana_KU);
    module.set_const("KEY_kana_KE", (guint) mousetrap::KEY_kana_KE);
    module.set_const("KEY_kana_KO", (guint) mousetrap::KEY_kana_KO);
    module.set_const("KEY_kana_SA", (guint) mousetrap::KEY_kana_SA);
    module.set_const("KEY_kana_SHI", (guint) mousetrap::KEY_kana_SHI);
    module.set_const("KEY_kana_SU", (guint) mousetrap::KEY_kana_SU);
    module.set_const("KEY_kana_SE", (guint) mousetrap::KEY_kana_SE);
    module.set_const("KEY_kana_SO", (guint) mousetrap::KEY_kana_SO);
    module.set_const("KEY_kana_TA", (guint) mousetrap::KEY_kana_TA);
    module.set_const("KEY_kana_CHI", (guint) mousetrap::KEY_kana_CHI);
    module.set_const("KEY_kana_TI", (guint) mousetrap::KEY_kana_TI);
    module.set_const("KEY_kana_TSU", (guint) mousetrap::KEY_kana_TSU);
    module.set_const("KEY_kana_TU", (guint) mousetrap::KEY_kana_TU);
    module.set_const("KEY_kana_TE", (guint) mousetrap::KEY_kana_TE);
    module.set_const("KEY_kana_TO", (guint) mousetrap::KEY_kana_TO);
    module.set_const("KEY_kana_NA", (guint) mousetrap::KEY_kana_NA);
    module.set_const("KEY_kana_NI", (guint) mousetrap::KEY_kana_NI);
    module.set_const("KEY_kana_NU", (guint) mousetrap::KEY_kana_NU);
    module.set_const("KEY_kana_NE", (guint) mousetrap::KEY_kana_NE);
    module.set_const("KEY_kana_NO", (guint) mousetrap::KEY_kana_NO);
    module.set_const("KEY_kana_HA", (guint) mousetrap::KEY_kana_HA);
    module.set_const("KEY_kana_HI", (guint) mousetrap::KEY_kana_HI);
    module.set_const("KEY_kana_FU", (guint) mousetrap::KEY_kana_FU);
    module.set_const("KEY_kana_HU", (guint) mousetrap::KEY_kana_HU);
    module.set_const("KEY_kana_HE", (guint) mousetrap::KEY_kana_HE);
    module.set_const("KEY_kana_HO", (guint) mousetrap::KEY_kana_HO);
    module.set_const("KEY_kana_MA", (guint) mousetrap::KEY_kana_MA);
    module.set_const("KEY_kana_MI", (guint) mousetrap::KEY_kana_MI);
    module.set_const("KEY_kana_MU", (guint) mousetrap::KEY_kana_MU);
    module.set_const("KEY_kana_ME", (guint) mousetrap::KEY_kana_ME);
    module.set_const("KEY_kana_MO", (guint) mousetrap::KEY_kana_MO);
    module.set_const("KEY_kana_YA", (guint) mousetrap::KEY_kana_YA);
    module.set_const("KEY_kana_YU", (guint) mousetrap::KEY_kana_YU);
    module.set_const("KEY_kana_YO", (guint) mousetrap::KEY_kana_YO);
    module.set_const("KEY_kana_RA", (guint) mousetrap::KEY_kana_RA);
    module.set_const("KEY_kana_RI", (guint) mousetrap::KEY_kana_RI);
    module.set_const("KEY_kana_RU", (guint) mousetrap::KEY_kana_RU);
    module.set_const("KEY_kana_RE", (guint) mousetrap::KEY_kana_RE);
    module.set_const("KEY_kana_RO", (guint) mousetrap::KEY_kana_RO);
    module.set_const("KEY_kana_WA", (guint) mousetrap::KEY_kana_WA);
    module.set_const("KEY_kana_N", (guint) mousetrap::KEY_kana_N);
    module.set_const("KEY_voicedsound", (guint) mousetrap::KEY_voicedsound);
    module.set_const("KEY_semivoicedsound", (guint) mousetrap::KEY_semivoicedsound);
    module.set_const("KEY_kana_switch", (guint) mousetrap::KEY_kana_switch);
    module.set_const("KEY_Farsi_0", (guint) mousetrap::KEY_Farsi_0);
    module.set_const("KEY_Farsi_1", (guint) mousetrap::KEY_Farsi_1);
    module.set_const("KEY_Farsi_2", (guint) mousetrap::KEY_Farsi_2);
    module.set_const("KEY_Farsi_3", (guint) mousetrap::KEY_Farsi_3);
    module.set_const("KEY_Farsi_4", (guint) mousetrap::KEY_Farsi_4);
    module.set_const("KEY_Farsi_5", (guint) mousetrap::KEY_Farsi_5);
    module.set_const("KEY_Farsi_6", (guint) mousetrap::KEY_Farsi_6);
    module.set_const("KEY_Farsi_7", (guint) mousetrap::KEY_Farsi_7);
    module.set_const("KEY_Farsi_8", (guint) mousetrap::KEY_Farsi_8);
    module.set_const("KEY_Farsi_9", (guint) mousetrap::KEY_Farsi_9);
    module.set_const("KEY_Arabic_percent", (guint) mousetrap::KEY_Arabic_percent);
    module.set_const("KEY_Arabic_superscript_alef", (guint) mousetrap::KEY_Arabic_superscript_alef);
    module.set_const("KEY_Arabic_tteh", (guint) mousetrap::KEY_Arabic_tteh);
    module.set_const("KEY_Arabic_peh", (guint) mousetrap::KEY_Arabic_peh);
    module.set_const("KEY_Arabic_tcheh", (guint) mousetrap::KEY_Arabic_tcheh);
    module.set_const("KEY_Arabic_ddal", (guint) mousetrap::KEY_Arabic_ddal);
    module.set_const("KEY_Arabic_rreh", (guint) mousetrap::KEY_Arabic_rreh);
    module.set_const("KEY_Arabic_comma", (guint) mousetrap::KEY_Arabic_comma);
    module.set_const("KEY_Arabic_fullstop", (guint) mousetrap::KEY_Arabic_fullstop);
    module.set_const("KEY_Arabic_0", (guint) mousetrap::KEY_Arabic_0);
    module.set_const("KEY_Arabic_1", (guint) mousetrap::KEY_Arabic_1);
    module.set_const("KEY_Arabic_2", (guint) mousetrap::KEY_Arabic_2);
    module.set_const("KEY_Arabic_3", (guint) mousetrap::KEY_Arabic_3);
    module.set_const("KEY_Arabic_4", (guint) mousetrap::KEY_Arabic_4);
    module.set_const("KEY_Arabic_5", (guint) mousetrap::KEY_Arabic_5);
    module.set_const("KEY_Arabic_6", (guint) mousetrap::KEY_Arabic_6);
    module.set_const("KEY_Arabic_7", (guint) mousetrap::KEY_Arabic_7);
    module.set_const("KEY_Arabic_8", (guint) mousetrap::KEY_Arabic_8);
    module.set_const("KEY_Arabic_9", (guint) mousetrap::KEY_Arabic_9);
    module.set_const("KEY_Arabic_semicolon", (guint) mousetrap::KEY_Arabic_semicolon);
    module.set_const("KEY_Arabic_question_mark", (guint) mousetrap::KEY_Arabic_question_mark);
    module.set_const("KEY_Arabic_hamza", (guint) mousetrap::KEY_Arabic_hamza);
    module.set_const("KEY_Arabic_maddaonalef", (guint) mousetrap::KEY_Arabic_maddaonalef);
    module.set_const("KEY_Arabic_hamzaonalef", (guint) mousetrap::KEY_Arabic_hamzaonalef);
    module.set_const("KEY_Arabic_hamzaonwaw", (guint) mousetrap::KEY_Arabic_hamzaonwaw);
    module.set_const("KEY_Arabic_hamzaunderalef", (guint) mousetrap::KEY_Arabic_hamzaunderalef);
    module.set_const("KEY_Arabic_hamzaonyeh", (guint) mousetrap::KEY_Arabic_hamzaonyeh);
    module.set_const("KEY_Arabic_alef", (guint) mousetrap::KEY_Arabic_alef);
    module.set_const("KEY_Arabic_beh", (guint) mousetrap::KEY_Arabic_beh);
    module.set_const("KEY_Arabic_tehmarbuta", (guint) mousetrap::KEY_Arabic_tehmarbuta);
    module.set_const("KEY_Arabic_teh", (guint) mousetrap::KEY_Arabic_teh);
    module.set_const("KEY_Arabic_theh", (guint) mousetrap::KEY_Arabic_theh);
    module.set_const("KEY_Arabic_jeem", (guint) mousetrap::KEY_Arabic_jeem);
    module.set_const("KEY_Arabic_hah", (guint) mousetrap::KEY_Arabic_hah);
    module.set_const("KEY_Arabic_khah", (guint) mousetrap::KEY_Arabic_khah);
    module.set_const("KEY_Arabic_dal", (guint) mousetrap::KEY_Arabic_dal);
    module.set_const("KEY_Arabic_thal", (guint) mousetrap::KEY_Arabic_thal);
    module.set_const("KEY_Arabic_ra", (guint) mousetrap::KEY_Arabic_ra);
    module.set_const("KEY_Arabic_zain", (guint) mousetrap::KEY_Arabic_zain);
    module.set_const("KEY_Arabic_seen", (guint) mousetrap::KEY_Arabic_seen);
    module.set_const("KEY_Arabic_sheen", (guint) mousetrap::KEY_Arabic_sheen);
    module.set_const("KEY_Arabic_sad", (guint) mousetrap::KEY_Arabic_sad);
    module.set_const("KEY_Arabic_dad", (guint) mousetrap::KEY_Arabic_dad);
    module.set_const("KEY_Arabic_tah", (guint) mousetrap::KEY_Arabic_tah);
    module.set_const("KEY_Arabic_zah", (guint) mousetrap::KEY_Arabic_zah);
    module.set_const("KEY_Arabic_ain", (guint) mousetrap::KEY_Arabic_ain);
    module.set_const("KEY_Arabic_ghain", (guint) mousetrap::KEY_Arabic_ghain);
    module.set_const("KEY_Arabic_tatweel", (guint) mousetrap::KEY_Arabic_tatweel);
    module.set_const("KEY_Arabic_feh", (guint) mousetrap::KEY_Arabic_feh);
    module.set_const("KEY_Arabic_qaf", (guint) mousetrap::KEY_Arabic_qaf);
    module.set_const("KEY_Arabic_kaf", (guint) mousetrap::KEY_Arabic_kaf);
    module.set_const("KEY_Arabic_lam", (guint) mousetrap::KEY_Arabic_lam);
    module.set_const("KEY_Arabic_meem", (guint) mousetrap::KEY_Arabic_meem);
    module.set_const("KEY_Arabic_noon", (guint) mousetrap::KEY_Arabic_noon);
    module.set_const("KEY_Arabic_ha", (guint) mousetrap::KEY_Arabic_ha);
    module.set_const("KEY_Arabic_heh", (guint) mousetrap::KEY_Arabic_heh);
    module.set_const("KEY_Arabic_waw", (guint) mousetrap::KEY_Arabic_waw);
    module.set_const("KEY_Arabic_alefmaksura", (guint) mousetrap::KEY_Arabic_alefmaksura);
    module.set_const("KEY_Arabic_yeh", (guint) mousetrap::KEY_Arabic_yeh);
    module.set_const("KEY_Arabic_fathatan", (guint) mousetrap::KEY_Arabic_fathatan);
    module.set_const("KEY_Arabic_dammatan", (guint) mousetrap::KEY_Arabic_dammatan);
    module.set_const("KEY_Arabic_kasratan", (guint) mousetrap::KEY_Arabic_kasratan);
    module.set_const("KEY_Arabic_fatha", (guint) mousetrap::KEY_Arabic_fatha);
    module.set_const("KEY_Arabic_damma", (guint) mousetrap::KEY_Arabic_damma);
    module.set_const("KEY_Arabic_kasra", (guint) mousetrap::KEY_Arabic_kasra);
    module.set_const("KEY_Arabic_shadda", (guint) mousetrap::KEY_Arabic_shadda);
    module.set_const("KEY_Arabic_sukun", (guint) mousetrap::KEY_Arabic_sukun);
    module.set_const("KEY_Arabic_madda_above", (guint) mousetrap::KEY_Arabic_madda_above);
    module.set_const("KEY_Arabic_hamza_above", (guint) mousetrap::KEY_Arabic_hamza_above);
    module.set_const("KEY_Arabic_hamza_below", (guint) mousetrap::KEY_Arabic_hamza_below);
    module.set_const("KEY_Arabic_jeh", (guint) mousetrap::KEY_Arabic_jeh);
    module.set_const("KEY_Arabic_veh", (guint) mousetrap::KEY_Arabic_veh);
    module.set_const("KEY_Arabic_keheh", (guint) mousetrap::KEY_Arabic_keheh);
    module.set_const("KEY_Arabic_gaf", (guint) mousetrap::KEY_Arabic_gaf);
    module.set_const("KEY_Arabic_noon_ghunna", (guint) mousetrap::KEY_Arabic_noon_ghunna);
    module.set_const("KEY_Arabic_heh_doachashmee", (guint) mousetrap::KEY_Arabic_heh_doachashmee);
    module.set_const("KEY_Farsi_yeh", (guint) mousetrap::KEY_Farsi_yeh);
    module.set_const("KEY_Arabic_farsi_yeh", (guint) mousetrap::KEY_Arabic_farsi_yeh);
    module.set_const("KEY_Arabic_yeh_baree", (guint) mousetrap::KEY_Arabic_yeh_baree);
    module.set_const("KEY_Arabic_heh_goal", (guint) mousetrap::KEY_Arabic_heh_goal);
    module.set_const("KEY_Arabic_switch", (guint) mousetrap::KEY_Arabic_switch);
    module.set_const("KEY_Cyrillic_GHE_bar", (guint) mousetrap::KEY_Cyrillic_GHE_bar);
    module.set_const("KEY_Cyrillic_ghe_bar", (guint) mousetrap::KEY_Cyrillic_ghe_bar);
    module.set_const("KEY_Cyrillic_ZHE_descender", (guint) mousetrap::KEY_Cyrillic_ZHE_descender);
    module.set_const("KEY_Cyrillic_zhe_descender", (guint) mousetrap::KEY_Cyrillic_zhe_descender);
    module.set_const("KEY_Cyrillic_KA_descender", (guint) mousetrap::KEY_Cyrillic_KA_descender);
    module.set_const("KEY_Cyrillic_ka_descender", (guint) mousetrap::KEY_Cyrillic_ka_descender);
    module.set_const("KEY_Cyrillic_KA_vertstroke", (guint) mousetrap::KEY_Cyrillic_KA_vertstroke);
    module.set_const("KEY_Cyrillic_ka_vertstroke", (guint) mousetrap::KEY_Cyrillic_ka_vertstroke);
    module.set_const("KEY_Cyrillic_EN_descender", (guint) mousetrap::KEY_Cyrillic_EN_descender);
    module.set_const("KEY_Cyrillic_en_descender", (guint) mousetrap::KEY_Cyrillic_en_descender);
    module.set_const("KEY_Cyrillic_U_straight", (guint) mousetrap::KEY_Cyrillic_U_straight);
    module.set_const("KEY_Cyrillic_u_straight", (guint) mousetrap::KEY_Cyrillic_u_straight);
    module.set_const("KEY_Cyrillic_U_straight_bar", (guint) mousetrap::KEY_Cyrillic_U_straight_bar);
    module.set_const("KEY_Cyrillic_u_straight_bar", (guint) mousetrap::KEY_Cyrillic_u_straight_bar);
    module.set_const("KEY_Cyrillic_HA_descender", (guint) mousetrap::KEY_Cyrillic_HA_descender);
    module.set_const("KEY_Cyrillic_ha_descender", (guint) mousetrap::KEY_Cyrillic_ha_descender);
    module.set_const("KEY_Cyrillic_CHE_descender", (guint) mousetrap::KEY_Cyrillic_CHE_descender);
    module.set_const("KEY_Cyrillic_che_descender", (guint) mousetrap::KEY_Cyrillic_che_descender);
    module.set_const("KEY_Cyrillic_CHE_vertstroke", (guint) mousetrap::KEY_Cyrillic_CHE_vertstroke);
    module.set_const("KEY_Cyrillic_che_vertstroke", (guint) mousetrap::KEY_Cyrillic_che_vertstroke);
    module.set_const("KEY_Cyrillic_SHHA", (guint) mousetrap::KEY_Cyrillic_SHHA);
    module.set_const("KEY_Cyrillic_shha", (guint) mousetrap::KEY_Cyrillic_shha);
    module.set_const("KEY_Cyrillic_SCHWA", (guint) mousetrap::KEY_Cyrillic_SCHWA);
    module.set_const("KEY_Cyrillic_schwa", (guint) mousetrap::KEY_Cyrillic_schwa);
    module.set_const("KEY_Cyrillic_I_macron", (guint) mousetrap::KEY_Cyrillic_I_macron);
    module.set_const("KEY_Cyrillic_i_macron", (guint) mousetrap::KEY_Cyrillic_i_macron);
    module.set_const("KEY_Cyrillic_O_bar", (guint) mousetrap::KEY_Cyrillic_O_bar);
    module.set_const("KEY_Cyrillic_o_bar", (guint) mousetrap::KEY_Cyrillic_o_bar);
    module.set_const("KEY_Cyrillic_U_macron", (guint) mousetrap::KEY_Cyrillic_U_macron);
    module.set_const("KEY_Cyrillic_u_macron", (guint) mousetrap::KEY_Cyrillic_u_macron);
    module.set_const("KEY_Serbian_dje", (guint) mousetrap::KEY_Serbian_dje);
    module.set_const("KEY_Macedonia_gje", (guint) mousetrap::KEY_Macedonia_gje);
    module.set_const("KEY_Cyrillic_io", (guint) mousetrap::KEY_Cyrillic_io);
    module.set_const("KEY_Ukrainian_ie", (guint) mousetrap::KEY_Ukrainian_ie);
    module.set_const("KEY_Ukranian_je", (guint) mousetrap::KEY_Ukranian_je);
    module.set_const("KEY_Macedonia_dse", (guint) mousetrap::KEY_Macedonia_dse);
    module.set_const("KEY_Ukrainian_i", (guint) mousetrap::KEY_Ukrainian_i);
    module.set_const("KEY_Ukranian_i", (guint) mousetrap::KEY_Ukranian_i);
    module.set_const("KEY_Ukrainian_yi", (guint) mousetrap::KEY_Ukrainian_yi);
    module.set_const("KEY_Ukranian_yi", (guint) mousetrap::KEY_Ukranian_yi);
    module.set_const("KEY_Cyrillic_je", (guint) mousetrap::KEY_Cyrillic_je);
    module.set_const("KEY_Serbian_je", (guint) mousetrap::KEY_Serbian_je);
    module.set_const("KEY_Cyrillic_lje", (guint) mousetrap::KEY_Cyrillic_lje);
    module.set_const("KEY_Serbian_lje", (guint) mousetrap::KEY_Serbian_lje);
    module.set_const("KEY_Cyrillic_nje", (guint) mousetrap::KEY_Cyrillic_nje);
    module.set_const("KEY_Serbian_nje", (guint) mousetrap::KEY_Serbian_nje);
    module.set_const("KEY_Serbian_tshe", (guint) mousetrap::KEY_Serbian_tshe);
    module.set_const("KEY_Macedonia_kje", (guint) mousetrap::KEY_Macedonia_kje);
    module.set_const("KEY_Ukrainian_ghe_with_upturn", (guint) mousetrap::KEY_Ukrainian_ghe_with_upturn);
    module.set_const("KEY_Byelorussian_shortu", (guint) mousetrap::KEY_Byelorussian_shortu);
    module.set_const("KEY_Cyrillic_dzhe", (guint) mousetrap::KEY_Cyrillic_dzhe);
    module.set_const("KEY_Serbian_dze", (guint) mousetrap::KEY_Serbian_dze);
    module.set_const("KEY_numerosign", (guint) mousetrap::KEY_numerosign);
    module.set_const("KEY_Serbian_DJE", (guint) mousetrap::KEY_Serbian_DJE);
    module.set_const("KEY_Macedonia_GJE", (guint) mousetrap::KEY_Macedonia_GJE);
    module.set_const("KEY_Cyrillic_IO", (guint) mousetrap::KEY_Cyrillic_IO);
    module.set_const("KEY_Ukrainian_IE", (guint) mousetrap::KEY_Ukrainian_IE);
    module.set_const("KEY_Ukranian_JE", (guint) mousetrap::KEY_Ukranian_JE);
    module.set_const("KEY_Macedonia_DSE", (guint) mousetrap::KEY_Macedonia_DSE);
    module.set_const("KEY_Ukrainian_I", (guint) mousetrap::KEY_Ukrainian_I);
    module.set_const("KEY_Ukranian_I", (guint) mousetrap::KEY_Ukranian_I);
    module.set_const("KEY_Ukrainian_YI", (guint) mousetrap::KEY_Ukrainian_YI);
    module.set_const("KEY_Ukranian_YI", (guint) mousetrap::KEY_Ukranian_YI);
    module.set_const("KEY_Cyrillic_JE", (guint) mousetrap::KEY_Cyrillic_JE);
    module.set_const("KEY_Serbian_JE", (guint) mousetrap::KEY_Serbian_JE);
    module.set_const("KEY_Cyrillic_LJE", (guint) mousetrap::KEY_Cyrillic_LJE);
    module.set_const("KEY_Serbian_LJE", (guint) mousetrap::KEY_Serbian_LJE);
    module.set_const("KEY_Cyrillic_NJE", (guint) mousetrap::KEY_Cyrillic_NJE);
    module.set_const("KEY_Serbian_NJE", (guint) mousetrap::KEY_Serbian_NJE);
    module.set_const("KEY_Serbian_TSHE", (guint) mousetrap::KEY_Serbian_TSHE);
    module.set_const("KEY_Macedonia_KJE", (guint) mousetrap::KEY_Macedonia_KJE);
    module.set_const("KEY_Ukrainian_GHE_WITH_UPTURN", (guint) mousetrap::KEY_Ukrainian_GHE_WITH_UPTURN);
    module.set_const("KEY_Byelorussian_SHORTU", (guint) mousetrap::KEY_Byelorussian_SHORTU);
    module.set_const("KEY_Cyrillic_DZHE", (guint) mousetrap::KEY_Cyrillic_DZHE);
    module.set_const("KEY_Serbian_DZE", (guint) mousetrap::KEY_Serbian_DZE);
    module.set_const("KEY_Cyrillic_yu", (guint) mousetrap::KEY_Cyrillic_yu);
    module.set_const("KEY_Cyrillic_a", (guint) mousetrap::KEY_Cyrillic_a);
    module.set_const("KEY_Cyrillic_be", (guint) mousetrap::KEY_Cyrillic_be);
    module.set_const("KEY_Cyrillic_tse", (guint) mousetrap::KEY_Cyrillic_tse);
    module.set_const("KEY_Cyrillic_de", (guint) mousetrap::KEY_Cyrillic_de);
    module.set_const("KEY_Cyrillic_ie", (guint) mousetrap::KEY_Cyrillic_ie);
    module.set_const("KEY_Cyrillic_ef", (guint) mousetrap::KEY_Cyrillic_ef);
    module.set_const("KEY_Cyrillic_ghe", (guint) mousetrap::KEY_Cyrillic_ghe);
    module.set_const("KEY_Cyrillic_ha", (guint) mousetrap::KEY_Cyrillic_ha);
    module.set_const("KEY_Cyrillic_i", (guint) mousetrap::KEY_Cyrillic_i);
    module.set_const("KEY_Cyrillic_shorti", (guint) mousetrap::KEY_Cyrillic_shorti);
    module.set_const("KEY_Cyrillic_ka", (guint) mousetrap::KEY_Cyrillic_ka);
    module.set_const("KEY_Cyrillic_el", (guint) mousetrap::KEY_Cyrillic_el);
    module.set_const("KEY_Cyrillic_em", (guint) mousetrap::KEY_Cyrillic_em);
    module.set_const("KEY_Cyrillic_en", (guint) mousetrap::KEY_Cyrillic_en);
    module.set_const("KEY_Cyrillic_o", (guint) mousetrap::KEY_Cyrillic_o);
    module.set_const("KEY_Cyrillic_pe", (guint) mousetrap::KEY_Cyrillic_pe);
    module.set_const("KEY_Cyrillic_ya", (guint) mousetrap::KEY_Cyrillic_ya);
    module.set_const("KEY_Cyrillic_er", (guint) mousetrap::KEY_Cyrillic_er);
    module.set_const("KEY_Cyrillic_es", (guint) mousetrap::KEY_Cyrillic_es);
    module.set_const("KEY_Cyrillic_te", (guint) mousetrap::KEY_Cyrillic_te);
    module.set_const("KEY_Cyrillic_u", (guint) mousetrap::KEY_Cyrillic_u);
    module.set_const("KEY_Cyrillic_zhe", (guint) mousetrap::KEY_Cyrillic_zhe);
    module.set_const("KEY_Cyrillic_ve", (guint) mousetrap::KEY_Cyrillic_ve);
    module.set_const("KEY_Cyrillic_softsign", (guint) mousetrap::KEY_Cyrillic_softsign);
    module.set_const("KEY_Cyrillic_yeru", (guint) mousetrap::KEY_Cyrillic_yeru);
    module.set_const("KEY_Cyrillic_ze", (guint) mousetrap::KEY_Cyrillic_ze);
    module.set_const("KEY_Cyrillic_sha", (guint) mousetrap::KEY_Cyrillic_sha);
    module.set_const("KEY_Cyrillic_e", (guint) mousetrap::KEY_Cyrillic_e);
    module.set_const("KEY_Cyrillic_shcha", (guint) mousetrap::KEY_Cyrillic_shcha);
    module.set_const("KEY_Cyrillic_che", (guint) mousetrap::KEY_Cyrillic_che);
    module.set_const("KEY_Cyrillic_hardsign", (guint) mousetrap::KEY_Cyrillic_hardsign);
    module.set_const("KEY_Cyrillic_YU", (guint) mousetrap::KEY_Cyrillic_YU);
    module.set_const("KEY_Cyrillic_A", (guint) mousetrap::KEY_Cyrillic_A);
    module.set_const("KEY_Cyrillic_BE", (guint) mousetrap::KEY_Cyrillic_BE);
    module.set_const("KEY_Cyrillic_TSE", (guint) mousetrap::KEY_Cyrillic_TSE);
    module.set_const("KEY_Cyrillic_DE", (guint) mousetrap::KEY_Cyrillic_DE);
    module.set_const("KEY_Cyrillic_IE", (guint) mousetrap::KEY_Cyrillic_IE);
    module.set_const("KEY_Cyrillic_EF", (guint) mousetrap::KEY_Cyrillic_EF);
    module.set_const("KEY_Cyrillic_GHE", (guint) mousetrap::KEY_Cyrillic_GHE);
    module.set_const("KEY_Cyrillic_HA", (guint) mousetrap::KEY_Cyrillic_HA);
    module.set_const("KEY_Cyrillic_I", (guint) mousetrap::KEY_Cyrillic_I);
    module.set_const("KEY_Cyrillic_SHORTI", (guint) mousetrap::KEY_Cyrillic_SHORTI);
    module.set_const("KEY_Cyrillic_KA", (guint) mousetrap::KEY_Cyrillic_KA);
    module.set_const("KEY_Cyrillic_EL", (guint) mousetrap::KEY_Cyrillic_EL);
    module.set_const("KEY_Cyrillic_EM", (guint) mousetrap::KEY_Cyrillic_EM);
    module.set_const("KEY_Cyrillic_EN", (guint) mousetrap::KEY_Cyrillic_EN);
    module.set_const("KEY_Cyrillic_O", (guint) mousetrap::KEY_Cyrillic_O);
    module.set_const("KEY_Cyrillic_PE", (guint) mousetrap::KEY_Cyrillic_PE);
    module.set_const("KEY_Cyrillic_YA", (guint) mousetrap::KEY_Cyrillic_YA);
    module.set_const("KEY_Cyrillic_ER", (guint) mousetrap::KEY_Cyrillic_ER);
    module.set_const("KEY_Cyrillic_ES", (guint) mousetrap::KEY_Cyrillic_ES);
    module.set_const("KEY_Cyrillic_TE", (guint) mousetrap::KEY_Cyrillic_TE);
    module.set_const("KEY_Cyrillic_U", (guint) mousetrap::KEY_Cyrillic_U);
    module.set_const("KEY_Cyrillic_ZHE", (guint) mousetrap::KEY_Cyrillic_ZHE);
    module.set_const("KEY_Cyrillic_VE", (guint) mousetrap::KEY_Cyrillic_VE);
    module.set_const("KEY_Cyrillic_SOFTSIGN", (guint) mousetrap::KEY_Cyrillic_SOFTSIGN);
    module.set_const("KEY_Cyrillic_YERU", (guint) mousetrap::KEY_Cyrillic_YERU);
    module.set_const("KEY_Cyrillic_ZE", (guint) mousetrap::KEY_Cyrillic_ZE);
    module.set_const("KEY_Cyrillic_SHA", (guint) mousetrap::KEY_Cyrillic_SHA);
    module.set_const("KEY_Cyrillic_E", (guint) mousetrap::KEY_Cyrillic_E);
    module.set_const("KEY_Cyrillic_SHCHA", (guint) mousetrap::KEY_Cyrillic_SHCHA);
    module.set_const("KEY_Cyrillic_CHE", (guint) mousetrap::KEY_Cyrillic_CHE);
    module.set_const("KEY_Cyrillic_HARDSIGN", (guint) mousetrap::KEY_Cyrillic_HARDSIGN);
    module.set_const("KEY_Greek_ALPHAaccent", (guint) mousetrap::KEY_Greek_ALPHAaccent);
    module.set_const("KEY_Greek_EPSILONaccent", (guint) mousetrap::KEY_Greek_EPSILONaccent);
    module.set_const("KEY_Greek_ETAaccent", (guint) mousetrap::KEY_Greek_ETAaccent);
    module.set_const("KEY_Greek_IOTAaccent", (guint) mousetrap::KEY_Greek_IOTAaccent);
    module.set_const("KEY_Greek_IOTAdieresis", (guint) mousetrap::KEY_Greek_IOTAdieresis);
    module.set_const("KEY_Greek_IOTAdiaeresis", (guint) mousetrap::KEY_Greek_IOTAdiaeresis);
    module.set_const("KEY_Greek_OMICRONaccent", (guint) mousetrap::KEY_Greek_OMICRONaccent);
    module.set_const("KEY_Greek_UPSILONaccent", (guint) mousetrap::KEY_Greek_UPSILONaccent);
    module.set_const("KEY_Greek_UPSILONdieresis", (guint) mousetrap::KEY_Greek_UPSILONdieresis);
    module.set_const("KEY_Greek_OMEGAaccent", (guint) mousetrap::KEY_Greek_OMEGAaccent);
    module.set_const("KEY_Greek_accentdieresis", (guint) mousetrap::KEY_Greek_accentdieresis);
    module.set_const("KEY_Greek_horizbar", (guint) mousetrap::KEY_Greek_horizbar);
    module.set_const("KEY_Greek_alphaaccent", (guint) mousetrap::KEY_Greek_alphaaccent);
    module.set_const("KEY_Greek_epsilonaccent", (guint) mousetrap::KEY_Greek_epsilonaccent);
    module.set_const("KEY_Greek_etaaccent", (guint) mousetrap::KEY_Greek_etaaccent);
    module.set_const("KEY_Greek_iotaaccent", (guint) mousetrap::KEY_Greek_iotaaccent);
    module.set_const("KEY_Greek_iotadieresis", (guint) mousetrap::KEY_Greek_iotadieresis);
    module.set_const("KEY_Greek_iotaaccentdieresis", (guint) mousetrap::KEY_Greek_iotaaccentdieresis);
    module.set_const("KEY_Greek_omicronaccent", (guint) mousetrap::KEY_Greek_omicronaccent);
    module.set_const("KEY_Greek_upsilonaccent", (guint) mousetrap::KEY_Greek_upsilonaccent);
    module.set_const("KEY_Greek_upsilondieresis", (guint) mousetrap::KEY_Greek_upsilondieresis);
    module.set_const("KEY_Greek_upsilonaccentdieresis", (guint) mousetrap::KEY_Greek_upsilonaccentdieresis);
    module.set_const("KEY_Greek_omegaaccent", (guint) mousetrap::KEY_Greek_omegaaccent);
    module.set_const("KEY_Greek_ALPHA", (guint) mousetrap::KEY_Greek_ALPHA);
    module.set_const("KEY_Greek_BETA", (guint) mousetrap::KEY_Greek_BETA);
    module.set_const("KEY_Greek_GAMMA", (guint) mousetrap::KEY_Greek_GAMMA);
    module.set_const("KEY_Greek_DELTA", (guint) mousetrap::KEY_Greek_DELTA);
    module.set_const("KEY_Greek_EPSILON", (guint) mousetrap::KEY_Greek_EPSILON);
    module.set_const("KEY_Greek_ZETA", (guint) mousetrap::KEY_Greek_ZETA);
    module.set_const("KEY_Greek_ETA", (guint) mousetrap::KEY_Greek_ETA);
    module.set_const("KEY_Greek_THETA", (guint) mousetrap::KEY_Greek_THETA);
    module.set_const("KEY_Greek_IOTA", (guint) mousetrap::KEY_Greek_IOTA);
    module.set_const("KEY_Greek_KAPPA", (guint) mousetrap::KEY_Greek_KAPPA);
    module.set_const("KEY_Greek_LAMDA", (guint) mousetrap::KEY_Greek_LAMDA);
    module.set_const("KEY_Greek_LAMBDA", (guint) mousetrap::KEY_Greek_LAMBDA);
    module.set_const("KEY_Greek_MU", (guint) mousetrap::KEY_Greek_MU);
    module.set_const("KEY_Greek_NU", (guint) mousetrap::KEY_Greek_NU);
    module.set_const("KEY_Greek_XI", (guint) mousetrap::KEY_Greek_XI);
    module.set_const("KEY_Greek_OMICRON", (guint) mousetrap::KEY_Greek_OMICRON);
    module.set_const("KEY_Greek_PI", (guint) mousetrap::KEY_Greek_PI);
    module.set_const("KEY_Greek_RHO", (guint) mousetrap::KEY_Greek_RHO);
    module.set_const("KEY_Greek_SIGMA", (guint) mousetrap::KEY_Greek_SIGMA);
    module.set_const("KEY_Greek_TAU", (guint) mousetrap::KEY_Greek_TAU);
    module.set_const("KEY_Greek_UPSILON", (guint) mousetrap::KEY_Greek_UPSILON);
    module.set_const("KEY_Greek_PHI", (guint) mousetrap::KEY_Greek_PHI);
    module.set_const("KEY_Greek_CHI", (guint) mousetrap::KEY_Greek_CHI);
    module.set_const("KEY_Greek_PSI", (guint) mousetrap::KEY_Greek_PSI);
    module.set_const("KEY_Greek_OMEGA", (guint) mousetrap::KEY_Greek_OMEGA);
    module.set_const("KEY_Greek_alpha", (guint) mousetrap::KEY_Greek_alpha);
    module.set_const("KEY_Greek_beta", (guint) mousetrap::KEY_Greek_beta);
    module.set_const("KEY_Greek_gamma", (guint) mousetrap::KEY_Greek_gamma);
    module.set_const("KEY_Greek_delta", (guint) mousetrap::KEY_Greek_delta);
    module.set_const("KEY_Greek_epsilon", (guint) mousetrap::KEY_Greek_epsilon);
    module.set_const("KEY_Greek_zeta", (guint) mousetrap::KEY_Greek_zeta);
    module.set_const("KEY_Greek_eta", (guint) mousetrap::KEY_Greek_eta);
    module.set_const("KEY_Greek_theta", (guint) mousetrap::KEY_Greek_theta);
    module.set_const("KEY_Greek_iota", (guint) mousetrap::KEY_Greek_iota);
    module.set_const("KEY_Greek_kappa", (guint) mousetrap::KEY_Greek_kappa);
    module.set_const("KEY_Greek_lamda", (guint) mousetrap::KEY_Greek_lamda);
    module.set_const("KEY_Greek_lambda", (guint) mousetrap::KEY_Greek_lambda);
    module.set_const("KEY_Greek_mu", (guint) mousetrap::KEY_Greek_mu);
    module.set_const("KEY_Greek_nu", (guint) mousetrap::KEY_Greek_nu);
    module.set_const("KEY_Greek_xi", (guint) mousetrap::KEY_Greek_xi);
    module.set_const("KEY_Greek_omicron", (guint) mousetrap::KEY_Greek_omicron);
    module.set_const("KEY_Greek_pi", (guint) mousetrap::KEY_Greek_pi);
    module.set_const("KEY_Greek_rho", (guint) mousetrap::KEY_Greek_rho);
    module.set_const("KEY_Greek_sigma", (guint) mousetrap::KEY_Greek_sigma);
    module.set_const("KEY_Greek_finalsmallsigma", (guint) mousetrap::KEY_Greek_finalsmallsigma);
    module.set_const("KEY_Greek_tau", (guint) mousetrap::KEY_Greek_tau);
    module.set_const("KEY_Greek_upsilon", (guint) mousetrap::KEY_Greek_upsilon);
    module.set_const("KEY_Greek_phi", (guint) mousetrap::KEY_Greek_phi);
    module.set_const("KEY_Greek_chi", (guint) mousetrap::KEY_Greek_chi);
    module.set_const("KEY_Greek_psi", (guint) mousetrap::KEY_Greek_psi);
    module.set_const("KEY_Greek_omega", (guint) mousetrap::KEY_Greek_omega);
    module.set_const("KEY_Greek_switch", (guint) mousetrap::KEY_Greek_switch);
    module.set_const("KEY_leftradical", (guint) mousetrap::KEY_leftradical);
    module.set_const("KEY_topleftradical", (guint) mousetrap::KEY_topleftradical);
    module.set_const("KEY_horizconnector", (guint) mousetrap::KEY_horizconnector);
    module.set_const("KEY_topintegral", (guint) mousetrap::KEY_topintegral);
    module.set_const("KEY_botintegral", (guint) mousetrap::KEY_botintegral);
    module.set_const("KEY_vertconnector", (guint) mousetrap::KEY_vertconnector);
    module.set_const("KEY_topleftsqbracket", (guint) mousetrap::KEY_topleftsqbracket);
    module.set_const("KEY_botleftsqbracket", (guint) mousetrap::KEY_botleftsqbracket);
    module.set_const("KEY_toprightsqbracket", (guint) mousetrap::KEY_toprightsqbracket);
    module.set_const("KEY_botrightsqbracket", (guint) mousetrap::KEY_botrightsqbracket);
    module.set_const("KEY_topleftparens", (guint) mousetrap::KEY_topleftparens);
    module.set_const("KEY_botleftparens", (guint) mousetrap::KEY_botleftparens);
    module.set_const("KEY_toprightparens", (guint) mousetrap::KEY_toprightparens);
    module.set_const("KEY_botrightparens", (guint) mousetrap::KEY_botrightparens);
    module.set_const("KEY_leftmiddlecurlybrace", (guint) mousetrap::KEY_leftmiddlecurlybrace);
    module.set_const("KEY_rightmiddlecurlybrace", (guint) mousetrap::KEY_rightmiddlecurlybrace);
    module.set_const("KEY_topleftsummation", (guint) mousetrap::KEY_topleftsummation);
    module.set_const("KEY_botleftsummation", (guint) mousetrap::KEY_botleftsummation);
    module.set_const("KEY_topvertsummationconnector", (guint) mousetrap::KEY_topvertsummationconnector);
    module.set_const("KEY_botvertsummationconnector", (guint) mousetrap::KEY_botvertsummationconnector);
    module.set_const("KEY_toprightsummation", (guint) mousetrap::KEY_toprightsummation);
    module.set_const("KEY_botrightsummation", (guint) mousetrap::KEY_botrightsummation);
    module.set_const("KEY_rightmiddlesummation", (guint) mousetrap::KEY_rightmiddlesummation);
    module.set_const("KEY_lessthanequal", (guint) mousetrap::KEY_lessthanequal);
    module.set_const("KEY_notequal", (guint) mousetrap::KEY_notequal);
    module.set_const("KEY_greaterthanequal", (guint) mousetrap::KEY_greaterthanequal);
    module.set_const("KEY_integral", (guint) mousetrap::KEY_integral);
    module.set_const("KEY_therefore", (guint) mousetrap::KEY_therefore);
    module.set_const("KEY_variation", (guint) mousetrap::KEY_variation);
    module.set_const("KEY_infinity", (guint) mousetrap::KEY_infinity);
    module.set_const("KEY_nabla", (guint) mousetrap::KEY_nabla);
    module.set_const("KEY_approximate", (guint) mousetrap::KEY_approximate);
    module.set_const("KEY_similarequal", (guint) mousetrap::KEY_similarequal);
    module.set_const("KEY_ifonlyif", (guint) mousetrap::KEY_ifonlyif);
    module.set_const("KEY_implies", (guint) mousetrap::KEY_implies);
    module.set_const("KEY_identical", (guint) mousetrap::KEY_identical);
    module.set_const("KEY_radical", (guint) mousetrap::KEY_radical);
    module.set_const("KEY_includedin", (guint) mousetrap::KEY_includedin);
    module.set_const("KEY_includes", (guint) mousetrap::KEY_includes);
    module.set_const("KEY_intersection", (guint) mousetrap::KEY_intersection);
    module.set_const("KEY_union", (guint) mousetrap::KEY_union);
    module.set_const("KEY_logicaland", (guint) mousetrap::KEY_logicaland);
    module.set_const("KEY_logicalor", (guint) mousetrap::KEY_logicalor);
    module.set_const("KEY_partialderivative", (guint) mousetrap::KEY_partialderivative);
    module.set_const("KEY_function", (guint) mousetrap::KEY_function);
    module.set_const("KEY_leftarrow", (guint) mousetrap::KEY_leftarrow);
    module.set_const("KEY_uparrow", (guint) mousetrap::KEY_uparrow);
    module.set_const("KEY_rightarrow", (guint) mousetrap::KEY_rightarrow);
    module.set_const("KEY_downarrow", (guint) mousetrap::KEY_downarrow);
    module.set_const("KEY_blank", (guint) mousetrap::KEY_blank);
    module.set_const("KEY_soliddiamond", (guint) mousetrap::KEY_soliddiamond);
    module.set_const("KEY_checkerboard", (guint) mousetrap::KEY_checkerboard);
    module.set_const("KEY_ht", (guint) mousetrap::KEY_ht);
    module.set_const("KEY_ff", (guint) mousetrap::KEY_ff);
    module.set_const("KEY_cr", (guint) mousetrap::KEY_cr);
    module.set_const("KEY_lf", (guint) mousetrap::KEY_lf);
    module.set_const("KEY_nl", (guint) mousetrap::KEY_nl);
    module.set_const("KEY_vt", (guint) mousetrap::KEY_vt);
    module.set_const("KEY_lowrightcorner", (guint) mousetrap::KEY_lowrightcorner);
    module.set_const("KEY_uprightcorner", (guint) mousetrap::KEY_uprightcorner);
    module.set_const("KEY_upleftcorner", (guint) mousetrap::KEY_upleftcorner);
    module.set_const("KEY_lowleftcorner", (guint) mousetrap::KEY_lowleftcorner);
    module.set_const("KEY_crossinglines", (guint) mousetrap::KEY_crossinglines);
    module.set_const("KEY_horizlinescan1", (guint) mousetrap::KEY_horizlinescan1);
    module.set_const("KEY_horizlinescan3", (guint) mousetrap::KEY_horizlinescan3);
    module.set_const("KEY_horizlinescan5", (guint) mousetrap::KEY_horizlinescan5);
    module.set_const("KEY_horizlinescan7", (guint) mousetrap::KEY_horizlinescan7);
    module.set_const("KEY_horizlinescan9", (guint) mousetrap::KEY_horizlinescan9);
    module.set_const("KEY_leftt", (guint) mousetrap::KEY_leftt);
    module.set_const("KEY_rightt", (guint) mousetrap::KEY_rightt);
    module.set_const("KEY_bott", (guint) mousetrap::KEY_bott);
    module.set_const("KEY_topt", (guint) mousetrap::KEY_topt);
    module.set_const("KEY_vertbar", (guint) mousetrap::KEY_vertbar);
    module.set_const("KEY_emspace", (guint) mousetrap::KEY_emspace);
    module.set_const("KEY_enspace", (guint) mousetrap::KEY_enspace);
    module.set_const("KEY_em3space", (guint) mousetrap::KEY_em3space);
    module.set_const("KEY_em4space", (guint) mousetrap::KEY_em4space);
    module.set_const("KEY_digitspace", (guint) mousetrap::KEY_digitspace);
    module.set_const("KEY_punctspace", (guint) mousetrap::KEY_punctspace);
    module.set_const("KEY_thinspace", (guint) mousetrap::KEY_thinspace);
    module.set_const("KEY_hairspace", (guint) mousetrap::KEY_hairspace);
    module.set_const("KEY_emdash", (guint) mousetrap::KEY_emdash);
    module.set_const("KEY_endash", (guint) mousetrap::KEY_endash);
    module.set_const("KEY_signifblank", (guint) mousetrap::KEY_signifblank);
    module.set_const("KEY_ellipsis", (guint) mousetrap::KEY_ellipsis);
    module.set_const("KEY_doubbaselinedot", (guint) mousetrap::KEY_doubbaselinedot);
    module.set_const("KEY_onethird", (guint) mousetrap::KEY_onethird);
    module.set_const("KEY_twothirds", (guint) mousetrap::KEY_twothirds);
    module.set_const("KEY_onefifth", (guint) mousetrap::KEY_onefifth);
    module.set_const("KEY_twofifths", (guint) mousetrap::KEY_twofifths);
    module.set_const("KEY_threefifths", (guint) mousetrap::KEY_threefifths);
    module.set_const("KEY_fourfifths", (guint) mousetrap::KEY_fourfifths);
    module.set_const("KEY_onesixth", (guint) mousetrap::KEY_onesixth);
    module.set_const("KEY_fivesixths", (guint) mousetrap::KEY_fivesixths);
    module.set_const("KEY_careof", (guint) mousetrap::KEY_careof);
    module.set_const("KEY_figdash", (guint) mousetrap::KEY_figdash);
    module.set_const("KEY_leftanglebracket", (guint) mousetrap::KEY_leftanglebracket);
    module.set_const("KEY_decimalpoint", (guint) mousetrap::KEY_decimalpoint);
    module.set_const("KEY_rightanglebracket", (guint) mousetrap::KEY_rightanglebracket);
    module.set_const("KEY_marker", (guint) mousetrap::KEY_marker);
    module.set_const("KEY_oneeighth", (guint) mousetrap::KEY_oneeighth);
    module.set_const("KEY_threeeighths", (guint) mousetrap::KEY_threeeighths);
    module.set_const("KEY_fiveeighths", (guint) mousetrap::KEY_fiveeighths);
    module.set_const("KEY_seveneighths", (guint) mousetrap::KEY_seveneighths);
    module.set_const("KEY_trademark", (guint) mousetrap::KEY_trademark);
    module.set_const("KEY_signaturemark", (guint) mousetrap::KEY_signaturemark);
    module.set_const("KEY_trademarkincircle", (guint) mousetrap::KEY_trademarkincircle);
    module.set_const("KEY_leftopentriangle", (guint) mousetrap::KEY_leftopentriangle);
    module.set_const("KEY_rightopentriangle", (guint) mousetrap::KEY_rightopentriangle);
    module.set_const("KEY_emopencircle", (guint) mousetrap::KEY_emopencircle);
    module.set_const("KEY_emopenrectangle", (guint) mousetrap::KEY_emopenrectangle);
    module.set_const("KEY_leftsinglequotemark", (guint) mousetrap::KEY_leftsinglequotemark);
    module.set_const("KEY_rightsinglequotemark", (guint) mousetrap::KEY_rightsinglequotemark);
    module.set_const("KEY_leftdoublequotemark", (guint) mousetrap::KEY_leftdoublequotemark);
    module.set_const("KEY_rightdoublequotemark", (guint) mousetrap::KEY_rightdoublequotemark);
    module.set_const("KEY_prescription", (guint) mousetrap::KEY_prescription);
    module.set_const("KEY_permille", (guint) mousetrap::KEY_permille);
    module.set_const("KEY_minutes", (guint) mousetrap::KEY_minutes);
    module.set_const("KEY_seconds", (guint) mousetrap::KEY_seconds);
    module.set_const("KEY_latincross", (guint) mousetrap::KEY_latincross);
    module.set_const("KEY_hexagram", (guint) mousetrap::KEY_hexagram);
    module.set_const("KEY_filledrectbullet", (guint) mousetrap::KEY_filledrectbullet);
    module.set_const("KEY_filledlefttribullet", (guint) mousetrap::KEY_filledlefttribullet);
    module.set_const("KEY_filledrighttribullet", (guint) mousetrap::KEY_filledrighttribullet);
    module.set_const("KEY_emfilledcircle", (guint) mousetrap::KEY_emfilledcircle);
    module.set_const("KEY_emfilledrect", (guint) mousetrap::KEY_emfilledrect);
    module.set_const("KEY_enopencircbullet", (guint) mousetrap::KEY_enopencircbullet);
    module.set_const("KEY_enopensquarebullet", (guint) mousetrap::KEY_enopensquarebullet);
    module.set_const("KEY_openrectbullet", (guint) mousetrap::KEY_openrectbullet);
    module.set_const("KEY_opentribulletup", (guint) mousetrap::KEY_opentribulletup);
    module.set_const("KEY_opentribulletdown", (guint) mousetrap::KEY_opentribulletdown);
    module.set_const("KEY_openstar", (guint) mousetrap::KEY_openstar);
    module.set_const("KEY_enfilledcircbullet", (guint) mousetrap::KEY_enfilledcircbullet);
    module.set_const("KEY_enfilledsqbullet", (guint) mousetrap::KEY_enfilledsqbullet);
    module.set_const("KEY_filledtribulletup", (guint) mousetrap::KEY_filledtribulletup);
    module.set_const("KEY_filledtribulletdown", (guint) mousetrap::KEY_filledtribulletdown);
    module.set_const("KEY_leftpointer", (guint) mousetrap::KEY_leftpointer);
    module.set_const("KEY_rightpointer", (guint) mousetrap::KEY_rightpointer);
    module.set_const("KEY_club", (guint) mousetrap::KEY_club);
    module.set_const("KEY_diamond", (guint) mousetrap::KEY_diamond);
    module.set_const("KEY_heart", (guint) mousetrap::KEY_heart);
    module.set_const("KEY_maltesecross", (guint) mousetrap::KEY_maltesecross);
    module.set_const("KEY_dagger", (guint) mousetrap::KEY_dagger);
    module.set_const("KEY_doubledagger", (guint) mousetrap::KEY_doubledagger);
    module.set_const("KEY_checkmark", (guint) mousetrap::KEY_checkmark);
    module.set_const("KEY_ballotcross", (guint) mousetrap::KEY_ballotcross);
    module.set_const("KEY_musicalsharp", (guint) mousetrap::KEY_musicalsharp);
    module.set_const("KEY_musicalflat", (guint) mousetrap::KEY_musicalflat);
    module.set_const("KEY_malesymbol", (guint) mousetrap::KEY_malesymbol);
    module.set_const("KEY_femalesymbol", (guint) mousetrap::KEY_femalesymbol);
    module.set_const("KEY_telephone", (guint) mousetrap::KEY_telephone);
    module.set_const("KEY_telephonerecorder", (guint) mousetrap::KEY_telephonerecorder);
    module.set_const("KEY_phonographcopyright", (guint) mousetrap::KEY_phonographcopyright);
    module.set_const("KEY_caret", (guint) mousetrap::KEY_caret);
    module.set_const("KEY_singlelowquotemark", (guint) mousetrap::KEY_singlelowquotemark);
    module.set_const("KEY_doublelowquotemark", (guint) mousetrap::KEY_doublelowquotemark);
    module.set_const("KEY_cursor", (guint) mousetrap::KEY_cursor);
    module.set_const("KEY_leftcaret", (guint) mousetrap::KEY_leftcaret);
    module.set_const("KEY_rightcaret", (guint) mousetrap::KEY_rightcaret);
    module.set_const("KEY_downcaret", (guint) mousetrap::KEY_downcaret);
    module.set_const("KEY_upcaret", (guint) mousetrap::KEY_upcaret);
    module.set_const("KEY_overbar", (guint) mousetrap::KEY_overbar);
    module.set_const("KEY_downtack", (guint) mousetrap::KEY_downtack);
    module.set_const("KEY_upshoe", (guint) mousetrap::KEY_upshoe);
    module.set_const("KEY_downstile", (guint) mousetrap::KEY_downstile);
    module.set_const("KEY_underbar", (guint) mousetrap::KEY_underbar);
    module.set_const("KEY_jot", (guint) mousetrap::KEY_jot);
    module.set_const("KEY_quad", (guint) mousetrap::KEY_quad);
    module.set_const("KEY_uptack", (guint) mousetrap::KEY_uptack);
    module.set_const("KEY_circle", (guint) mousetrap::KEY_circle);
    module.set_const("KEY_upstile", (guint) mousetrap::KEY_upstile);
    module.set_const("KEY_downshoe", (guint) mousetrap::KEY_downshoe);
    module.set_const("KEY_rightshoe", (guint) mousetrap::KEY_rightshoe);
    module.set_const("KEY_leftshoe", (guint) mousetrap::KEY_leftshoe);
    module.set_const("KEY_lefttack", (guint) mousetrap::KEY_lefttack);
    module.set_const("KEY_righttack", (guint) mousetrap::KEY_righttack);
    module.set_const("KEY_hebrew_doublelowline", (guint) mousetrap::KEY_hebrew_doublelowline);
    module.set_const("KEY_hebrew_aleph", (guint) mousetrap::KEY_hebrew_aleph);
    module.set_const("KEY_hebrew_bet", (guint) mousetrap::KEY_hebrew_bet);
    module.set_const("KEY_hebrew_beth", (guint) mousetrap::KEY_hebrew_beth);
    module.set_const("KEY_hebrew_gimel", (guint) mousetrap::KEY_hebrew_gimel);
    module.set_const("KEY_hebrew_gimmel", (guint) mousetrap::KEY_hebrew_gimmel);
    module.set_const("KEY_hebrew_dalet", (guint) mousetrap::KEY_hebrew_dalet);
    module.set_const("KEY_hebrew_daleth", (guint) mousetrap::KEY_hebrew_daleth);
    module.set_const("KEY_hebrew_he", (guint) mousetrap::KEY_hebrew_he);
    module.set_const("KEY_hebrew_waw", (guint) mousetrap::KEY_hebrew_waw);
    module.set_const("KEY_hebrew_zain", (guint) mousetrap::KEY_hebrew_zain);
    module.set_const("KEY_hebrew_zayin", (guint) mousetrap::KEY_hebrew_zayin);
    module.set_const("KEY_hebrew_chet", (guint) mousetrap::KEY_hebrew_chet);
    module.set_const("KEY_hebrew_het", (guint) mousetrap::KEY_hebrew_het);
    module.set_const("KEY_hebrew_tet", (guint) mousetrap::KEY_hebrew_tet);
    module.set_const("KEY_hebrew_teth", (guint) mousetrap::KEY_hebrew_teth);
    module.set_const("KEY_hebrew_yod", (guint) mousetrap::KEY_hebrew_yod);
    module.set_const("KEY_hebrew_finalkaph", (guint) mousetrap::KEY_hebrew_finalkaph);
    module.set_const("KEY_hebrew_kaph", (guint) mousetrap::KEY_hebrew_kaph);
    module.set_const("KEY_hebrew_lamed", (guint) mousetrap::KEY_hebrew_lamed);
    module.set_const("KEY_hebrew_finalmem", (guint) mousetrap::KEY_hebrew_finalmem);
    module.set_const("KEY_hebrew_mem", (guint) mousetrap::KEY_hebrew_mem);
    module.set_const("KEY_hebrew_finalnun", (guint) mousetrap::KEY_hebrew_finalnun);
    module.set_const("KEY_hebrew_nun", (guint) mousetrap::KEY_hebrew_nun);
    module.set_const("KEY_hebrew_samech", (guint) mousetrap::KEY_hebrew_samech);
    module.set_const("KEY_hebrew_samekh", (guint) mousetrap::KEY_hebrew_samekh);
    module.set_const("KEY_hebrew_ayin", (guint) mousetrap::KEY_hebrew_ayin);
    module.set_const("KEY_hebrew_finalpe", (guint) mousetrap::KEY_hebrew_finalpe);
    module.set_const("KEY_hebrew_pe", (guint) mousetrap::KEY_hebrew_pe);
    module.set_const("KEY_hebrew_finalzade", (guint) mousetrap::KEY_hebrew_finalzade);
    module.set_const("KEY_hebrew_finalzadi", (guint) mousetrap::KEY_hebrew_finalzadi);
    module.set_const("KEY_hebrew_zade", (guint) mousetrap::KEY_hebrew_zade);
    module.set_const("KEY_hebrew_zadi", (guint) mousetrap::KEY_hebrew_zadi);
    module.set_const("KEY_hebrew_qoph", (guint) mousetrap::KEY_hebrew_qoph);
    module.set_const("KEY_hebrew_kuf", (guint) mousetrap::KEY_hebrew_kuf);
    module.set_const("KEY_hebrew_resh", (guint) mousetrap::KEY_hebrew_resh);
    module.set_const("KEY_hebrew_shin", (guint) mousetrap::KEY_hebrew_shin);
    module.set_const("KEY_hebrew_taw", (guint) mousetrap::KEY_hebrew_taw);
    module.set_const("KEY_hebrew_taf", (guint) mousetrap::KEY_hebrew_taf);
    module.set_const("KEY_Hebrew_switch", (guint) mousetrap::KEY_Hebrew_switch);
    module.set_const("KEY_Thai_kokai", (guint) mousetrap::KEY_Thai_kokai);
    module.set_const("KEY_Thai_khokhai", (guint) mousetrap::KEY_Thai_khokhai);
    module.set_const("KEY_Thai_khokhuat", (guint) mousetrap::KEY_Thai_khokhuat);
    module.set_const("KEY_Thai_khokhwai", (guint) mousetrap::KEY_Thai_khokhwai);
    module.set_const("KEY_Thai_khokhon", (guint) mousetrap::KEY_Thai_khokhon);
    module.set_const("KEY_Thai_khorakhang", (guint) mousetrap::KEY_Thai_khorakhang);
    module.set_const("KEY_Thai_ngongu", (guint) mousetrap::KEY_Thai_ngongu);
    module.set_const("KEY_Thai_chochan", (guint) mousetrap::KEY_Thai_chochan);
    module.set_const("KEY_Thai_choching", (guint) mousetrap::KEY_Thai_choching);
    module.set_const("KEY_Thai_chochang", (guint) mousetrap::KEY_Thai_chochang);
    module.set_const("KEY_Thai_soso", (guint) mousetrap::KEY_Thai_soso);
    module.set_const("KEY_Thai_chochoe", (guint) mousetrap::KEY_Thai_chochoe);
    module.set_const("KEY_Thai_yoying", (guint) mousetrap::KEY_Thai_yoying);
    module.set_const("KEY_Thai_dochada", (guint) mousetrap::KEY_Thai_dochada);
    module.set_const("KEY_Thai_topatak", (guint) mousetrap::KEY_Thai_topatak);
    module.set_const("KEY_Thai_thothan", (guint) mousetrap::KEY_Thai_thothan);
    module.set_const("KEY_Thai_thonangmontho", (guint) mousetrap::KEY_Thai_thonangmontho);
    module.set_const("KEY_Thai_thophuthao", (guint) mousetrap::KEY_Thai_thophuthao);
    module.set_const("KEY_Thai_nonen", (guint) mousetrap::KEY_Thai_nonen);
    module.set_const("KEY_Thai_dodek", (guint) mousetrap::KEY_Thai_dodek);
    module.set_const("KEY_Thai_totao", (guint) mousetrap::KEY_Thai_totao);
    module.set_const("KEY_Thai_thothung", (guint) mousetrap::KEY_Thai_thothung);
    module.set_const("KEY_Thai_thothahan", (guint) mousetrap::KEY_Thai_thothahan);
    module.set_const("KEY_Thai_thothong", (guint) mousetrap::KEY_Thai_thothong);
    module.set_const("KEY_Thai_nonu", (guint) mousetrap::KEY_Thai_nonu);
    module.set_const("KEY_Thai_bobaimai", (guint) mousetrap::KEY_Thai_bobaimai);
    module.set_const("KEY_Thai_popla", (guint) mousetrap::KEY_Thai_popla);
    module.set_const("KEY_Thai_phophung", (guint) mousetrap::KEY_Thai_phophung);
    module.set_const("KEY_Thai_fofa", (guint) mousetrap::KEY_Thai_fofa);
    module.set_const("KEY_Thai_phophan", (guint) mousetrap::KEY_Thai_phophan);
    module.set_const("KEY_Thai_fofan", (guint) mousetrap::KEY_Thai_fofan);
    module.set_const("KEY_Thai_phosamphao", (guint) mousetrap::KEY_Thai_phosamphao);
    module.set_const("KEY_Thai_moma", (guint) mousetrap::KEY_Thai_moma);
    module.set_const("KEY_Thai_yoyak", (guint) mousetrap::KEY_Thai_yoyak);
    module.set_const("KEY_Thai_rorua", (guint) mousetrap::KEY_Thai_rorua);
    module.set_const("KEY_Thai_ru", (guint) mousetrap::KEY_Thai_ru);
    module.set_const("KEY_Thai_loling", (guint) mousetrap::KEY_Thai_loling);
    module.set_const("KEY_Thai_lu", (guint) mousetrap::KEY_Thai_lu);
    module.set_const("KEY_Thai_wowaen", (guint) mousetrap::KEY_Thai_wowaen);
    module.set_const("KEY_Thai_sosala", (guint) mousetrap::KEY_Thai_sosala);
    module.set_const("KEY_Thai_sorusi", (guint) mousetrap::KEY_Thai_sorusi);
    module.set_const("KEY_Thai_sosua", (guint) mousetrap::KEY_Thai_sosua);
    module.set_const("KEY_Thai_hohip", (guint) mousetrap::KEY_Thai_hohip);
    module.set_const("KEY_Thai_lochula", (guint) mousetrap::KEY_Thai_lochula);
    module.set_const("KEY_Thai_oang", (guint) mousetrap::KEY_Thai_oang);
    module.set_const("KEY_Thai_honokhuk", (guint) mousetrap::KEY_Thai_honokhuk);
    module.set_const("KEY_Thai_paiyannoi", (guint) mousetrap::KEY_Thai_paiyannoi);
    module.set_const("KEY_Thai_saraa", (guint) mousetrap::KEY_Thai_saraa);
    module.set_const("KEY_Thai_maihanakat", (guint) mousetrap::KEY_Thai_maihanakat);
    module.set_const("KEY_Thai_saraaa", (guint) mousetrap::KEY_Thai_saraaa);
    module.set_const("KEY_Thai_saraam", (guint) mousetrap::KEY_Thai_saraam);
    module.set_const("KEY_Thai_sarai", (guint) mousetrap::KEY_Thai_sarai);
    module.set_const("KEY_Thai_saraii", (guint) mousetrap::KEY_Thai_saraii);
    module.set_const("KEY_Thai_saraue", (guint) mousetrap::KEY_Thai_saraue);
    module.set_const("KEY_Thai_sarauee", (guint) mousetrap::KEY_Thai_sarauee);
    module.set_const("KEY_Thai_sarau", (guint) mousetrap::KEY_Thai_sarau);
    module.set_const("KEY_Thai_sarauu", (guint) mousetrap::KEY_Thai_sarauu);
    module.set_const("KEY_Thai_phinthu", (guint) mousetrap::KEY_Thai_phinthu);
    module.set_const("KEY_Thai_maihanakat_maitho", (guint) mousetrap::KEY_Thai_maihanakat_maitho);
    module.set_const("KEY_Thai_baht", (guint) mousetrap::KEY_Thai_baht);
    module.set_const("KEY_Thai_sarae", (guint) mousetrap::KEY_Thai_sarae);
    module.set_const("KEY_Thai_saraae", (guint) mousetrap::KEY_Thai_saraae);
    module.set_const("KEY_Thai_sarao", (guint) mousetrap::KEY_Thai_sarao);
    module.set_const("KEY_Thai_saraaimaimuan", (guint) mousetrap::KEY_Thai_saraaimaimuan);
    module.set_const("KEY_Thai_saraaimaimalai", (guint) mousetrap::KEY_Thai_saraaimaimalai);
    module.set_const("KEY_Thai_lakkhangyao", (guint) mousetrap::KEY_Thai_lakkhangyao);
    module.set_const("KEY_Thai_maiyamok", (guint) mousetrap::KEY_Thai_maiyamok);
    module.set_const("KEY_Thai_maitaikhu", (guint) mousetrap::KEY_Thai_maitaikhu);
    module.set_const("KEY_Thai_maiek", (guint) mousetrap::KEY_Thai_maiek);
    module.set_const("KEY_Thai_maitho", (guint) mousetrap::KEY_Thai_maitho);
    module.set_const("KEY_Thai_maitri", (guint) mousetrap::KEY_Thai_maitri);
    module.set_const("KEY_Thai_maichattawa", (guint) mousetrap::KEY_Thai_maichattawa);
    module.set_const("KEY_Thai_thanthakhat", (guint) mousetrap::KEY_Thai_thanthakhat);
    module.set_const("KEY_Thai_nikhahit", (guint) mousetrap::KEY_Thai_nikhahit);
    module.set_const("KEY_Thai_leksun", (guint) mousetrap::KEY_Thai_leksun);
    module.set_const("KEY_Thai_leknung", (guint) mousetrap::KEY_Thai_leknung);
    module.set_const("KEY_Thai_leksong", (guint) mousetrap::KEY_Thai_leksong);
    module.set_const("KEY_Thai_leksam", (guint) mousetrap::KEY_Thai_leksam);
    module.set_const("KEY_Thai_leksi", (guint) mousetrap::KEY_Thai_leksi);
    module.set_const("KEY_Thai_lekha", (guint) mousetrap::KEY_Thai_lekha);
    module.set_const("KEY_Thai_lekhok", (guint) mousetrap::KEY_Thai_lekhok);
    module.set_const("KEY_Thai_lekchet", (guint) mousetrap::KEY_Thai_lekchet);
    module.set_const("KEY_Thai_lekpaet", (guint) mousetrap::KEY_Thai_lekpaet);
    module.set_const("KEY_Thai_lekkao", (guint) mousetrap::KEY_Thai_lekkao);
    module.set_const("KEY_Hangul", (guint) mousetrap::KEY_Hangul);
    module.set_const("KEY_Hangul_Start", (guint) mousetrap::KEY_Hangul_Start);
    module.set_const("KEY_Hangul_End", (guint) mousetrap::KEY_Hangul_End);
    module.set_const("KEY_Hangul_Hanja", (guint) mousetrap::KEY_Hangul_Hanja);
    module.set_const("KEY_Hangul_Jamo", (guint) mousetrap::KEY_Hangul_Jamo);
    module.set_const("KEY_Hangul_Romaja", (guint) mousetrap::KEY_Hangul_Romaja);
    module.set_const("KEY_Hangul_Codeinput", (guint) mousetrap::KEY_Hangul_Codeinput);
    module.set_const("KEY_Hangul_Jeonja", (guint) mousetrap::KEY_Hangul_Jeonja);
    module.set_const("KEY_Hangul_Banja", (guint) mousetrap::KEY_Hangul_Banja);
    module.set_const("KEY_Hangul_PreHanja", (guint) mousetrap::KEY_Hangul_PreHanja);
    module.set_const("KEY_Hangul_PostHanja", (guint) mousetrap::KEY_Hangul_PostHanja);
    module.set_const("KEY_Hangul_SingleCandidate", (guint) mousetrap::KEY_Hangul_SingleCandidate);
    module.set_const("KEY_Hangul_MultipleCandidate", (guint) mousetrap::KEY_Hangul_MultipleCandidate);
    module.set_const("KEY_Hangul_PreviousCandidate", (guint) mousetrap::KEY_Hangul_PreviousCandidate);
    module.set_const("KEY_Hangul_Special", (guint) mousetrap::KEY_Hangul_Special);
    module.set_const("KEY_Hangul_switch", (guint) mousetrap::KEY_Hangul_switch);
    module.set_const("KEY_Hangul_Kiyeog", (guint) mousetrap::KEY_Hangul_Kiyeog);
    module.set_const("KEY_Hangul_SsangKiyeog", (guint) mousetrap::KEY_Hangul_SsangKiyeog);
    module.set_const("KEY_Hangul_KiyeogSios", (guint) mousetrap::KEY_Hangul_KiyeogSios);
    module.set_const("KEY_Hangul_Nieun", (guint) mousetrap::KEY_Hangul_Nieun);
    module.set_const("KEY_Hangul_NieunJieuj", (guint) mousetrap::KEY_Hangul_NieunJieuj);
    module.set_const("KEY_Hangul_NieunHieuh", (guint) mousetrap::KEY_Hangul_NieunHieuh);
    module.set_const("KEY_Hangul_Dikeud", (guint) mousetrap::KEY_Hangul_Dikeud);
    module.set_const("KEY_Hangul_SsangDikeud", (guint) mousetrap::KEY_Hangul_SsangDikeud);
    module.set_const("KEY_Hangul_Rieul", (guint) mousetrap::KEY_Hangul_Rieul);
    module.set_const("KEY_Hangul_RieulKiyeog", (guint) mousetrap::KEY_Hangul_RieulKiyeog);
    module.set_const("KEY_Hangul_RieulMieum", (guint) mousetrap::KEY_Hangul_RieulMieum);
    module.set_const("KEY_Hangul_RieulPieub", (guint) mousetrap::KEY_Hangul_RieulPieub);
    module.set_const("KEY_Hangul_RieulSios", (guint) mousetrap::KEY_Hangul_RieulSios);
    module.set_const("KEY_Hangul_RieulTieut", (guint) mousetrap::KEY_Hangul_RieulTieut);
    module.set_const("KEY_Hangul_RieulPhieuf", (guint) mousetrap::KEY_Hangul_RieulPhieuf);
    module.set_const("KEY_Hangul_RieulHieuh", (guint) mousetrap::KEY_Hangul_RieulHieuh);
    module.set_const("KEY_Hangul_Mieum", (guint) mousetrap::KEY_Hangul_Mieum);
    module.set_const("KEY_Hangul_Pieub", (guint) mousetrap::KEY_Hangul_Pieub);
    module.set_const("KEY_Hangul_SsangPieub", (guint) mousetrap::KEY_Hangul_SsangPieub);
    module.set_const("KEY_Hangul_PieubSios", (guint) mousetrap::KEY_Hangul_PieubSios);
    module.set_const("KEY_Hangul_Sios", (guint) mousetrap::KEY_Hangul_Sios);
    module.set_const("KEY_Hangul_SsangSios", (guint) mousetrap::KEY_Hangul_SsangSios);
    module.set_const("KEY_Hangul_Ieung", (guint) mousetrap::KEY_Hangul_Ieung);
    module.set_const("KEY_Hangul_Jieuj", (guint) mousetrap::KEY_Hangul_Jieuj);
    module.set_const("KEY_Hangul_SsangJieuj", (guint) mousetrap::KEY_Hangul_SsangJieuj);
    module.set_const("KEY_Hangul_Cieuc", (guint) mousetrap::KEY_Hangul_Cieuc);
    module.set_const("KEY_Hangul_Khieuq", (guint) mousetrap::KEY_Hangul_Khieuq);
    module.set_const("KEY_Hangul_Tieut", (guint) mousetrap::KEY_Hangul_Tieut);
    module.set_const("KEY_Hangul_Phieuf", (guint) mousetrap::KEY_Hangul_Phieuf);
    module.set_const("KEY_Hangul_Hieuh", (guint) mousetrap::KEY_Hangul_Hieuh);
    module.set_const("KEY_Hangul_A", (guint) mousetrap::KEY_Hangul_A);
    module.set_const("KEY_Hangul_AE", (guint) mousetrap::KEY_Hangul_AE);
    module.set_const("KEY_Hangul_YA", (guint) mousetrap::KEY_Hangul_YA);
    module.set_const("KEY_Hangul_YAE", (guint) mousetrap::KEY_Hangul_YAE);
    module.set_const("KEY_Hangul_EO", (guint) mousetrap::KEY_Hangul_EO);
    module.set_const("KEY_Hangul_E", (guint) mousetrap::KEY_Hangul_E);
    module.set_const("KEY_Hangul_YEO", (guint) mousetrap::KEY_Hangul_YEO);
    module.set_const("KEY_Hangul_YE", (guint) mousetrap::KEY_Hangul_YE);
    module.set_const("KEY_Hangul_O", (guint) mousetrap::KEY_Hangul_O);
    module.set_const("KEY_Hangul_WA", (guint) mousetrap::KEY_Hangul_WA);
    module.set_const("KEY_Hangul_WAE", (guint) mousetrap::KEY_Hangul_WAE);
    module.set_const("KEY_Hangul_OE", (guint) mousetrap::KEY_Hangul_OE);
    module.set_const("KEY_Hangul_YO", (guint) mousetrap::KEY_Hangul_YO);
    module.set_const("KEY_Hangul_U", (guint) mousetrap::KEY_Hangul_U);
    module.set_const("KEY_Hangul_WEO", (guint) mousetrap::KEY_Hangul_WEO);
    module.set_const("KEY_Hangul_WE", (guint) mousetrap::KEY_Hangul_WE);
    module.set_const("KEY_Hangul_WI", (guint) mousetrap::KEY_Hangul_WI);
    module.set_const("KEY_Hangul_YU", (guint) mousetrap::KEY_Hangul_YU);
    module.set_const("KEY_Hangul_EU", (guint) mousetrap::KEY_Hangul_EU);
    module.set_const("KEY_Hangul_YI", (guint) mousetrap::KEY_Hangul_YI);
    module.set_const("KEY_Hangul_I", (guint) mousetrap::KEY_Hangul_I);
    module.set_const("KEY_Hangul_J_Kiyeog", (guint) mousetrap::KEY_Hangul_J_Kiyeog);
    module.set_const("KEY_Hangul_J_SsangKiyeog", (guint) mousetrap::KEY_Hangul_J_SsangKiyeog);
    module.set_const("KEY_Hangul_J_KiyeogSios", (guint) mousetrap::KEY_Hangul_J_KiyeogSios);
    module.set_const("KEY_Hangul_J_Nieun", (guint) mousetrap::KEY_Hangul_J_Nieun);
    module.set_const("KEY_Hangul_J_NieunJieuj", (guint) mousetrap::KEY_Hangul_J_NieunJieuj);
    module.set_const("KEY_Hangul_J_NieunHieuh", (guint) mousetrap::KEY_Hangul_J_NieunHieuh);
    module.set_const("KEY_Hangul_J_Dikeud", (guint) mousetrap::KEY_Hangul_J_Dikeud);
    module.set_const("KEY_Hangul_J_Rieul", (guint) mousetrap::KEY_Hangul_J_Rieul);
    module.set_const("KEY_Hangul_J_RieulKiyeog", (guint) mousetrap::KEY_Hangul_J_RieulKiyeog);
    module.set_const("KEY_Hangul_J_RieulMieum", (guint) mousetrap::KEY_Hangul_J_RieulMieum);
    module.set_const("KEY_Hangul_J_RieulPieub", (guint) mousetrap::KEY_Hangul_J_RieulPieub);
    module.set_const("KEY_Hangul_J_RieulSios", (guint) mousetrap::KEY_Hangul_J_RieulSios);
    module.set_const("KEY_Hangul_J_RieulTieut", (guint) mousetrap::KEY_Hangul_J_RieulTieut);
    module.set_const("KEY_Hangul_J_RieulPhieuf", (guint) mousetrap::KEY_Hangul_J_RieulPhieuf);
    module.set_const("KEY_Hangul_J_RieulHieuh", (guint) mousetrap::KEY_Hangul_J_RieulHieuh);
    module.set_const("KEY_Hangul_J_Mieum", (guint) mousetrap::KEY_Hangul_J_Mieum);
    module.set_const("KEY_Hangul_J_Pieub", (guint) mousetrap::KEY_Hangul_J_Pieub);
    module.set_const("KEY_Hangul_J_PieubSios", (guint) mousetrap::KEY_Hangul_J_PieubSios);
    module.set_const("KEY_Hangul_J_Sios", (guint) mousetrap::KEY_Hangul_J_Sios);
    module.set_const("KEY_Hangul_J_SsangSios", (guint) mousetrap::KEY_Hangul_J_SsangSios);
    module.set_const("KEY_Hangul_J_Ieung", (guint) mousetrap::KEY_Hangul_J_Ieung);
    module.set_const("KEY_Hangul_J_Jieuj", (guint) mousetrap::KEY_Hangul_J_Jieuj);
    module.set_const("KEY_Hangul_J_Cieuc", (guint) mousetrap::KEY_Hangul_J_Cieuc);
    module.set_const("KEY_Hangul_J_Khieuq", (guint) mousetrap::KEY_Hangul_J_Khieuq);
    module.set_const("KEY_Hangul_J_Tieut", (guint) mousetrap::KEY_Hangul_J_Tieut);
    module.set_const("KEY_Hangul_J_Phieuf", (guint) mousetrap::KEY_Hangul_J_Phieuf);
    module.set_const("KEY_Hangul_J_Hieuh", (guint) mousetrap::KEY_Hangul_J_Hieuh);
    module.set_const("KEY_Hangul_RieulYeorinHieuh", (guint) mousetrap::KEY_Hangul_RieulYeorinHieuh);
    module.set_const("KEY_Hangul_SunkyeongeumMieum", (guint) mousetrap::KEY_Hangul_SunkyeongeumMieum);
    module.set_const("KEY_Hangul_SunkyeongeumPieub", (guint) mousetrap::KEY_Hangul_SunkyeongeumPieub);
    module.set_const("KEY_Hangul_PanSios", (guint) mousetrap::KEY_Hangul_PanSios);
    module.set_const("KEY_Hangul_KkogjiDalrinIeung", (guint) mousetrap::KEY_Hangul_KkogjiDalrinIeung);
    module.set_const("KEY_Hangul_SunkyeongeumPhieuf", (guint) mousetrap::KEY_Hangul_SunkyeongeumPhieuf);
    module.set_const("KEY_Hangul_YeorinHieuh", (guint) mousetrap::KEY_Hangul_YeorinHieuh);
    module.set_const("KEY_Hangul_AraeA", (guint) mousetrap::KEY_Hangul_AraeA);
    module.set_const("KEY_Hangul_AraeAE", (guint) mousetrap::KEY_Hangul_AraeAE);
    module.set_const("KEY_Hangul_J_PanSios", (guint) mousetrap::KEY_Hangul_J_PanSios);
    module.set_const("KEY_Hangul_J_KkogjiDalrinIeung", (guint) mousetrap::KEY_Hangul_J_KkogjiDalrinIeung);
    module.set_const("KEY_Hangul_J_YeorinHieuh", (guint) mousetrap::KEY_Hangul_J_YeorinHieuh);
    module.set_const("KEY_Korean_Won", (guint) mousetrap::KEY_Korean_Won);
    module.set_const("KEY_Armenian_ligature_ew", (guint) mousetrap::KEY_Armenian_ligature_ew);
    module.set_const("KEY_Armenian_full_stop", (guint) mousetrap::KEY_Armenian_full_stop);
    module.set_const("KEY_Armenian_verjaket", (guint) mousetrap::KEY_Armenian_verjaket);
    module.set_const("KEY_Armenian_separation_mark", (guint) mousetrap::KEY_Armenian_separation_mark);
    module.set_const("KEY_Armenian_but", (guint) mousetrap::KEY_Armenian_but);
    module.set_const("KEY_Armenian_hyphen", (guint) mousetrap::KEY_Armenian_hyphen);
    module.set_const("KEY_Armenian_yentamna", (guint) mousetrap::KEY_Armenian_yentamna);
    module.set_const("KEY_Armenian_exclam", (guint) mousetrap::KEY_Armenian_exclam);
    module.set_const("KEY_Armenian_amanak", (guint) mousetrap::KEY_Armenian_amanak);
    module.set_const("KEY_Armenian_accent", (guint) mousetrap::KEY_Armenian_accent);
    module.set_const("KEY_Armenian_shesht", (guint) mousetrap::KEY_Armenian_shesht);
    module.set_const("KEY_Armenian_question", (guint) mousetrap::KEY_Armenian_question);
    module.set_const("KEY_Armenian_paruyk", (guint) mousetrap::KEY_Armenian_paruyk);
    module.set_const("KEY_Armenian_AYB", (guint) mousetrap::KEY_Armenian_AYB);
    module.set_const("KEY_Armenian_ayb", (guint) mousetrap::KEY_Armenian_ayb);
    module.set_const("KEY_Armenian_BEN", (guint) mousetrap::KEY_Armenian_BEN);
    module.set_const("KEY_Armenian_ben", (guint) mousetrap::KEY_Armenian_ben);
    module.set_const("KEY_Armenian_GIM", (guint) mousetrap::KEY_Armenian_GIM);
    module.set_const("KEY_Armenian_gim", (guint) mousetrap::KEY_Armenian_gim);
    module.set_const("KEY_Armenian_DA", (guint) mousetrap::KEY_Armenian_DA);
    module.set_const("KEY_Armenian_da", (guint) mousetrap::KEY_Armenian_da);
    module.set_const("KEY_Armenian_YECH", (guint) mousetrap::KEY_Armenian_YECH);
    module.set_const("KEY_Armenian_yech", (guint) mousetrap::KEY_Armenian_yech);
    module.set_const("KEY_Armenian_ZA", (guint) mousetrap::KEY_Armenian_ZA);
    module.set_const("KEY_Armenian_za", (guint) mousetrap::KEY_Armenian_za);
    module.set_const("KEY_Armenian_E", (guint) mousetrap::KEY_Armenian_E);
    module.set_const("KEY_Armenian_e", (guint) mousetrap::KEY_Armenian_e);
    module.set_const("KEY_Armenian_AT", (guint) mousetrap::KEY_Armenian_AT);
    module.set_const("KEY_Armenian_at", (guint) mousetrap::KEY_Armenian_at);
    module.set_const("KEY_Armenian_TO", (guint) mousetrap::KEY_Armenian_TO);
    module.set_const("KEY_Armenian_to", (guint) mousetrap::KEY_Armenian_to);
    module.set_const("KEY_Armenian_ZHE", (guint) mousetrap::KEY_Armenian_ZHE);
    module.set_const("KEY_Armenian_zhe", (guint) mousetrap::KEY_Armenian_zhe);
    module.set_const("KEY_Armenian_INI", (guint) mousetrap::KEY_Armenian_INI);
    module.set_const("KEY_Armenian_ini", (guint) mousetrap::KEY_Armenian_ini);
    module.set_const("KEY_Armenian_LYUN", (guint) mousetrap::KEY_Armenian_LYUN);
    module.set_const("KEY_Armenian_lyun", (guint) mousetrap::KEY_Armenian_lyun);
    module.set_const("KEY_Armenian_KHE", (guint) mousetrap::KEY_Armenian_KHE);
    module.set_const("KEY_Armenian_khe", (guint) mousetrap::KEY_Armenian_khe);
    module.set_const("KEY_Armenian_TSA", (guint) mousetrap::KEY_Armenian_TSA);
    module.set_const("KEY_Armenian_tsa", (guint) mousetrap::KEY_Armenian_tsa);
    module.set_const("KEY_Armenian_KEN", (guint) mousetrap::KEY_Armenian_KEN);
    module.set_const("KEY_Armenian_ken", (guint) mousetrap::KEY_Armenian_ken);
    module.set_const("KEY_Armenian_HO", (guint) mousetrap::KEY_Armenian_HO);
    module.set_const("KEY_Armenian_ho", (guint) mousetrap::KEY_Armenian_ho);
    module.set_const("KEY_Armenian_DZA", (guint) mousetrap::KEY_Armenian_DZA);
    module.set_const("KEY_Armenian_dza", (guint) mousetrap::KEY_Armenian_dza);
    module.set_const("KEY_Armenian_GHAT", (guint) mousetrap::KEY_Armenian_GHAT);
    module.set_const("KEY_Armenian_ghat", (guint) mousetrap::KEY_Armenian_ghat);
    module.set_const("KEY_Armenian_TCHE", (guint) mousetrap::KEY_Armenian_TCHE);
    module.set_const("KEY_Armenian_tche", (guint) mousetrap::KEY_Armenian_tche);
    module.set_const("KEY_Armenian_MEN", (guint) mousetrap::KEY_Armenian_MEN);
    module.set_const("KEY_Armenian_men", (guint) mousetrap::KEY_Armenian_men);
    module.set_const("KEY_Armenian_HI", (guint) mousetrap::KEY_Armenian_HI);
    module.set_const("KEY_Armenian_hi", (guint) mousetrap::KEY_Armenian_hi);
    module.set_const("KEY_Armenian_NU", (guint) mousetrap::KEY_Armenian_NU);
    module.set_const("KEY_Armenian_nu", (guint) mousetrap::KEY_Armenian_nu);
    module.set_const("KEY_Armenian_SHA", (guint) mousetrap::KEY_Armenian_SHA);
    module.set_const("KEY_Armenian_sha", (guint) mousetrap::KEY_Armenian_sha);
    module.set_const("KEY_Armenian_VO", (guint) mousetrap::KEY_Armenian_VO);
    module.set_const("KEY_Armenian_vo", (guint) mousetrap::KEY_Armenian_vo);
    module.set_const("KEY_Armenian_CHA", (guint) mousetrap::KEY_Armenian_CHA);
    module.set_const("KEY_Armenian_cha", (guint) mousetrap::KEY_Armenian_cha);
    module.set_const("KEY_Armenian_PE", (guint) mousetrap::KEY_Armenian_PE);
    module.set_const("KEY_Armenian_pe", (guint) mousetrap::KEY_Armenian_pe);
    module.set_const("KEY_Armenian_JE", (guint) mousetrap::KEY_Armenian_JE);
    module.set_const("KEY_Armenian_je", (guint) mousetrap::KEY_Armenian_je);
    module.set_const("KEY_Armenian_RA", (guint) mousetrap::KEY_Armenian_RA);
    module.set_const("KEY_Armenian_ra", (guint) mousetrap::KEY_Armenian_ra);
    module.set_const("KEY_Armenian_SE", (guint) mousetrap::KEY_Armenian_SE);
    module.set_const("KEY_Armenian_se", (guint) mousetrap::KEY_Armenian_se);
    module.set_const("KEY_Armenian_VEV", (guint) mousetrap::KEY_Armenian_VEV);
    module.set_const("KEY_Armenian_vev", (guint) mousetrap::KEY_Armenian_vev);
    module.set_const("KEY_Armenian_TYUN", (guint) mousetrap::KEY_Armenian_TYUN);
    module.set_const("KEY_Armenian_tyun", (guint) mousetrap::KEY_Armenian_tyun);
    module.set_const("KEY_Armenian_RE", (guint) mousetrap::KEY_Armenian_RE);
    module.set_const("KEY_Armenian_re", (guint) mousetrap::KEY_Armenian_re);
    module.set_const("KEY_Armenian_TSO", (guint) mousetrap::KEY_Armenian_TSO);
    module.set_const("KEY_Armenian_tso", (guint) mousetrap::KEY_Armenian_tso);
    module.set_const("KEY_Armenian_VYUN", (guint) mousetrap::KEY_Armenian_VYUN);
    module.set_const("KEY_Armenian_vyun", (guint) mousetrap::KEY_Armenian_vyun);
    module.set_const("KEY_Armenian_PYUR", (guint) mousetrap::KEY_Armenian_PYUR);
    module.set_const("KEY_Armenian_pyur", (guint) mousetrap::KEY_Armenian_pyur);
    module.set_const("KEY_Armenian_KE", (guint) mousetrap::KEY_Armenian_KE);
    module.set_const("KEY_Armenian_ke", (guint) mousetrap::KEY_Armenian_ke);
    module.set_const("KEY_Armenian_O", (guint) mousetrap::KEY_Armenian_O);
    module.set_const("KEY_Armenian_o", (guint) mousetrap::KEY_Armenian_o);
    module.set_const("KEY_Armenian_FE", (guint) mousetrap::KEY_Armenian_FE);
    module.set_const("KEY_Armenian_fe", (guint) mousetrap::KEY_Armenian_fe);
    module.set_const("KEY_Armenian_apostrophe", (guint) mousetrap::KEY_Armenian_apostrophe);
    module.set_const("KEY_Georgian_an", (guint) mousetrap::KEY_Georgian_an);
    module.set_const("KEY_Georgian_ban", (guint) mousetrap::KEY_Georgian_ban);
    module.set_const("KEY_Georgian_gan", (guint) mousetrap::KEY_Georgian_gan);
    module.set_const("KEY_Georgian_don", (guint) mousetrap::KEY_Georgian_don);
    module.set_const("KEY_Georgian_en", (guint) mousetrap::KEY_Georgian_en);
    module.set_const("KEY_Georgian_vin", (guint) mousetrap::KEY_Georgian_vin);
    module.set_const("KEY_Georgian_zen", (guint) mousetrap::KEY_Georgian_zen);
    module.set_const("KEY_Georgian_tan", (guint) mousetrap::KEY_Georgian_tan);
    module.set_const("KEY_Georgian_in", (guint) mousetrap::KEY_Georgian_in);
    module.set_const("KEY_Georgian_kan", (guint) mousetrap::KEY_Georgian_kan);
    module.set_const("KEY_Georgian_las", (guint) mousetrap::KEY_Georgian_las);
    module.set_const("KEY_Georgian_man", (guint) mousetrap::KEY_Georgian_man);
    module.set_const("KEY_Georgian_nar", (guint) mousetrap::KEY_Georgian_nar);
    module.set_const("KEY_Georgian_on", (guint) mousetrap::KEY_Georgian_on);
    module.set_const("KEY_Georgian_par", (guint) mousetrap::KEY_Georgian_par);
    module.set_const("KEY_Georgian_zhar", (guint) mousetrap::KEY_Georgian_zhar);
    module.set_const("KEY_Georgian_rae", (guint) mousetrap::KEY_Georgian_rae);
    module.set_const("KEY_Georgian_san", (guint) mousetrap::KEY_Georgian_san);
    module.set_const("KEY_Georgian_tar", (guint) mousetrap::KEY_Georgian_tar);
    module.set_const("KEY_Georgian_un", (guint) mousetrap::KEY_Georgian_un);
    module.set_const("KEY_Georgian_phar", (guint) mousetrap::KEY_Georgian_phar);
    module.set_const("KEY_Georgian_khar", (guint) mousetrap::KEY_Georgian_khar);
    module.set_const("KEY_Georgian_ghan", (guint) mousetrap::KEY_Georgian_ghan);
    module.set_const("KEY_Georgian_qar", (guint) mousetrap::KEY_Georgian_qar);
    module.set_const("KEY_Georgian_shin", (guint) mousetrap::KEY_Georgian_shin);
    module.set_const("KEY_Georgian_chin", (guint) mousetrap::KEY_Georgian_chin);
    module.set_const("KEY_Georgian_can", (guint) mousetrap::KEY_Georgian_can);
    module.set_const("KEY_Georgian_jil", (guint) mousetrap::KEY_Georgian_jil);
    module.set_const("KEY_Georgian_cil", (guint) mousetrap::KEY_Georgian_cil);
    module.set_const("KEY_Georgian_char", (guint) mousetrap::KEY_Georgian_char);
    module.set_const("KEY_Georgian_xan", (guint) mousetrap::KEY_Georgian_xan);
    module.set_const("KEY_Georgian_jhan", (guint) mousetrap::KEY_Georgian_jhan);
    module.set_const("KEY_Georgian_hae", (guint) mousetrap::KEY_Georgian_hae);
    module.set_const("KEY_Georgian_he", (guint) mousetrap::KEY_Georgian_he);
    module.set_const("KEY_Georgian_hie", (guint) mousetrap::KEY_Georgian_hie);
    module.set_const("KEY_Georgian_we", (guint) mousetrap::KEY_Georgian_we);
    module.set_const("KEY_Georgian_har", (guint) mousetrap::KEY_Georgian_har);
    module.set_const("KEY_Georgian_hoe", (guint) mousetrap::KEY_Georgian_hoe);
    module.set_const("KEY_Georgian_fi", (guint) mousetrap::KEY_Georgian_fi);
    module.set_const("KEY_Xabovedot", (guint) mousetrap::KEY_Xabovedot);
    module.set_const("KEY_Ibreve", (guint) mousetrap::KEY_Ibreve);
    module.set_const("KEY_Zstroke", (guint) mousetrap::KEY_Zstroke);
    module.set_const("KEY_Gcaron", (guint) mousetrap::KEY_Gcaron);
    module.set_const("KEY_Ocaron", (guint) mousetrap::KEY_Ocaron);
    module.set_const("KEY_Obarred", (guint) mousetrap::KEY_Obarred);
    module.set_const("KEY_xabovedot", (guint) mousetrap::KEY_xabovedot);
    module.set_const("KEY_ibreve", (guint) mousetrap::KEY_ibreve);
    module.set_const("KEY_zstroke", (guint) mousetrap::KEY_zstroke);
    module.set_const("KEY_gcaron", (guint) mousetrap::KEY_gcaron);
    module.set_const("KEY_ocaron", (guint) mousetrap::KEY_ocaron);
    module.set_const("KEY_obarred", (guint) mousetrap::KEY_obarred);
    module.set_const("KEY_SCHWA", (guint) mousetrap::KEY_SCHWA);
    module.set_const("KEY_schwa", (guint) mousetrap::KEY_schwa);
    module.set_const("KEY_EZH", (guint) mousetrap::KEY_EZH);
    module.set_const("KEY_ezh", (guint) mousetrap::KEY_ezh);
    module.set_const("KEY_Lbelowdot", (guint) mousetrap::KEY_Lbelowdot);
    module.set_const("KEY_lbelowdot", (guint) mousetrap::KEY_lbelowdot);
    module.set_const("KEY_Abelowdot", (guint) mousetrap::KEY_Abelowdot);
    module.set_const("KEY_abelowdot", (guint) mousetrap::KEY_abelowdot);
    module.set_const("KEY_Ahook", (guint) mousetrap::KEY_Ahook);
    module.set_const("KEY_ahook", (guint) mousetrap::KEY_ahook);
    module.set_const("KEY_Acircumflexacute", (guint) mousetrap::KEY_Acircumflexacute);
    module.set_const("KEY_acircumflexacute", (guint) mousetrap::KEY_acircumflexacute);
    module.set_const("KEY_Acircumflexgrave", (guint) mousetrap::KEY_Acircumflexgrave);
    module.set_const("KEY_acircumflexgrave", (guint) mousetrap::KEY_acircumflexgrave);
    module.set_const("KEY_Acircumflexhook", (guint) mousetrap::KEY_Acircumflexhook);
    module.set_const("KEY_acircumflexhook", (guint) mousetrap::KEY_acircumflexhook);
    module.set_const("KEY_Acircumflextilde", (guint) mousetrap::KEY_Acircumflextilde);
    module.set_const("KEY_acircumflextilde", (guint) mousetrap::KEY_acircumflextilde);
    module.set_const("KEY_Acircumflexbelowdot", (guint) mousetrap::KEY_Acircumflexbelowdot);
    module.set_const("KEY_acircumflexbelowdot", (guint) mousetrap::KEY_acircumflexbelowdot);
    module.set_const("KEY_Abreveacute", (guint) mousetrap::KEY_Abreveacute);
    module.set_const("KEY_abreveacute", (guint) mousetrap::KEY_abreveacute);
    module.set_const("KEY_Abrevegrave", (guint) mousetrap::KEY_Abrevegrave);
    module.set_const("KEY_abrevegrave", (guint) mousetrap::KEY_abrevegrave);
    module.set_const("KEY_Abrevehook", (guint) mousetrap::KEY_Abrevehook);
    module.set_const("KEY_abrevehook", (guint) mousetrap::KEY_abrevehook);
    module.set_const("KEY_Abrevetilde", (guint) mousetrap::KEY_Abrevetilde);
    module.set_const("KEY_abrevetilde", (guint) mousetrap::KEY_abrevetilde);
    module.set_const("KEY_Abrevebelowdot", (guint) mousetrap::KEY_Abrevebelowdot);
    module.set_const("KEY_abrevebelowdot", (guint) mousetrap::KEY_abrevebelowdot);
    module.set_const("KEY_Ebelowdot", (guint) mousetrap::KEY_Ebelowdot);
    module.set_const("KEY_ebelowdot", (guint) mousetrap::KEY_ebelowdot);
    module.set_const("KEY_Ehook", (guint) mousetrap::KEY_Ehook);
    module.set_const("KEY_ehook", (guint) mousetrap::KEY_ehook);
    module.set_const("KEY_Etilde", (guint) mousetrap::KEY_Etilde);
    module.set_const("KEY_etilde", (guint) mousetrap::KEY_etilde);
    module.set_const("KEY_Ecircumflexacute", (guint) mousetrap::KEY_Ecircumflexacute);
    module.set_const("KEY_ecircumflexacute", (guint) mousetrap::KEY_ecircumflexacute);
    module.set_const("KEY_Ecircumflexgrave", (guint) mousetrap::KEY_Ecircumflexgrave);
    module.set_const("KEY_ecircumflexgrave", (guint) mousetrap::KEY_ecircumflexgrave);
    module.set_const("KEY_Ecircumflexhook", (guint) mousetrap::KEY_Ecircumflexhook);
    module.set_const("KEY_ecircumflexhook", (guint) mousetrap::KEY_ecircumflexhook);
    module.set_const("KEY_Ecircumflextilde", (guint) mousetrap::KEY_Ecircumflextilde);
    module.set_const("KEY_ecircumflextilde", (guint) mousetrap::KEY_ecircumflextilde);
    module.set_const("KEY_Ecircumflexbelowdot", (guint) mousetrap::KEY_Ecircumflexbelowdot);
    module.set_const("KEY_ecircumflexbelowdot", (guint) mousetrap::KEY_ecircumflexbelowdot);
    module.set_const("KEY_Ihook", (guint) mousetrap::KEY_Ihook);
    module.set_const("KEY_ihook", (guint) mousetrap::KEY_ihook);
    module.set_const("KEY_Ibelowdot", (guint) mousetrap::KEY_Ibelowdot);
    module.set_const("KEY_ibelowdot", (guint) mousetrap::KEY_ibelowdot);
    module.set_const("KEY_Obelowdot", (guint) mousetrap::KEY_Obelowdot);
    module.set_const("KEY_obelowdot", (guint) mousetrap::KEY_obelowdot);
    module.set_const("KEY_Ohook", (guint) mousetrap::KEY_Ohook);
    module.set_const("KEY_ohook", (guint) mousetrap::KEY_ohook);
    module.set_const("KEY_Ocircumflexacute", (guint) mousetrap::KEY_Ocircumflexacute);
    module.set_const("KEY_ocircumflexacute", (guint) mousetrap::KEY_ocircumflexacute);
    module.set_const("KEY_Ocircumflexgrave", (guint) mousetrap::KEY_Ocircumflexgrave);
    module.set_const("KEY_ocircumflexgrave", (guint) mousetrap::KEY_ocircumflexgrave);
    module.set_const("KEY_Ocircumflexhook", (guint) mousetrap::KEY_Ocircumflexhook);
    module.set_const("KEY_ocircumflexhook", (guint) mousetrap::KEY_ocircumflexhook);
    module.set_const("KEY_Ocircumflextilde", (guint) mousetrap::KEY_Ocircumflextilde);
    module.set_const("KEY_ocircumflextilde", (guint) mousetrap::KEY_ocircumflextilde);
    module.set_const("KEY_Ocircumflexbelowdot", (guint) mousetrap::KEY_Ocircumflexbelowdot);
    module.set_const("KEY_ocircumflexbelowdot", (guint) mousetrap::KEY_ocircumflexbelowdot);
    module.set_const("KEY_Ohornacute", (guint) mousetrap::KEY_Ohornacute);
    module.set_const("KEY_ohornacute", (guint) mousetrap::KEY_ohornacute);
    module.set_const("KEY_Ohorngrave", (guint) mousetrap::KEY_Ohorngrave);
    module.set_const("KEY_ohorngrave", (guint) mousetrap::KEY_ohorngrave);
    module.set_const("KEY_Ohornhook", (guint) mousetrap::KEY_Ohornhook);
    module.set_const("KEY_ohornhook", (guint) mousetrap::KEY_ohornhook);
    module.set_const("KEY_Ohorntilde", (guint) mousetrap::KEY_Ohorntilde);
    module.set_const("KEY_ohorntilde", (guint) mousetrap::KEY_ohorntilde);
    module.set_const("KEY_Ohornbelowdot", (guint) mousetrap::KEY_Ohornbelowdot);
    module.set_const("KEY_ohornbelowdot", (guint) mousetrap::KEY_ohornbelowdot);
    module.set_const("KEY_Ubelowdot", (guint) mousetrap::KEY_Ubelowdot);
    module.set_const("KEY_ubelowdot", (guint) mousetrap::KEY_ubelowdot);
    module.set_const("KEY_Uhook", (guint) mousetrap::KEY_Uhook);
    module.set_const("KEY_uhook", (guint) mousetrap::KEY_uhook);
    module.set_const("KEY_Uhornacute", (guint) mousetrap::KEY_Uhornacute);
    module.set_const("KEY_uhornacute", (guint) mousetrap::KEY_uhornacute);
    module.set_const("KEY_Uhorngrave", (guint) mousetrap::KEY_Uhorngrave);
    module.set_const("KEY_uhorngrave", (guint) mousetrap::KEY_uhorngrave);
    module.set_const("KEY_Uhornhook", (guint) mousetrap::KEY_Uhornhook);
    module.set_const("KEY_uhornhook", (guint) mousetrap::KEY_uhornhook);
    module.set_const("KEY_Uhorntilde", (guint) mousetrap::KEY_Uhorntilde);
    module.set_const("KEY_uhorntilde", (guint) mousetrap::KEY_uhorntilde);
    module.set_const("KEY_Uhornbelowdot", (guint) mousetrap::KEY_Uhornbelowdot);
    module.set_const("KEY_uhornbelowdot", (guint) mousetrap::KEY_uhornbelowdot);
    module.set_const("KEY_Ybelowdot", (guint) mousetrap::KEY_Ybelowdot);
    module.set_const("KEY_ybelowdot", (guint) mousetrap::KEY_ybelowdot);
    module.set_const("KEY_Yhook", (guint) mousetrap::KEY_Yhook);
    module.set_const("KEY_yhook", (guint) mousetrap::KEY_yhook);
    module.set_const("KEY_Ytilde", (guint) mousetrap::KEY_Ytilde);
    module.set_const("KEY_ytilde", (guint) mousetrap::KEY_ytilde);
    module.set_const("KEY_Ohorn", (guint) mousetrap::KEY_Ohorn);
    module.set_const("KEY_ohorn", (guint) mousetrap::KEY_ohorn);
    module.set_const("KEY_Uhorn", (guint) mousetrap::KEY_Uhorn);
    module.set_const("KEY_uhorn", (guint) mousetrap::KEY_uhorn);
    module.set_const("KEY_EcuSign", (guint) mousetrap::KEY_EcuSign);
    module.set_const("KEY_ColonSign", (guint) mousetrap::KEY_ColonSign);
    module.set_const("KEY_CruzeiroSign", (guint) mousetrap::KEY_CruzeiroSign);
    module.set_const("KEY_FFrancSign", (guint) mousetrap::KEY_FFrancSign);
    module.set_const("KEY_LiraSign", (guint) mousetrap::KEY_LiraSign);
    module.set_const("KEY_MillSign", (guint) mousetrap::KEY_MillSign);
    module.set_const("KEY_NairaSign", (guint) mousetrap::KEY_NairaSign);
    module.set_const("KEY_PesetaSign", (guint) mousetrap::KEY_PesetaSign);
    module.set_const("KEY_RupeeSign", (guint) mousetrap::KEY_RupeeSign);
    module.set_const("KEY_WonSign", (guint) mousetrap::KEY_WonSign);
    module.set_const("KEY_NewSheqelSign", (guint) mousetrap::KEY_NewSheqelSign);
    module.set_const("KEY_DongSign", (guint) mousetrap::KEY_DongSign);
    module.set_const("KEY_EuroSign", (guint) mousetrap::KEY_EuroSign);
    module.set_const("KEY_zerosuperior", (guint) mousetrap::KEY_zerosuperior);
    module.set_const("KEY_foursuperior", (guint) mousetrap::KEY_foursuperior);
    module.set_const("KEY_fivesuperior", (guint) mousetrap::KEY_fivesuperior);
    module.set_const("KEY_sixsuperior", (guint) mousetrap::KEY_sixsuperior);
    module.set_const("KEY_sevensuperior", (guint) mousetrap::KEY_sevensuperior);
    module.set_const("KEY_eightsuperior", (guint) mousetrap::KEY_eightsuperior);
    module.set_const("KEY_ninesuperior", (guint) mousetrap::KEY_ninesuperior);
    module.set_const("KEY_zerosubscript", (guint) mousetrap::KEY_zerosubscript);
    module.set_const("KEY_onesubscript", (guint) mousetrap::KEY_onesubscript);
    module.set_const("KEY_twosubscript", (guint) mousetrap::KEY_twosubscript);
    module.set_const("KEY_threesubscript", (guint) mousetrap::KEY_threesubscript);
    module.set_const("KEY_foursubscript", (guint) mousetrap::KEY_foursubscript);
    module.set_const("KEY_fivesubscript", (guint) mousetrap::KEY_fivesubscript);
    module.set_const("KEY_sixsubscript", (guint) mousetrap::KEY_sixsubscript);
    module.set_const("KEY_sevensubscript", (guint) mousetrap::KEY_sevensubscript);
    module.set_const("KEY_eightsubscript", (guint) mousetrap::KEY_eightsubscript);
    module.set_const("KEY_ninesubscript", (guint) mousetrap::KEY_ninesubscript);
    module.set_const("KEY_partdifferential", (guint) mousetrap::KEY_partdifferential);
    module.set_const("KEY_emptyset", (guint) mousetrap::KEY_emptyset);
    module.set_const("KEY_elementof", (guint) mousetrap::KEY_elementof);
    module.set_const("KEY_notelementof", (guint) mousetrap::KEY_notelementof);
    module.set_const("KEY_containsas", (guint) mousetrap::KEY_containsas);
    module.set_const("KEY_squareroot", (guint) mousetrap::KEY_squareroot);
    module.set_const("KEY_cuberoot", (guint) mousetrap::KEY_cuberoot);
    module.set_const("KEY_fourthroot", (guint) mousetrap::KEY_fourthroot);
    module.set_const("KEY_dintegral", (guint) mousetrap::KEY_dintegral);
    module.set_const("KEY_tintegral", (guint) mousetrap::KEY_tintegral);
    module.set_const("KEY_because", (guint) mousetrap::KEY_because);
    module.set_const("KEY_approxeq", (guint) mousetrap::KEY_approxeq);
    module.set_const("KEY_notapproxeq", (guint) mousetrap::KEY_notapproxeq);
    module.set_const("KEY_notidentical", (guint) mousetrap::KEY_notidentical);
    module.set_const("KEY_stricteq", (guint) mousetrap::KEY_stricteq);
    module.set_const("KEY_braille_dot_1", (guint) mousetrap::KEY_braille_dot_1);
    module.set_const("KEY_braille_dot_2", (guint) mousetrap::KEY_braille_dot_2);
    module.set_const("KEY_braille_dot_3", (guint) mousetrap::KEY_braille_dot_3);
    module.set_const("KEY_braille_dot_4", (guint) mousetrap::KEY_braille_dot_4);
    module.set_const("KEY_braille_dot_5", (guint) mousetrap::KEY_braille_dot_5);
    module.set_const("KEY_braille_dot_6", (guint) mousetrap::KEY_braille_dot_6);
    module.set_const("KEY_braille_dot_7", (guint) mousetrap::KEY_braille_dot_7);
    module.set_const("KEY_braille_dot_8", (guint) mousetrap::KEY_braille_dot_8);
    module.set_const("KEY_braille_dot_9", (guint) mousetrap::KEY_braille_dot_9);
    module.set_const("KEY_braille_dot_10", (guint) mousetrap::KEY_braille_dot_10);
    module.set_const("KEY_braille_blank", (guint) mousetrap::KEY_braille_blank);
    module.set_const("KEY_braille_dots_1", (guint) mousetrap::KEY_braille_dots_1);
    module.set_const("KEY_braille_dots_2", (guint) mousetrap::KEY_braille_dots_2);
    module.set_const("KEY_braille_dots_12", (guint) mousetrap::KEY_braille_dots_12);
    module.set_const("KEY_braille_dots_3", (guint) mousetrap::KEY_braille_dots_3);
    module.set_const("KEY_braille_dots_13", (guint) mousetrap::KEY_braille_dots_13);
    module.set_const("KEY_braille_dots_23", (guint) mousetrap::KEY_braille_dots_23);
    module.set_const("KEY_braille_dots_123", (guint) mousetrap::KEY_braille_dots_123);
    module.set_const("KEY_braille_dots_4", (guint) mousetrap::KEY_braille_dots_4);
    module.set_const("KEY_braille_dots_14", (guint) mousetrap::KEY_braille_dots_14);
    module.set_const("KEY_braille_dots_24", (guint) mousetrap::KEY_braille_dots_24);
    module.set_const("KEY_braille_dots_124", (guint) mousetrap::KEY_braille_dots_124);
    module.set_const("KEY_braille_dots_34", (guint) mousetrap::KEY_braille_dots_34);
    module.set_const("KEY_braille_dots_134", (guint) mousetrap::KEY_braille_dots_134);
    module.set_const("KEY_braille_dots_234", (guint) mousetrap::KEY_braille_dots_234);
    module.set_const("KEY_braille_dots_1234", (guint) mousetrap::KEY_braille_dots_1234);
    module.set_const("KEY_braille_dots_5", (guint) mousetrap::KEY_braille_dots_5);
    module.set_const("KEY_braille_dots_15", (guint) mousetrap::KEY_braille_dots_15);
    module.set_const("KEY_braille_dots_25", (guint) mousetrap::KEY_braille_dots_25);
    module.set_const("KEY_braille_dots_125", (guint) mousetrap::KEY_braille_dots_125);
    module.set_const("KEY_braille_dots_35", (guint) mousetrap::KEY_braille_dots_35);
    module.set_const("KEY_braille_dots_135", (guint) mousetrap::KEY_braille_dots_135);
    module.set_const("KEY_braille_dots_235", (guint) mousetrap::KEY_braille_dots_235);
    module.set_const("KEY_braille_dots_1235", (guint) mousetrap::KEY_braille_dots_1235);
    module.set_const("KEY_braille_dots_45", (guint) mousetrap::KEY_braille_dots_45);
    module.set_const("KEY_braille_dots_145", (guint) mousetrap::KEY_braille_dots_145);
    module.set_const("KEY_braille_dots_245", (guint) mousetrap::KEY_braille_dots_245);
    module.set_const("KEY_braille_dots_1245", (guint) mousetrap::KEY_braille_dots_1245);
    module.set_const("KEY_braille_dots_345", (guint) mousetrap::KEY_braille_dots_345);
    module.set_const("KEY_braille_dots_1345", (guint) mousetrap::KEY_braille_dots_1345);
    module.set_const("KEY_braille_dots_2345", (guint) mousetrap::KEY_braille_dots_2345);
    module.set_const("KEY_braille_dots_12345", (guint) mousetrap::KEY_braille_dots_12345);
    module.set_const("KEY_braille_dots_6", (guint) mousetrap::KEY_braille_dots_6);
    module.set_const("KEY_braille_dots_16", (guint) mousetrap::KEY_braille_dots_16);
    module.set_const("KEY_braille_dots_26", (guint) mousetrap::KEY_braille_dots_26);
    module.set_const("KEY_braille_dots_126", (guint) mousetrap::KEY_braille_dots_126);
    module.set_const("KEY_braille_dots_36", (guint) mousetrap::KEY_braille_dots_36);
    module.set_const("KEY_braille_dots_136", (guint) mousetrap::KEY_braille_dots_136);
    module.set_const("KEY_braille_dots_236", (guint) mousetrap::KEY_braille_dots_236);
    module.set_const("KEY_braille_dots_1236", (guint) mousetrap::KEY_braille_dots_1236);
    module.set_const("KEY_braille_dots_46", (guint) mousetrap::KEY_braille_dots_46);
    module.set_const("KEY_braille_dots_146", (guint) mousetrap::KEY_braille_dots_146);
    module.set_const("KEY_braille_dots_246", (guint) mousetrap::KEY_braille_dots_246);
    module.set_const("KEY_braille_dots_1246", (guint) mousetrap::KEY_braille_dots_1246);
    module.set_const("KEY_braille_dots_346", (guint) mousetrap::KEY_braille_dots_346);
    module.set_const("KEY_braille_dots_1346", (guint) mousetrap::KEY_braille_dots_1346);
    module.set_const("KEY_braille_dots_2346", (guint) mousetrap::KEY_braille_dots_2346);
    module.set_const("KEY_braille_dots_12346", (guint) mousetrap::KEY_braille_dots_12346);
    module.set_const("KEY_braille_dots_56", (guint) mousetrap::KEY_braille_dots_56);
    module.set_const("KEY_braille_dots_156", (guint) mousetrap::KEY_braille_dots_156);
    module.set_const("KEY_braille_dots_256", (guint) mousetrap::KEY_braille_dots_256);
    module.set_const("KEY_braille_dots_1256", (guint) mousetrap::KEY_braille_dots_1256);
    module.set_const("KEY_braille_dots_356", (guint) mousetrap::KEY_braille_dots_356);
    module.set_const("KEY_braille_dots_1356", (guint) mousetrap::KEY_braille_dots_1356);
    module.set_const("KEY_braille_dots_2356", (guint) mousetrap::KEY_braille_dots_2356);
    module.set_const("KEY_braille_dots_12356", (guint) mousetrap::KEY_braille_dots_12356);
    module.set_const("KEY_braille_dots_456", (guint) mousetrap::KEY_braille_dots_456);
    module.set_const("KEY_braille_dots_1456", (guint) mousetrap::KEY_braille_dots_1456);
    module.set_const("KEY_braille_dots_2456", (guint) mousetrap::KEY_braille_dots_2456);
    module.set_const("KEY_braille_dots_12456", (guint) mousetrap::KEY_braille_dots_12456);
    module.set_const("KEY_braille_dots_3456", (guint) mousetrap::KEY_braille_dots_3456);
    module.set_const("KEY_braille_dots_13456", (guint) mousetrap::KEY_braille_dots_13456);
    module.set_const("KEY_braille_dots_23456", (guint) mousetrap::KEY_braille_dots_23456);
    module.set_const("KEY_braille_dots_123456", (guint) mousetrap::KEY_braille_dots_123456);
    module.set_const("KEY_braille_dots_7", (guint) mousetrap::KEY_braille_dots_7);
    module.set_const("KEY_braille_dots_17", (guint) mousetrap::KEY_braille_dots_17);
    module.set_const("KEY_braille_dots_27", (guint) mousetrap::KEY_braille_dots_27);
    module.set_const("KEY_braille_dots_127", (guint) mousetrap::KEY_braille_dots_127);
    module.set_const("KEY_braille_dots_37", (guint) mousetrap::KEY_braille_dots_37);
    module.set_const("KEY_braille_dots_137", (guint) mousetrap::KEY_braille_dots_137);
    module.set_const("KEY_braille_dots_237", (guint) mousetrap::KEY_braille_dots_237);
    module.set_const("KEY_braille_dots_1237", (guint) mousetrap::KEY_braille_dots_1237);
    module.set_const("KEY_braille_dots_47", (guint) mousetrap::KEY_braille_dots_47);
    module.set_const("KEY_braille_dots_147", (guint) mousetrap::KEY_braille_dots_147);
    module.set_const("KEY_braille_dots_247", (guint) mousetrap::KEY_braille_dots_247);
    module.set_const("KEY_braille_dots_1247", (guint) mousetrap::KEY_braille_dots_1247);
    module.set_const("KEY_braille_dots_347", (guint) mousetrap::KEY_braille_dots_347);
    module.set_const("KEY_braille_dots_1347", (guint) mousetrap::KEY_braille_dots_1347);
    module.set_const("KEY_braille_dots_2347", (guint) mousetrap::KEY_braille_dots_2347);
    module.set_const("KEY_braille_dots_12347", (guint) mousetrap::KEY_braille_dots_12347);
    module.set_const("KEY_braille_dots_57", (guint) mousetrap::KEY_braille_dots_57);
    module.set_const("KEY_braille_dots_157", (guint) mousetrap::KEY_braille_dots_157);
    module.set_const("KEY_braille_dots_257", (guint) mousetrap::KEY_braille_dots_257);
    module.set_const("KEY_braille_dots_1257", (guint) mousetrap::KEY_braille_dots_1257);
    module.set_const("KEY_braille_dots_357", (guint) mousetrap::KEY_braille_dots_357);
    module.set_const("KEY_braille_dots_1357", (guint) mousetrap::KEY_braille_dots_1357);
    module.set_const("KEY_braille_dots_2357", (guint) mousetrap::KEY_braille_dots_2357);
    module.set_const("KEY_braille_dots_12357", (guint) mousetrap::KEY_braille_dots_12357);
    module.set_const("KEY_braille_dots_457", (guint) mousetrap::KEY_braille_dots_457);
    module.set_const("KEY_braille_dots_1457", (guint) mousetrap::KEY_braille_dots_1457);
    module.set_const("KEY_braille_dots_2457", (guint) mousetrap::KEY_braille_dots_2457);
    module.set_const("KEY_braille_dots_12457", (guint) mousetrap::KEY_braille_dots_12457);
    module.set_const("KEY_braille_dots_3457", (guint) mousetrap::KEY_braille_dots_3457);
    module.set_const("KEY_braille_dots_13457", (guint) mousetrap::KEY_braille_dots_13457);
    module.set_const("KEY_braille_dots_23457", (guint) mousetrap::KEY_braille_dots_23457);
    module.set_const("KEY_braille_dots_123457", (guint) mousetrap::KEY_braille_dots_123457);
    module.set_const("KEY_braille_dots_67", (guint) mousetrap::KEY_braille_dots_67);
    module.set_const("KEY_braille_dots_167", (guint) mousetrap::KEY_braille_dots_167);
    module.set_const("KEY_braille_dots_267", (guint) mousetrap::KEY_braille_dots_267);
    module.set_const("KEY_braille_dots_1267", (guint) mousetrap::KEY_braille_dots_1267);
    module.set_const("KEY_braille_dots_367", (guint) mousetrap::KEY_braille_dots_367);
    module.set_const("KEY_braille_dots_1367", (guint) mousetrap::KEY_braille_dots_1367);
    module.set_const("KEY_braille_dots_2367", (guint) mousetrap::KEY_braille_dots_2367);
    module.set_const("KEY_braille_dots_12367", (guint) mousetrap::KEY_braille_dots_12367);
    module.set_const("KEY_braille_dots_467", (guint) mousetrap::KEY_braille_dots_467);
    module.set_const("KEY_braille_dots_1467", (guint) mousetrap::KEY_braille_dots_1467);
    module.set_const("KEY_braille_dots_2467", (guint) mousetrap::KEY_braille_dots_2467);
    module.set_const("KEY_braille_dots_12467", (guint) mousetrap::KEY_braille_dots_12467);
    module.set_const("KEY_braille_dots_3467", (guint) mousetrap::KEY_braille_dots_3467);
    module.set_const("KEY_braille_dots_13467", (guint) mousetrap::KEY_braille_dots_13467);
    module.set_const("KEY_braille_dots_23467", (guint) mousetrap::KEY_braille_dots_23467);
    module.set_const("KEY_braille_dots_123467", (guint) mousetrap::KEY_braille_dots_123467);
    module.set_const("KEY_braille_dots_567", (guint) mousetrap::KEY_braille_dots_567);
    module.set_const("KEY_braille_dots_1567", (guint) mousetrap::KEY_braille_dots_1567);
    module.set_const("KEY_braille_dots_2567", (guint) mousetrap::KEY_braille_dots_2567);
    module.set_const("KEY_braille_dots_12567", (guint) mousetrap::KEY_braille_dots_12567);
    module.set_const("KEY_braille_dots_3567", (guint) mousetrap::KEY_braille_dots_3567);
    module.set_const("KEY_braille_dots_13567", (guint) mousetrap::KEY_braille_dots_13567);
    module.set_const("KEY_braille_dots_23567", (guint) mousetrap::KEY_braille_dots_23567);
    module.set_const("KEY_braille_dots_123567", (guint) mousetrap::KEY_braille_dots_123567);
    module.set_const("KEY_braille_dots_4567", (guint) mousetrap::KEY_braille_dots_4567);
    module.set_const("KEY_braille_dots_14567", (guint) mousetrap::KEY_braille_dots_14567);
    module.set_const("KEY_braille_dots_24567", (guint) mousetrap::KEY_braille_dots_24567);
    module.set_const("KEY_braille_dots_124567", (guint) mousetrap::KEY_braille_dots_124567);
    module.set_const("KEY_braille_dots_34567", (guint) mousetrap::KEY_braille_dots_34567);
    module.set_const("KEY_braille_dots_134567", (guint) mousetrap::KEY_braille_dots_134567);
    module.set_const("KEY_braille_dots_234567", (guint) mousetrap::KEY_braille_dots_234567);
    module.set_const("KEY_braille_dots_1234567", (guint) mousetrap::KEY_braille_dots_1234567);
    module.set_const("KEY_braille_dots_8", (guint) mousetrap::KEY_braille_dots_8);
    module.set_const("KEY_braille_dots_18", (guint) mousetrap::KEY_braille_dots_18);
    module.set_const("KEY_braille_dots_28", (guint) mousetrap::KEY_braille_dots_28);
    module.set_const("KEY_braille_dots_128", (guint) mousetrap::KEY_braille_dots_128);
    module.set_const("KEY_braille_dots_38", (guint) mousetrap::KEY_braille_dots_38);
    module.set_const("KEY_braille_dots_138", (guint) mousetrap::KEY_braille_dots_138);
    module.set_const("KEY_braille_dots_238", (guint) mousetrap::KEY_braille_dots_238);
    module.set_const("KEY_braille_dots_1238", (guint) mousetrap::KEY_braille_dots_1238);
    module.set_const("KEY_braille_dots_48", (guint) mousetrap::KEY_braille_dots_48);
    module.set_const("KEY_braille_dots_148", (guint) mousetrap::KEY_braille_dots_148);
    module.set_const("KEY_braille_dots_248", (guint) mousetrap::KEY_braille_dots_248);
    module.set_const("KEY_braille_dots_1248", (guint) mousetrap::KEY_braille_dots_1248);
    module.set_const("KEY_braille_dots_348", (guint) mousetrap::KEY_braille_dots_348);
    module.set_const("KEY_braille_dots_1348", (guint) mousetrap::KEY_braille_dots_1348);
    module.set_const("KEY_braille_dots_2348", (guint) mousetrap::KEY_braille_dots_2348);
    module.set_const("KEY_braille_dots_12348", (guint) mousetrap::KEY_braille_dots_12348);
    module.set_const("KEY_braille_dots_58", (guint) mousetrap::KEY_braille_dots_58);
    module.set_const("KEY_braille_dots_158", (guint) mousetrap::KEY_braille_dots_158);
    module.set_const("KEY_braille_dots_258", (guint) mousetrap::KEY_braille_dots_258);
    module.set_const("KEY_braille_dots_1258", (guint) mousetrap::KEY_braille_dots_1258);
    module.set_const("KEY_braille_dots_358", (guint) mousetrap::KEY_braille_dots_358);
    module.set_const("KEY_braille_dots_1358", (guint) mousetrap::KEY_braille_dots_1358);
    module.set_const("KEY_braille_dots_2358", (guint) mousetrap::KEY_braille_dots_2358);
    module.set_const("KEY_braille_dots_12358", (guint) mousetrap::KEY_braille_dots_12358);
    module.set_const("KEY_braille_dots_458", (guint) mousetrap::KEY_braille_dots_458);
    module.set_const("KEY_braille_dots_1458", (guint) mousetrap::KEY_braille_dots_1458);
    module.set_const("KEY_braille_dots_2458", (guint) mousetrap::KEY_braille_dots_2458);
    module.set_const("KEY_braille_dots_12458", (guint) mousetrap::KEY_braille_dots_12458);
    module.set_const("KEY_braille_dots_3458", (guint) mousetrap::KEY_braille_dots_3458);
    module.set_const("KEY_braille_dots_13458", (guint) mousetrap::KEY_braille_dots_13458);
    module.set_const("KEY_braille_dots_23458", (guint) mousetrap::KEY_braille_dots_23458);
    module.set_const("KEY_braille_dots_123458", (guint) mousetrap::KEY_braille_dots_123458);
    module.set_const("KEY_braille_dots_68", (guint) mousetrap::KEY_braille_dots_68);
    module.set_const("KEY_braille_dots_168", (guint) mousetrap::KEY_braille_dots_168);
    module.set_const("KEY_braille_dots_268", (guint) mousetrap::KEY_braille_dots_268);
    module.set_const("KEY_braille_dots_1268", (guint) mousetrap::KEY_braille_dots_1268);
    module.set_const("KEY_braille_dots_368", (guint) mousetrap::KEY_braille_dots_368);
    module.set_const("KEY_braille_dots_1368", (guint) mousetrap::KEY_braille_dots_1368);
    module.set_const("KEY_braille_dots_2368", (guint) mousetrap::KEY_braille_dots_2368);
    module.set_const("KEY_braille_dots_12368", (guint) mousetrap::KEY_braille_dots_12368);
    module.set_const("KEY_braille_dots_468", (guint) mousetrap::KEY_braille_dots_468);
    module.set_const("KEY_braille_dots_1468", (guint) mousetrap::KEY_braille_dots_1468);
    module.set_const("KEY_braille_dots_2468", (guint) mousetrap::KEY_braille_dots_2468);
    module.set_const("KEY_braille_dots_12468", (guint) mousetrap::KEY_braille_dots_12468);
    module.set_const("KEY_braille_dots_3468", (guint) mousetrap::KEY_braille_dots_3468);
    module.set_const("KEY_braille_dots_13468", (guint) mousetrap::KEY_braille_dots_13468);
    module.set_const("KEY_braille_dots_23468", (guint) mousetrap::KEY_braille_dots_23468);
    module.set_const("KEY_braille_dots_123468", (guint) mousetrap::KEY_braille_dots_123468);
    module.set_const("KEY_braille_dots_568", (guint) mousetrap::KEY_braille_dots_568);
    module.set_const("KEY_braille_dots_1568", (guint) mousetrap::KEY_braille_dots_1568);
    module.set_const("KEY_braille_dots_2568", (guint) mousetrap::KEY_braille_dots_2568);
    module.set_const("KEY_braille_dots_12568", (guint) mousetrap::KEY_braille_dots_12568);
    module.set_const("KEY_braille_dots_3568", (guint) mousetrap::KEY_braille_dots_3568);
    module.set_const("KEY_braille_dots_13568", (guint) mousetrap::KEY_braille_dots_13568);
    module.set_const("KEY_braille_dots_23568", (guint) mousetrap::KEY_braille_dots_23568);
    module.set_const("KEY_braille_dots_123568", (guint) mousetrap::KEY_braille_dots_123568);
    module.set_const("KEY_braille_dots_4568", (guint) mousetrap::KEY_braille_dots_4568);
    module.set_const("KEY_braille_dots_14568", (guint) mousetrap::KEY_braille_dots_14568);
    module.set_const("KEY_braille_dots_24568", (guint) mousetrap::KEY_braille_dots_24568);
    module.set_const("KEY_braille_dots_124568", (guint) mousetrap::KEY_braille_dots_124568);
    module.set_const("KEY_braille_dots_34568", (guint) mousetrap::KEY_braille_dots_34568);
    module.set_const("KEY_braille_dots_134568", (guint) mousetrap::KEY_braille_dots_134568);
    module.set_const("KEY_braille_dots_234568", (guint) mousetrap::KEY_braille_dots_234568);
    module.set_const("KEY_braille_dots_1234568", (guint) mousetrap::KEY_braille_dots_1234568);
    module.set_const("KEY_braille_dots_78", (guint) mousetrap::KEY_braille_dots_78);
    module.set_const("KEY_braille_dots_178", (guint) mousetrap::KEY_braille_dots_178);
    module.set_const("KEY_braille_dots_278", (guint) mousetrap::KEY_braille_dots_278);
    module.set_const("KEY_braille_dots_1278", (guint) mousetrap::KEY_braille_dots_1278);
    module.set_const("KEY_braille_dots_378", (guint) mousetrap::KEY_braille_dots_378);
    module.set_const("KEY_braille_dots_1378", (guint) mousetrap::KEY_braille_dots_1378);
    module.set_const("KEY_braille_dots_2378", (guint) mousetrap::KEY_braille_dots_2378);
    module.set_const("KEY_braille_dots_12378", (guint) mousetrap::KEY_braille_dots_12378);
    module.set_const("KEY_braille_dots_478", (guint) mousetrap::KEY_braille_dots_478);
    module.set_const("KEY_braille_dots_1478", (guint) mousetrap::KEY_braille_dots_1478);
    module.set_const("KEY_braille_dots_2478", (guint) mousetrap::KEY_braille_dots_2478);
    module.set_const("KEY_braille_dots_12478", (guint) mousetrap::KEY_braille_dots_12478);
    module.set_const("KEY_braille_dots_3478", (guint) mousetrap::KEY_braille_dots_3478);
    module.set_const("KEY_braille_dots_13478", (guint) mousetrap::KEY_braille_dots_13478);
    module.set_const("KEY_braille_dots_23478", (guint) mousetrap::KEY_braille_dots_23478);
    module.set_const("KEY_braille_dots_123478", (guint) mousetrap::KEY_braille_dots_123478);
    module.set_const("KEY_braille_dots_578", (guint) mousetrap::KEY_braille_dots_578);
    module.set_const("KEY_braille_dots_1578", (guint) mousetrap::KEY_braille_dots_1578);
    module.set_const("KEY_braille_dots_2578", (guint) mousetrap::KEY_braille_dots_2578);
    module.set_const("KEY_braille_dots_12578", (guint) mousetrap::KEY_braille_dots_12578);
    module.set_const("KEY_braille_dots_3578", (guint) mousetrap::KEY_braille_dots_3578);
    module.set_const("KEY_braille_dots_13578", (guint) mousetrap::KEY_braille_dots_13578);
    module.set_const("KEY_braille_dots_23578", (guint) mousetrap::KEY_braille_dots_23578);
    module.set_const("KEY_braille_dots_123578", (guint) mousetrap::KEY_braille_dots_123578);
    module.set_const("KEY_braille_dots_4578", (guint) mousetrap::KEY_braille_dots_4578);
    module.set_const("KEY_braille_dots_14578", (guint) mousetrap::KEY_braille_dots_14578);
    module.set_const("KEY_braille_dots_24578", (guint) mousetrap::KEY_braille_dots_24578);
    module.set_const("KEY_braille_dots_124578", (guint) mousetrap::KEY_braille_dots_124578);
    module.set_const("KEY_braille_dots_34578", (guint) mousetrap::KEY_braille_dots_34578);
    module.set_const("KEY_braille_dots_134578", (guint) mousetrap::KEY_braille_dots_134578);
    module.set_const("KEY_braille_dots_234578", (guint) mousetrap::KEY_braille_dots_234578);
    module.set_const("KEY_braille_dots_1234578", (guint) mousetrap::KEY_braille_dots_1234578);
    module.set_const("KEY_braille_dots_678", (guint) mousetrap::KEY_braille_dots_678);
    module.set_const("KEY_braille_dots_1678", (guint) mousetrap::KEY_braille_dots_1678);
    module.set_const("KEY_braille_dots_2678", (guint) mousetrap::KEY_braille_dots_2678);
    module.set_const("KEY_braille_dots_12678", (guint) mousetrap::KEY_braille_dots_12678);
    module.set_const("KEY_braille_dots_3678", (guint) mousetrap::KEY_braille_dots_3678);
    module.set_const("KEY_braille_dots_13678", (guint) mousetrap::KEY_braille_dots_13678);
    module.set_const("KEY_braille_dots_23678", (guint) mousetrap::KEY_braille_dots_23678);
    module.set_const("KEY_braille_dots_123678", (guint) mousetrap::KEY_braille_dots_123678);
    module.set_const("KEY_braille_dots_4678", (guint) mousetrap::KEY_braille_dots_4678);
    module.set_const("KEY_braille_dots_14678", (guint) mousetrap::KEY_braille_dots_14678);
    module.set_const("KEY_braille_dots_24678", (guint) mousetrap::KEY_braille_dots_24678);
    module.set_const("KEY_braille_dots_124678", (guint) mousetrap::KEY_braille_dots_124678);
    module.set_const("KEY_braille_dots_34678", (guint) mousetrap::KEY_braille_dots_34678);
    module.set_const("KEY_braille_dots_134678", (guint) mousetrap::KEY_braille_dots_134678);
    module.set_const("KEY_braille_dots_234678", (guint) mousetrap::KEY_braille_dots_234678);
    module.set_const("KEY_braille_dots_1234678", (guint) mousetrap::KEY_braille_dots_1234678);
    module.set_const("KEY_braille_dots_5678", (guint) mousetrap::KEY_braille_dots_5678);
    module.set_const("KEY_braille_dots_15678", (guint) mousetrap::KEY_braille_dots_15678);
    module.set_const("KEY_braille_dots_25678", (guint) mousetrap::KEY_braille_dots_25678);
    module.set_const("KEY_braille_dots_125678", (guint) mousetrap::KEY_braille_dots_125678);
    module.set_const("KEY_braille_dots_35678", (guint) mousetrap::KEY_braille_dots_35678);
    module.set_const("KEY_braille_dots_135678", (guint) mousetrap::KEY_braille_dots_135678);
    module.set_const("KEY_braille_dots_235678", (guint) mousetrap::KEY_braille_dots_235678);
    module.set_const("KEY_braille_dots_1235678", (guint) mousetrap::KEY_braille_dots_1235678);
    module.set_const("KEY_braille_dots_45678", (guint) mousetrap::KEY_braille_dots_45678);
    module.set_const("KEY_braille_dots_145678", (guint) mousetrap::KEY_braille_dots_145678);
    module.set_const("KEY_braille_dots_245678", (guint) mousetrap::KEY_braille_dots_245678);
    module.set_const("KEY_braille_dots_1245678", (guint) mousetrap::KEY_braille_dots_1245678);
    module.set_const("KEY_braille_dots_345678", (guint) mousetrap::KEY_braille_dots_345678);
    module.set_const("KEY_braille_dots_1345678", (guint) mousetrap::KEY_braille_dots_1345678);
    module.set_const("KEY_braille_dots_2345678", (guint) mousetrap::KEY_braille_dots_2345678);
    module.set_const("KEY_braille_dots_12345678", (guint) mousetrap::KEY_braille_dots_12345678);
    module.set_const("KEY_Sinh_ng", (guint) mousetrap::KEY_Sinh_ng);
    module.set_const("KEY_Sinh_h2", (guint) mousetrap::KEY_Sinh_h2);
    module.set_const("KEY_Sinh_a", (guint) mousetrap::KEY_Sinh_a);
    module.set_const("KEY_Sinh_aa", (guint) mousetrap::KEY_Sinh_aa);
    module.set_const("KEY_Sinh_ae", (guint) mousetrap::KEY_Sinh_ae);
    module.set_const("KEY_Sinh_aee", (guint) mousetrap::KEY_Sinh_aee);
    module.set_const("KEY_Sinh_i", (guint) mousetrap::KEY_Sinh_i);
    module.set_const("KEY_Sinh_ii", (guint) mousetrap::KEY_Sinh_ii);
    module.set_const("KEY_Sinh_u", (guint) mousetrap::KEY_Sinh_u);
    module.set_const("KEY_Sinh_uu", (guint) mousetrap::KEY_Sinh_uu);
    module.set_const("KEY_Sinh_ri", (guint) mousetrap::KEY_Sinh_ri);
    module.set_const("KEY_Sinh_rii", (guint) mousetrap::KEY_Sinh_rii);
    module.set_const("KEY_Sinh_lu", (guint) mousetrap::KEY_Sinh_lu);
    module.set_const("KEY_Sinh_luu", (guint) mousetrap::KEY_Sinh_luu);
    module.set_const("KEY_Sinh_e", (guint) mousetrap::KEY_Sinh_e);
    module.set_const("KEY_Sinh_ee", (guint) mousetrap::KEY_Sinh_ee);
    module.set_const("KEY_Sinh_ai", (guint) mousetrap::KEY_Sinh_ai);
    module.set_const("KEY_Sinh_o", (guint) mousetrap::KEY_Sinh_o);
    module.set_const("KEY_Sinh_oo", (guint) mousetrap::KEY_Sinh_oo);
    module.set_const("KEY_Sinh_au", (guint) mousetrap::KEY_Sinh_au);
    module.set_const("KEY_Sinh_ka", (guint) mousetrap::KEY_Sinh_ka);
    module.set_const("KEY_Sinh_kha", (guint) mousetrap::KEY_Sinh_kha);
    module.set_const("KEY_Sinh_ga", (guint) mousetrap::KEY_Sinh_ga);
    module.set_const("KEY_Sinh_gha", (guint) mousetrap::KEY_Sinh_gha);
    module.set_const("KEY_Sinh_ng2", (guint) mousetrap::KEY_Sinh_ng2);
    module.set_const("KEY_Sinh_nga", (guint) mousetrap::KEY_Sinh_nga);
    module.set_const("KEY_Sinh_ca", (guint) mousetrap::KEY_Sinh_ca);
    module.set_const("KEY_Sinh_cha", (guint) mousetrap::KEY_Sinh_cha);
    module.set_const("KEY_Sinh_ja", (guint) mousetrap::KEY_Sinh_ja);
    module.set_const("KEY_Sinh_jha", (guint) mousetrap::KEY_Sinh_jha);
    module.set_const("KEY_Sinh_nya", (guint) mousetrap::KEY_Sinh_nya);
    module.set_const("KEY_Sinh_jnya", (guint) mousetrap::KEY_Sinh_jnya);
    module.set_const("KEY_Sinh_nja", (guint) mousetrap::KEY_Sinh_nja);
    module.set_const("KEY_Sinh_tta", (guint) mousetrap::KEY_Sinh_tta);
    module.set_const("KEY_Sinh_ttha", (guint) mousetrap::KEY_Sinh_ttha);
    module.set_const("KEY_Sinh_dda", (guint) mousetrap::KEY_Sinh_dda);
    module.set_const("KEY_Sinh_ddha", (guint) mousetrap::KEY_Sinh_ddha);
    module.set_const("KEY_Sinh_nna", (guint) mousetrap::KEY_Sinh_nna);
    module.set_const("KEY_Sinh_ndda", (guint) mousetrap::KEY_Sinh_ndda);
    module.set_const("KEY_Sinh_tha", (guint) mousetrap::KEY_Sinh_tha);
    module.set_const("KEY_Sinh_thha", (guint) mousetrap::KEY_Sinh_thha);
    module.set_const("KEY_Sinh_dha", (guint) mousetrap::KEY_Sinh_dha);
    module.set_const("KEY_Sinh_dhha", (guint) mousetrap::KEY_Sinh_dhha);
    module.set_const("KEY_Sinh_na", (guint) mousetrap::KEY_Sinh_na);
    module.set_const("KEY_Sinh_ndha", (guint) mousetrap::KEY_Sinh_ndha);
    module.set_const("KEY_Sinh_pa", (guint) mousetrap::KEY_Sinh_pa);
    module.set_const("KEY_Sinh_pha", (guint) mousetrap::KEY_Sinh_pha);
    module.set_const("KEY_Sinh_ba", (guint) mousetrap::KEY_Sinh_ba);
    module.set_const("KEY_Sinh_bha", (guint) mousetrap::KEY_Sinh_bha);
    module.set_const("KEY_Sinh_ma", (guint) mousetrap::KEY_Sinh_ma);
    module.set_const("KEY_Sinh_mba", (guint) mousetrap::KEY_Sinh_mba);
    module.set_const("KEY_Sinh_ya", (guint) mousetrap::KEY_Sinh_ya);
    module.set_const("KEY_Sinh_ra", (guint) mousetrap::KEY_Sinh_ra);
    module.set_const("KEY_Sinh_la", (guint) mousetrap::KEY_Sinh_la);
    module.set_const("KEY_Sinh_va", (guint) mousetrap::KEY_Sinh_va);
    module.set_const("KEY_Sinh_sha", (guint) mousetrap::KEY_Sinh_sha);
    module.set_const("KEY_Sinh_ssha", (guint) mousetrap::KEY_Sinh_ssha);
    module.set_const("KEY_Sinh_sa", (guint) mousetrap::KEY_Sinh_sa);
    module.set_const("KEY_Sinh_ha", (guint) mousetrap::KEY_Sinh_ha);
    module.set_const("KEY_Sinh_lla", (guint) mousetrap::KEY_Sinh_lla);
    module.set_const("KEY_Sinh_fa", (guint) mousetrap::KEY_Sinh_fa);
    module.set_const("KEY_Sinh_al", (guint) mousetrap::KEY_Sinh_al);
    module.set_const("KEY_Sinh_aa2", (guint) mousetrap::KEY_Sinh_aa2);
    module.set_const("KEY_Sinh_ae2", (guint) mousetrap::KEY_Sinh_ae2);
    module.set_const("KEY_Sinh_aee2", (guint) mousetrap::KEY_Sinh_aee2);
    module.set_const("KEY_Sinh_i2", (guint) mousetrap::KEY_Sinh_i2);
    module.set_const("KEY_Sinh_ii2", (guint) mousetrap::KEY_Sinh_ii2);
    module.set_const("KEY_Sinh_u2", (guint) mousetrap::KEY_Sinh_u2);
    module.set_const("KEY_Sinh_uu2", (guint) mousetrap::KEY_Sinh_uu2);
    module.set_const("KEY_Sinh_ru2", (guint) mousetrap::KEY_Sinh_ru2);
    module.set_const("KEY_Sinh_e2", (guint) mousetrap::KEY_Sinh_e2);
    module.set_const("KEY_Sinh_ee2", (guint) mousetrap::KEY_Sinh_ee2);
    module.set_const("KEY_Sinh_ai2", (guint) mousetrap::KEY_Sinh_ai2);
    module.set_const("KEY_Sinh_o2", (guint) mousetrap::KEY_Sinh_o2);
    module.set_const("KEY_Sinh_oo2", (guint) mousetrap::KEY_Sinh_oo2);
    module.set_const("KEY_Sinh_au2", (guint) mousetrap::KEY_Sinh_au2);
    module.set_const("KEY_Sinh_lu2", (guint) mousetrap::KEY_Sinh_lu2);
    module.set_const("KEY_Sinh_ruu2", (guint) mousetrap::KEY_Sinh_ruu2);
    module.set_const("KEY_Sinh_luu2", (guint) mousetrap::KEY_Sinh_luu2);
    module.set_const("KEY_Sinh_kunddaliya", (guint) mousetrap::KEY_Sinh_kunddaliya);
    module.set_const("KEY_ModeLock", (guint) mousetrap::KEY_ModeLock);
    module.set_const("KEY_MonBrightnessUp", (guint) mousetrap::KEY_MonBrightnessUp);
    module.set_const("KEY_MonBrightnessDown", (guint) mousetrap::KEY_MonBrightnessDown);
    module.set_const("KEY_KbdLightOnOff", (guint) mousetrap::KEY_KbdLightOnOff);
    module.set_const("KEY_KbdBrightnessUp", (guint) mousetrap::KEY_KbdBrightnessUp);
    module.set_const("KEY_KbdBrightnessDown", (guint) mousetrap::KEY_KbdBrightnessDown);
    module.set_const("KEY_Standby", (guint) mousetrap::KEY_Standby);
    module.set_const("KEY_AudioLowerVolume", (guint) mousetrap::KEY_AudioLowerVolume);
    module.set_const("KEY_AudioMute", (guint) mousetrap::KEY_AudioMute);
    module.set_const("KEY_AudioRaiseVolume", (guint) mousetrap::KEY_AudioRaiseVolume);
    module.set_const("KEY_AudioPlay", (guint) mousetrap::KEY_AudioPlay);
    module.set_const("KEY_AudioStop", (guint) mousetrap::KEY_AudioStop);
    module.set_const("KEY_AudioPrev", (guint) mousetrap::KEY_AudioPrev);
    module.set_const("KEY_AudioNext", (guint) mousetrap::KEY_AudioNext);
    module.set_const("KEY_HomePage", (guint) mousetrap::KEY_HomePage);
    module.set_const("KEY_Mail", (guint) mousetrap::KEY_Mail);
    module.set_const("KEY_Start", (guint) mousetrap::KEY_Start);
    module.set_const("KEY_Search", (guint) mousetrap::KEY_Search);
    module.set_const("KEY_AudioRecord", (guint) mousetrap::KEY_AudioRecord);
    module.set_const("KEY_Calculator", (guint) mousetrap::KEY_Calculator);
    module.set_const("KEY_Memo", (guint) mousetrap::KEY_Memo);
    module.set_const("KEY_ToDoList", (guint) mousetrap::KEY_ToDoList);
    module.set_const("KEY_Calendar", (guint) mousetrap::KEY_Calendar);
    module.set_const("KEY_PowerDown", (guint) mousetrap::KEY_PowerDown);
    module.set_const("KEY_ContrastAdjust", (guint) mousetrap::KEY_ContrastAdjust);
    module.set_const("KEY_RockerUp", (guint) mousetrap::KEY_RockerUp);
    module.set_const("KEY_RockerDown", (guint) mousetrap::KEY_RockerDown);
    module.set_const("KEY_RockerEnter", (guint) mousetrap::KEY_RockerEnter);
    module.set_const("KEY_Back", (guint) mousetrap::KEY_Back);
    module.set_const("KEY_Forward", (guint) mousetrap::KEY_Forward);
    module.set_const("KEY_Stop", (guint) mousetrap::KEY_Stop);
    module.set_const("KEY_Refresh", (guint) mousetrap::KEY_Refresh);
    module.set_const("KEY_PowerOff", (guint) mousetrap::KEY_PowerOff);
    module.set_const("KEY_WakeUp", (guint) mousetrap::KEY_WakeUp);
    module.set_const("KEY_Eject", (guint) mousetrap::KEY_Eject);
    module.set_const("KEY_ScreenSaver", (guint) mousetrap::KEY_ScreenSaver);
    module.set_const("KEY_WWW", (guint) mousetrap::KEY_WWW);
    module.set_const("KEY_Sleep", (guint) mousetrap::KEY_Sleep);
    module.set_const("KEY_Favorites", (guint) mousetrap::KEY_Favorites);
    module.set_const("KEY_AudioPause", (guint) mousetrap::KEY_AudioPause);
    module.set_const("KEY_AudioMedia", (guint) mousetrap::KEY_AudioMedia);
    module.set_const("KEY_MyComputer", (guint) mousetrap::KEY_MyComputer);
    module.set_const("KEY_VendorHome", (guint) mousetrap::KEY_VendorHome);
    module.set_const("KEY_LightBulb", (guint) mousetrap::KEY_LightBulb);
    module.set_const("KEY_Shop", (guint) mousetrap::KEY_Shop);
    module.set_const("KEY_History", (guint) mousetrap::KEY_History);
    module.set_const("KEY_OpenURL", (guint) mousetrap::KEY_OpenURL);
    module.set_const("KEY_AddFavorite", (guint) mousetrap::KEY_AddFavorite);
    module.set_const("KEY_HotLinks", (guint) mousetrap::KEY_HotLinks);
    module.set_const("KEY_BrightnessAdjust", (guint) mousetrap::KEY_BrightnessAdjust);
    module.set_const("KEY_Finance", (guint) mousetrap::KEY_Finance);
    module.set_const("KEY_Community", (guint) mousetrap::KEY_Community);
    module.set_const("KEY_AudioRewind", (guint) mousetrap::KEY_AudioRewind);
    module.set_const("KEY_BackForward", (guint) mousetrap::KEY_BackForward);
    module.set_const("KEY_Launch0", (guint) mousetrap::KEY_Launch0);
    module.set_const("KEY_Launch1", (guint) mousetrap::KEY_Launch1);
    module.set_const("KEY_Launch2", (guint) mousetrap::KEY_Launch2);
    module.set_const("KEY_Launch3", (guint) mousetrap::KEY_Launch3);
    module.set_const("KEY_Launch4", (guint) mousetrap::KEY_Launch4);
    module.set_const("KEY_Launch5", (guint) mousetrap::KEY_Launch5);
    module.set_const("KEY_Launch6", (guint) mousetrap::KEY_Launch6);
    module.set_const("KEY_Launch7", (guint) mousetrap::KEY_Launch7);
    module.set_const("KEY_Launch8", (guint) mousetrap::KEY_Launch8);
    module.set_const("KEY_Launch9", (guint) mousetrap::KEY_Launch9);
    module.set_const("KEY_LaunchA", (guint) mousetrap::KEY_LaunchA);
    module.set_const("KEY_LaunchB", (guint) mousetrap::KEY_LaunchB);
    module.set_const("KEY_LaunchC", (guint) mousetrap::KEY_LaunchC);
    module.set_const("KEY_LaunchD", (guint) mousetrap::KEY_LaunchD);
    module.set_const("KEY_LaunchE", (guint) mousetrap::KEY_LaunchE);
    module.set_const("KEY_LaunchF", (guint) mousetrap::KEY_LaunchF);
    module.set_const("KEY_ApplicationLeft", (guint) mousetrap::KEY_ApplicationLeft);
    module.set_const("KEY_ApplicationRight", (guint) mousetrap::KEY_ApplicationRight);
    module.set_const("KEY_Book", (guint) mousetrap::KEY_Book);
    module.set_const("KEY_CD", (guint) mousetrap::KEY_CD);
    module.set_const("KEY_WindowClear", (guint) mousetrap::KEY_WindowClear);
    module.set_const("KEY_Close", (guint) mousetrap::KEY_Close);
    module.set_const("KEY_Copy", (guint) mousetrap::KEY_Copy);
    module.set_const("KEY_Cut", (guint) mousetrap::KEY_Cut);
    module.set_const("KEY_Display", (guint) mousetrap::KEY_Display);
    module.set_const("KEY_DOS", (guint) mousetrap::KEY_DOS);
    module.set_const("KEY_Documents", (guint) mousetrap::KEY_Documents);
    module.set_const("KEY_Excel", (guint) mousetrap::KEY_Excel);
    module.set_const("KEY_Explorer", (guint) mousetrap::KEY_Explorer);
    module.set_const("KEY_Game", (guint) mousetrap::KEY_Game);
    module.set_const("KEY_Go", (guint) mousetrap::KEY_Go);
    module.set_const("KEY_iTouch", (guint) mousetrap::KEY_iTouch);
    module.set_const("KEY_LogOff", (guint) mousetrap::KEY_LogOff);
    module.set_const("KEY_Market", (guint) mousetrap::KEY_Market);
    module.set_const("KEY_Meeting", (guint) mousetrap::KEY_Meeting);
    module.set_const("KEY_MenuKB", (guint) mousetrap::KEY_MenuKB);
    module.set_const("KEY_MenuPB", (guint) mousetrap::KEY_MenuPB);
    module.set_const("KEY_MySites", (guint) mousetrap::KEY_MySites);
    module.set_const("KEY_New", (guint) mousetrap::KEY_New);
    module.set_const("KEY_News", (guint) mousetrap::KEY_News);
    module.set_const("KEY_OfficeHome", (guint) mousetrap::KEY_OfficeHome);
    module.set_const("KEY_Open", (guint) mousetrap::KEY_Open);
    module.set_const("KEY_Option", (guint) mousetrap::KEY_Option);
    module.set_const("KEY_Paste", (guint) mousetrap::KEY_Paste);
    module.set_const("KEY_Phone", (guint) mousetrap::KEY_Phone);
    module.set_const("KEY_Reply", (guint) mousetrap::KEY_Reply);
    module.set_const("KEY_Reload", (guint) mousetrap::KEY_Reload);
    module.set_const("KEY_RotateWindows", (guint) mousetrap::KEY_RotateWindows);
    module.set_const("KEY_RotationPB", (guint) mousetrap::KEY_RotationPB);
    module.set_const("KEY_RotationKB", (guint) mousetrap::KEY_RotationKB);
    module.set_const("KEY_Save", (guint) mousetrap::KEY_Save);
    module.set_const("KEY_ScrollUp", (guint) mousetrap::KEY_ScrollUp);
    module.set_const("KEY_ScrollDown", (guint) mousetrap::KEY_ScrollDown);
    module.set_const("KEY_ScrollClick", (guint) mousetrap::KEY_ScrollClick);
    module.set_const("KEY_Send", (guint) mousetrap::KEY_Send);
    module.set_const("KEY_Spell", (guint) mousetrap::KEY_Spell);
    module.set_const("KEY_SplitScreen", (guint) mousetrap::KEY_SplitScreen);
    module.set_const("KEY_Support", (guint) mousetrap::KEY_Support);
    module.set_const("KEY_TaskPane", (guint) mousetrap::KEY_TaskPane);
    module.set_const("KEY_Terminal", (guint) mousetrap::KEY_Terminal);
    module.set_const("KEY_Tools", (guint) mousetrap::KEY_Tools);
    module.set_const("KEY_Travel", (guint) mousetrap::KEY_Travel);
    module.set_const("KEY_UserPB", (guint) mousetrap::KEY_UserPB);
    module.set_const("KEY_User1KB", (guint) mousetrap::KEY_User1KB);
    module.set_const("KEY_User2KB", (guint) mousetrap::KEY_User2KB);
    module.set_const("KEY_Video", (guint) mousetrap::KEY_Video);
    module.set_const("KEY_WheelButton", (guint) mousetrap::KEY_WheelButton);
    module.set_const("KEY_Word", (guint) mousetrap::KEY_Word);
    module.set_const("KEY_Xfer", (guint) mousetrap::KEY_Xfer);
    module.set_const("KEY_ZoomIn", (guint) mousetrap::KEY_ZoomIn);
    module.set_const("KEY_ZoomOut", (guint) mousetrap::KEY_ZoomOut);
    module.set_const("KEY_Away", (guint) mousetrap::KEY_Away);
    module.set_const("KEY_Messenger", (guint) mousetrap::KEY_Messenger);
    module.set_const("KEY_WebCam", (guint) mousetrap::KEY_WebCam);
    module.set_const("KEY_MailForward", (guint) mousetrap::KEY_MailForward);
    module.set_const("KEY_Pictures", (guint) mousetrap::KEY_Pictures);
    module.set_const("KEY_Music", (guint) mousetrap::KEY_Music);
    module.set_const("KEY_Battery", (guint) mousetrap::KEY_Battery);
    module.set_const("KEY_Bluetooth", (guint) mousetrap::KEY_Bluetooth);
    module.set_const("KEY_WLAN", (guint) mousetrap::KEY_WLAN);
    module.set_const("KEY_UWB", (guint) mousetrap::KEY_UWB);
    module.set_const("KEY_AudioForward", (guint) mousetrap::KEY_AudioForward);
    module.set_const("KEY_AudioRepeat", (guint) mousetrap::KEY_AudioRepeat);
    module.set_const("KEY_AudioRandomPlay", (guint) mousetrap::KEY_AudioRandomPlay);
    module.set_const("KEY_Subtitle", (guint) mousetrap::KEY_Subtitle);
    module.set_const("KEY_AudioCycleTrack", (guint) mousetrap::KEY_AudioCycleTrack);
    module.set_const("KEY_CycleAngle", (guint) mousetrap::KEY_CycleAngle);
    module.set_const("KEY_FrameBack", (guint) mousetrap::KEY_FrameBack);
    module.set_const("KEY_FrameForward", (guint) mousetrap::KEY_FrameForward);
    module.set_const("KEY_Time", (guint) mousetrap::KEY_Time);
    module.set_const("KEY_SelectButton", (guint) mousetrap::KEY_SelectButton);
    module.set_const("KEY_View", (guint) mousetrap::KEY_View);
    module.set_const("KEY_TopMenu", (guint) mousetrap::KEY_TopMenu);
    module.set_const("KEY_Red", (guint) mousetrap::KEY_Red);
    module.set_const("KEY_Green", (guint) mousetrap::KEY_Green);
    module.set_const("KEY_Yellow", (guint) mousetrap::KEY_Yellow);
    module.set_const("KEY_Blue", (guint) mousetrap::KEY_Blue);
    module.set_const("KEY_Suspend", (guint) mousetrap::KEY_Suspend);
    module.set_const("KEY_Hibernate", (guint) mousetrap::KEY_Hibernate);
    module.set_const("KEY_TouchpadToggle", (guint) mousetrap::KEY_TouchpadToggle);
    module.set_const("KEY_TouchpadOn", (guint) mousetrap::KEY_TouchpadOn);
    module.set_const("KEY_TouchpadOff", (guint) mousetrap::KEY_TouchpadOff);
    module.set_const("KEY_AudioMicMute", (guint) mousetrap::KEY_AudioMicMute);
    module.set_const("KEY_Keyboard", (guint) mousetrap::KEY_Keyboard);
    module.set_const("KEY_WWAN", (guint) mousetrap::KEY_WWAN);
    module.set_const("KEY_RFKill", (guint) mousetrap::KEY_RFKill);
    module.set_const("KEY_AudioPreset", (guint) mousetrap::KEY_AudioPreset);
    module.set_const("KEY_Switch_VT_1", (guint) mousetrap::KEY_Switch_VT_1);
    module.set_const("KEY_Switch_VT_2", (guint) mousetrap::KEY_Switch_VT_2);
    module.set_const("KEY_Switch_VT_3", (guint) mousetrap::KEY_Switch_VT_3);
    module.set_const("KEY_Switch_VT_4", (guint) mousetrap::KEY_Switch_VT_4);
    module.set_const("KEY_Switch_VT_5", (guint) mousetrap::KEY_Switch_VT_5);
    module.set_const("KEY_Switch_VT_6", (guint) mousetrap::KEY_Switch_VT_6);
    module.set_const("KEY_Switch_VT_7", (guint) mousetrap::KEY_Switch_VT_7);
    module.set_const("KEY_Switch_VT_8", (guint) mousetrap::KEY_Switch_VT_8);
    module.set_const("KEY_Switch_VT_9", (guint) mousetrap::KEY_Switch_VT_9);
    module.set_const("KEY_Switch_VT_10", (guint) mousetrap::KEY_Switch_VT_10);
    module.set_const("KEY_Switch_VT_11", (guint) mousetrap::KEY_Switch_VT_11);
    module.set_const("KEY_Switch_VT_12", (guint) mousetrap::KEY_Switch_VT_12);
    module.set_const("KEY_Ungrab", (guint) mousetrap::KEY_Ungrab);
    module.set_const("KEY_ClearGrab", (guint) mousetrap::KEY_ClearGrab);
    module.set_const("KEY_Next_VMode", (guint) mousetrap::KEY_Next_VMode);
    module.set_const("KEY_Prev_VMode", (guint) mousetrap::KEY_Prev_VMode);
    module.set_const("KEY_LogWindowTree", (guint) mousetrap::KEY_LogWindowTree);
    module.set_const("KEY_LogGrabInfo", (guint) mousetrap::KEY_LogGrabInfo);
}
