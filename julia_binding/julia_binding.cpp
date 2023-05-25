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

// ### CXX WRAP COMMON

#define add_type(Type) add_type<Type>(std::string("_") + #Type)
#define add_type_method(Type, id) method(#id, &Type::id)
#define add_constructor(...) constructor<__VA_ARGS__>(false)
#define add_constructor_with_finalizer(arg_type) constructor<arg_type>(false)

#define declare_is_subtype_of(A, B) template<> struct jlcxx::SuperType<A> { typedef B type; };
#define make_not_mirrored(Name) template<> struct jlcxx::IsMirroredType<Name> : std::false_type {};
#define add_enum(Name) add_bits<Name>(#Name, jlcxx::julia_type("CppEnum"))
#define add_enum_value(EnumName, PREFIX, VALUE_NAME) set_const(std::string(#PREFIX) + "_" + std::string(#VALUE_NAME),  EnumName::VALUE_NAME)

// ### SIGNAL COMPONENTS

#define DEFINE_ADD_SIGNAL(snake_case, return_t) \
template<typename T, typename Arg_t>                               \
void add_signal_##snake_case(Arg_t type) {\
type.method("connect_signal_" + std::string(#snake_case), [](T& instance, jl_function_t* task) \
    { \
        instance.connect_signal_##snake_case([](T& instance, jl_function_t* task) { \
            return jlcxx::unbox<return_t>(jl_safe_call(("emit_signal_" + std::string(#snake_case)).c_str(), task, jlcxx::box<T&>(instance))); \
        }, task); \
    }) \
    .method("disconnect_signal_" + std::string(#snake_case), [](T& instance) { \
        instance.disconnect_signal_##snake_case(); \
    }) \
    .method("set_signal_" + std::string(#snake_case) + "_blocked", [](T& instance, bool b){ \
        instance.set_signal_##snake_case##_blocked(b); \
    }) \
    .method("get_signal_" + std::string(#snake_case) + "_blocked", [](T& instance) -> bool { \
        return instance.get_signal_##snake_case##_blocked(); \
    }) \
    .method("emit_signal_" + std::string(#snake_case), [](T& instance) { \
        return instance.emit_signal_##snake_case(); \
    }); \
}

DEFINE_ADD_SIGNAL(activate, void)
DEFINE_ADD_SIGNAL(shutdown, void)

//DEFINE_ADD_SIGNAL_MANUAL(activated, void)

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
        .add_constructor(const std::string&, Application&)
        .add_type_method(Action, get_id)
        .add_type_method(Action, set_state)
        .add_type_method(Action, get_state)
        .add_type_method(Action, activate)
        .add_type_method(Action, add_shortcut)
        .add_type_method(Action, get_shortcuts)
        .add_type_method(Action, clear_shortcuts)
        .add_type_method(Action, set_enabled)
        .add_type_method(Action, get_enabled)
        .add_type_method(Action, get_is_stateful);

    action.method("set_function", [](Action& action, jl_function_t* task) {
        action.set_function([](Action& action, jl_function_t* task){
            jl_safe_call("Action::activate", task, jlcxx::box<Action&>(action));
        }, task);
    });

    action.method("set_stateful_function", [](Action& action, jl_function_t* task, bool initial_state) {
        action.set_stateful_function([](Action& action, bool state, jl_function_t* task) -> bool {
            return jl_unbox_bool(jl_safe_call("Action::activate", task, jlcxx::box<Action&>(action)));
        }, task);
    });

    // TODO add_signal_activated<Action>(action);
}

// ### MAIN

JLCXX_MODULE define_julia_module(jlcxx::Module& module)
{
    implement_application(module);
    implement_action(module);

    module.method("test_initialize", [](Application& app){
        auto window = Window(app);
        auto label = Label("test");
        label.set_margin(75);
        window.set_child(label);
        window.present();
    });
}
