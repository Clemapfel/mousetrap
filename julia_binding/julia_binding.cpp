//
// Created by clem on 4/16/23.
//

#include <mousetrap.hpp>
#include <jlcxx/jlcxx.hpp>
#include "julia_interface.cpp"

using namespace mousetrap;

#define add_type_method(Type, id) method(#id, &Type::id)
#define declare_is_subtype_of(A, B) template<> struct jlcxx::SuperType<A> { typedef B type; };
#define make_not_mirrored(Name) template<> struct jlcxx::IsMirroredType<Name> : std::false_type {};
#define add_enum(Name) add_bits<Name>(#Name, jlcxx::julia_type("CppEnum"))
#define add_enum_value(EnumName, PREFIX, VALUE_NAME) set_const(std::string(#PREFIX) + "_" + std::string(#VALUE_NAME),  EnumName::VALUE_NAME)

JLCXX_MODULE define_julia_module(jlcxx::Module& module)
{
    module.method("invoke_test", [](jl_function_t* f){
        auto* typed_f = jl_eval_string(R"(
            (f, x) -> mousetrap.TypedFunction(f, Integer, (Integer,))(x)
        )");

        auto* out = jl_calln(typed_f, f, jl_box_uint64(1234));

        auto* exception_maybe = jl_exception_occurred();
        if (exception_maybe)
        {
            log::critical("In invoke_test: julia exception occurred:");
            jl_throw(exception_maybe);
        }

        return out;
    });
}
