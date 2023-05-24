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
        auto str = std::string(jl_string_ptr(jl_calln(exception_to_string, exception_maybe)));
        log::critical("In " + domain_name + ": " + str, JULIA_DOMAIN);
    }
}

#define add_type_method(Type, id) method(#id, &Type::id)
#define add_constructor(arg_type) constructor<arg_type>(false)
#define add_constructor_with_finalizer(arg_type) constructor<arg_type>(false)

#define declare_is_subtype_of(A, B) template<> struct jlcxx::SuperType<A> { typedef B type; };
#define make_not_mirrored(Name) template<> struct jlcxx::IsMirroredType<Name> : std::false_type {};
#define add_enum(Name) add_bits<Name>(#Name, jlcxx::julia_type("CppEnum"))
#define add_enum_value(EnumName, PREFIX, VALUE_NAME) set_const(std::string(#PREFIX) + "_" + std::string(#VALUE_NAME),  EnumName::VALUE_NAME)

/// @brief signal activate
template<typename Wrapper_t>
void add_signal_activate(Wrapper_t& type)
{
    type.method("connect_signal_activate", [](Application& instance, jl_function_t* f, jl_value_t* data)
    {
        instance.connect_signal_activate([](Application& instance, jl_value_t* data) {

            auto* typed_f = jl_eval_string(R"(
                return mousetrap.TypedFunction(f, Cvoid, (mousetrap.SignalEmitter, Any,))
            )");

            auto* out = jl_calln(typed_f, jlcxx::box<Application>(instance), data);
            forward_last_exception("connect_signal_activate");
            return out;
        }, data);
    });
}

/// @brief mousetrap::Application
static void implement_application(jlcxx::Module& module)
{
    auto application = module.add_type<Application>("Application")
        .add_constructor(const std::string&)
        .add_type_method(Application, run)
        .add_type_method(Application, quit)
        .add_type_method(Application, hold)
        .add_type_method(Application, release)
        .add_type_method(Application, mark_as_busy)
        .add_type_method(Application, unmark_as_busy)
        //.add_type_method(Application, add_action)
        //.add_type_method(Application, remove_action)
        //.add_type_method(Application, get_action)
        //.add_type_method(Application, has_action)
    ;

    application.method("connect_signal_activate", [](Application& instance, jl_function_t* f, jl_value_t* data)
    {
        instance.connect_signal_activate([](Application& instance, jl_value_t* data) {

            auto* typed_f = jl_eval_string(R"(
                return mousetrap.TypedFunction(f, Cvoid, (mousetrap.SignalEmitter, Any,))
            )");

            auto* out = jl_calln(typed_f, jlcxx::box<Application>(instance), data);
            forward_last_exception("connect_signal_activate");
            return out;
        }, data);
    });
}

// main
JLCXX_MODULE define_julia_module(jlcxx::Module& module)
{
    implement_application(module);
}
