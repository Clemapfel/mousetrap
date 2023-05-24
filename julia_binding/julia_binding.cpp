//
// Created by clem on 4/16/23.
//

#include <mousetrap.hpp>

#include <jlcxx/jlcxx.hpp>
#include "julia_interface.cpp"

using namespace mousetrap;

/// @brief log domain for julia-specific mousetrap log messages
static inline const char* JULIA_DOMAIN = "mousetrap_jl";

/// @brief print last julia exception as mousetrap log entry, does not cause runtime to end
/// @param domain_name name of the current function, will be used for the error message
static void forward_last_exception(const std::string& domain_name)
{
    auto* exception_maybe = jl_exception_occurred();
    if (exception_maybe)
    {
        static auto* exception_to_string = jl_eval_string(R"(
            (exception) -> string(typeof(exception)) * ": " * exception.msg
        )");

        auto* exception = jl_calln(exception_to_string, exception_maybe);
        if (exception)
        {
            auto* str = jl_string_ptr(exception);
            log::critical("In " + domain_name + ": " + (str ? std::string(str) : "ERROR"), JULIA_DOMAIN);
        }
    }
}

static void jl_println(jl_value_t* value)
{
    static auto* println = jl_get_function(jl_base_module, "println");
    jl_call1(println, value);
}

#define add_type(Type) add_type<Type>(std::string("_") + #Type)
#define add_type_method(Type, id) method(#id, &Type::id)
#define add_constructor(arg_type) constructor<arg_type>(false)
#define add_constructor_with_finalizer(arg_type) constructor<arg_type>(false)

#define declare_is_subtype_of(A, B) template<> struct jlcxx::SuperType<A> { typedef B type; };
#define make_not_mirrored(Name) template<> struct jlcxx::IsMirroredType<Name> : std::false_type {};
#define add_enum(Name) add_bits<Name>(#Name, jlcxx::julia_type("CppEnum"))
#define add_enum_value(EnumName, PREFIX, VALUE_NAME) set_const(std::string(#PREFIX) + "_" + std::string(#VALUE_NAME),  EnumName::VALUE_NAME)


#define DEFINE_ADD_SIGNAL(T, snake_case, return_t) \
method("connect_signal_" + std::string(#snake_case), [](T& instance, jl_function_t* task) \
{ \
    instance.connect_signal_##snake_case([](T& instance, jl_function_t* task) { \
        return jlcxx::unbox<return_t>(jl_safe_call(("T::emit_signal" + std::string(#snake_case)).c_str(), task, jlcxx::box<T&>(instance))); \
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



/// @brief mousetrap::Application
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
        //.add_type_method(Application, add_action)
        //.add_type_method(Application, remove_action)
        //.add_type_method(Application, get_action)
        //.add_type_method(Application, has_action)
    ;

    application.add_signal(Application, activate, void)

}

// main
JLCXX_MODULE define_julia_module(jlcxx::Module& module)
{
    implement_application(module);

    module.method("test_initialize", [](Application& app){
        auto window = Window(app);
        auto label = Label("test");
        label.set_margin(75);
        window.set_child(label);
        window.present();
    });
}
