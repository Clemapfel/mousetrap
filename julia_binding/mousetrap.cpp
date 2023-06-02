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

#define add_type(Type) add_type<Type>(std::string("_") + #Type)
#define add_type_method(Type, id) method(#id, &Type::id)
#define add_constructor(...) constructor<__VA_ARGS__>(USE_FINALIZERS)
#define add_enum(Enum) add_bits<Enum>(#Enum, jl_int32_type)
#define add_enum_value(Enum, PREFIX, VALUE) set_const(std::string(#PREFIX) + "_" + std::string(#VALUE), Enum::VALUE)

#define declare_is_subtype_of(A, B) template<> struct jlcxx::SuperType<A> { typedef B type; };
#define make_not_mirrored(Name) template<> struct jlcxx::IsMirroredType<Name> : std::false_type {};

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

#define DEFINE_ADD_WIDGET_SIGNAL(snake_case) \
template<typename T, typename Arg_t>                               \
void add_signal_##snake_case(Arg_t type) {\
    type.method("connect_signal_" + std::string(#snake_case), [](T& instance, jl_function_t* task) \
    { \
        instance.connect_signal_##snake_case([](Widget& instance, jl_function_t* task) -> void { \
            jl_safe_call(("emit_signal_" + std::string(#snake_case)).c_str(), task, jlcxx::box<T&>(dynamic_cast<T&>(instance))); \
        }, task); \
    }) \
    .method("emit_signal_" + std::string(#snake_case), [](Widget& instance) { \
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

// ### ACTION

// TODO

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

// ### MAIN



JLCXX_MODULE define_julia_module(jlcxx::Module& module)
{
    module.set_const("GTK_MAJOR_VERSION", (int) GTK_MAJOR_VERSION);
    module.set_const("GTK_MINOR_VERSION", (int) GTK_MINOR_VERSION);

}