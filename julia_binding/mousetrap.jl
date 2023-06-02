module mousetrap

####### typed_function.jl

    """
    # TypedFunction

    Object used to invoke an arbitrary function using the given signature. This wrapper
    will automatically convert any arguments and return values to the given types
    unless impossible, at which point an assertion error will be thrown on instantiation.

    In this way, it can be used to assert a functions signature at compile time.

    ### Example

    ```julia
    as_typed = TypedFunction(Int64, (Integer,)) do(x::Integer)
        return string(x)
    end
    as_typed(12) # returns 12, because "12" will be converted to given return type, Int64
    ```
    """
    mutable struct TypedFunction

        _apply::Function
        _return_t::Type
        _arg_ts::Tuple

        function TypedFunction(f::Function, return_t::Type, arg_ts::Tuple)

            actual_return_ts = Base.return_types(f, arg_ts)
            for arg_t in arg_ts

                match_found = false
                for type in actual_return_ts
                     if type <: return_t || !isempty(Base.return_types(Base.convert, (Type{return_t}, type)))
                        match_found = true
                        break;
                    end
                end

                if !match_found
                     signature = "("
                     for i in 1:length(arg_ts)
                        signature = signature * string(arg_ts[i]) * ((i < length(arg_ts)) ? ", " : ")")
                     end
                     signature = signature * " -> $return_t"
                     throw(AssertionError("Object `$f` is not invokable as function with signature `$signature`"))
                end
            end

            return new(f, return_t, arg_ts)
        end
    end
    export TypedFunction

    function (instance::TypedFunction)(args...)
        return Base.convert(instance._return_t, instance._apply([Base.convert(instance._arg_ts[i], args[i]) for i in 1:length(args)]...))
    end

####### detail.jl

    module detail
        using CxxWrap
        function __init__() @initcxx end
        @wrapmodule("./libjulia_binding.so")

        const ENUM_VALUE_01::Cint = 1234
        const ENUM_VALUE_02::Cint = 1235
        const ENUM_VALUE_03::Cint = 1236
        const ENUM_VALUE_04::Cint = 1237
    end

####### common.jl

    function safe_call(scope::String, f, args...)
        try
            return f(args...)
        catch exception
            printstyled(stderr, "[ERROR] "; bold = true, color = :red)
            printstyled(stderr, "In " * scope * ": "; bold = true)
            Base.showerror(stderr, exception, catch_backtrace())
            print(stderr, "\n")
            throw(exception) # this causes jl_call to return nullptr, which we can check against C-side
        end
    end

    const DISABLE_IF_UNDEFINED = true

    macro export_enum(enum)

        out = Expr(:block)
        for instance in instances(eval(enum))
            mousetrap.eval(:(export $(Symbol(instance))))
        end

        push!(out.args, :(Base.convert(type::Type{$enum}, x::Integer) = return $enum(Base.convert(Int64, x))))
        return out
    end

    macro export_function(type, name)

        out = Expr(:block)
        mousetrap.eval(:(export $name))
        push!(out.args, :($name(x::$type) = detail.$name(x._internal)))
        return out
    end

    macro export_function(type, name, arg1_name, arg1_type)

        arg1_name = esc(arg1_name)
        arg1_type = esc(arg1_type)

        out = Expr(:block)
        mousetrap.eval(:(export $name))
        push!(out.args, :($name(x::$type, $arg1_name::$arg1_type) = detail.$name(x._internal, $arg1_name)))
        return out
    end

    macro export_function(type, name, arg1_name, arg1_type, arg2_name, arg2_type)

        arg1_name = esc(arg1_name)
        arg1_type = esc(arg1_type)
        arg2_name = esc(arg2_name)
        arg2_type = esc(arg2_type)

        out = Expr(:block)
        mousetrap.eval(:(export $name))
        push!(out.args, :($name(
                x::$type,
                $arg1_name::$arg1_type,
                $arg2_name::$arg2_type
            ) = detail.$name(x._internal, $arg1_name, $arg2_name))
        )
        return out
    end

    macro export_function(type, name, arg1_name, arg1_type, arg2_name, arg2_type, arg3_name, arg3_type)
        
        arg1_name = esc(arg1_name)
        arg1_type = esc(arg1_type)
        arg2_name = esc(arg2_name)
        arg2_type = esc(arg2_type)
        arg3_name = esc(arg3_name)
        arg3_type = esc(arg3_type)

        out = Expr(:block)
        mousetrap.eval(:(export $name))
        push!(out.args, :($name(
                x::$type,
                $arg1_name::$arg1_type,
                $arg2_name::$arg2_type,
                $arg3_name::$arg3_type
            ) = detail.$name(x._internal, $arg1_name, $arg2_name, $arg3_name))
        )
        return out
    end

    macro export_function(type, name, arg1_name, arg1_type, arg2_name, arg2_type, arg3_name, arg3_type)

        arg1_name = esc(arg1_name)
        arg1_type = esc(arg1_type)
        arg2_name = esc(arg2_name)
        arg2_type = esc(arg2_type)
        arg3_name = esc(arg3_name)
        arg3_type = esc(arg3_type)
        arg4_name = esc(arg4_name)
        arg4_type = esc(arg4_type)

        out = Expr(:block)
        mousetrap.eval(:(export $name))
        push!(out.args, :($name(
                x::$type,
                $arg1_name::$arg1_type,
                $arg2_name::$arg2_type,
                $arg3_name::$arg3_type,
                $arg4_name::$arg4_type
            ) = detail.$name(x._internal, $arg1_name, $arg2_name, $arg3_name, $arg4_name))
        )
        return out
    end

    macro export_type(name, super)

        super = esc(super)
        internal_name = Symbol("_" * "$name")

        out = Expr(:block)
        mousetrap.eval(:(export $name))
        push!(out.args, quote
            struct $name <: $super
                _internal::detail.$internal_name
            end
        end)
        return out
    end

####### types.jl

    abstract type SignalEmitter end
    export SignalEmitter

    abstract type Widget end
    export Widget

    abstract type EventController end
    export EventController

####### types.jl



end # module mousetrap

using .mousetrap

println(methods(mousetrap.detail.test_enum))
#mousetrap.detail.test_enum(TEST_ENUM_VALUE_01)




