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

// ### TODO

static void implement_click_event_controller(jlcxx::Module& module) {}

// ### TODO

static void implement_clipboard(jlcxx::Module& module) {}

// ### TODO

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

// ### TODO

static void implement_drag_event_controller(jlcxx::Module& module) {}

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

// ### TODO

static void implement_event_controller(jlcxx::Module& module) {}

// ### TODO

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

// ### TODO

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

// ### TODO

static void implement_focus_event_controller(jlcxx::Module& module) {}

// ### TODO

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

// ### TODO

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

// ### TODO

static void implement_key_event_controller(jlcxx::Module& module) {}

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

// ### TODO

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

// ### TODO

static void implement_log(jlcxx::Module& module) {}

// ### TODO

static void implement_long_press_event_controller(jlcxx::Module& module) {}

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

// ### TODO

static void implement_motion_event_controller(jlcxx::Module& module) {}

// ### TODO

static void implement_msaa_render_texture(jlcxx::Module& module) {}

// ### TODO

static void implement_music(jlcxx::Module& module) {}

// ### TODO

static void implement_notebook(jlcxx::Module& module) {}

// ### TODO

static void implement_orientation(jlcxx::Module& module)
{
    define_enum_in(module, Orientation);
    module.add_enum_value(Orientation, ORIENTATION, HORIZONTAL);
    module.add_enum_value(Orientation, ORIENTATION, VERTICAL);
}

// ### TODO

static void implement_overlay(jlcxx::Module& module) {}

// ### TODO

static void implement_pan_event_controller(jlcxx::Module& module) {}

// ### TODO

static void implement_paned(jlcxx::Module& module) {}

// ### TODO

static void implement_pinch_zoom_event_controller(jlcxx::Module& module) {}

// ### TODO

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

// ### TODO

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

// ### TODO

static void implement_rotate_event_controller(jlcxx::Module& module) {}

// ### TODO

static void implement_scale(jlcxx::Module& module) {}

// ### TODO

static void implement_scale_mode(jlcxx::Module& module) {}

// ### TODO

static void implement_scroll_event_controller(jlcxx::Module& module) {}

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

// ### TODO

static void implement_shortcut_event_controller(jlcxx::Module& module) {}

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

// ### TODO

static void implement_stylus_event_controller(jlcxx::Module& module) {}

// ### TODO

static void implement_swipe_event_controller(jlcxx::Module& module) {}

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
    implement_window(module);
    implement_aspect_frame(module);
    implement_blend_mode(module);
    implement_orientation(module);
    implement_box(module);
    implement_button(module);
    implement_center_box(module);
    implement_check_button(module);
    implement_click_event_controller(module);
    implement_clipboard(module);
    implement_color(module);
    implement_icon(module);
    implement_image(module);
    implement_image_display(module);
    implement_column_view(module);
    implement_cursor_type(module);
    implement_drag_event_controller(module);
    implement_drop_down(module);
    implement_entry(module);
    implement_event_controller(module);
    implement_expander(module);
    implement_file_chooser(module);
    implement_file_descriptor(module);
    implement_file_monitor(module);
    implement_file_system(module);
    implement_fixed(module);
    implement_focus_event_controller(module);
    implement_frame(module);
    implement_frame_clock(module);
    implement_geometry(module);
    implement_gl_common(module);
    implement_gl_transform(module);
    implement_grid(module);
    implement_grid_view(module);
    implement_header_bar(module);
    implement_key_event_controller(module);
    implement_key_file(module);

    implement_justify_mode(module);
    implement_label(module);
    implement_text_view(module);

    implement_level_bar(module);
    implement_list_view(module);
    implement_log(module);
    implement_long_press_event_controller(module);
    implement_motion_event_controller(module);
    implement_msaa_render_texture(module);
    implement_music(module);
    implement_notebook(module);
    implement_overlay(module);
    implement_pan_event_controller(module);
    implement_paned(module);
    implement_pinch_zoom_event_controller(module);

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
    implement_rotate_event_controller(module);
    implement_scale(module);
    implement_scale_mode(module);
    implement_scroll_event_controller(module);
    implement_scrollbar(module);
    implement_selection_model(module);
    implement_separator(module);
    implement_shader(module);
    implement_shape(module);
    implement_shortcut_event_controller(module);
    implement_sound(module);
    implement_sound_buffer(module);
    implement_spin_button(module);
    implement_spinner(module);
    implement_stack(module);
    implement_stylus_event_controller(module);
    implement_swipe_event_controller(module);
    implement_switch(module);
    implement_texture(module);
    implement_toggle_button(module);
    implement_transition_type(module);
    implement_viewport(module);
    implement_widget(module);
}