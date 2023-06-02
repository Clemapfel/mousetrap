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

        mousetrap.eval(:(export $name))
        return :($name(x::$type) = detail.$name(x._internal))
    end

    macro export_function(type, name, arg1_name, arg1_type)

        arg1_name = esc(arg1_name)
        arg1_type = esc(arg1_type)

        mousetrap.eval(:(export $name))
        return :($name(x::$type, $arg1_name::$arg1_type) = detail.$name(x._internal, $arg1_name))
    end

    macro export_function(type, name, arg1_name, arg1_type, arg2_name, arg2_type)

        arg1_name = esc(arg1_name)
        arg1_type = esc(arg1_type)
        arg2_name = esc(arg2_name)
        arg2_type = esc(arg2_type)

        out = Expr(:block)
        mousetrap.eval(:(export $name))
        return :($name(
                x::$type,
                $arg1_name::$arg1_type,
                $arg2_name::$arg2_type
            ) = detail.$name(x._internal, $arg1_name, $arg2_name))
    end

    macro export_function(type, name, arg1_name, arg1_type, arg2_name, arg2_type, arg3_name, arg3_type)
        
        arg1_name = esc(arg1_name)
        arg1_type = esc(arg1_type)
        arg2_name = esc(arg2_name)
        arg2_type = esc(arg2_type)
        arg3_name = esc(arg3_name)
        arg3_type = esc(arg3_type)

        mousetrap.eval(:(export $name))
        return :($name(
                x::$type,
                $arg1_name::$arg1_type,
                $arg2_name::$arg2_type,
                $arg3_name::$arg3_type
            ) = detail.$name(x._internal, $arg1_name, $arg2_name, $arg3_name))
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

        mousetrap.eval(:(export $name))
        return :($name(
                x::$type,
                $arg1_name::$arg1_type,
                $arg2_name::$arg2_type,
                $arg3_name::$arg3_type,
                $arg4_name::$arg4_type
            ) = detail.$name(x._internal, $arg1_name, $arg2_name, $arg3_name, $arg4_name))
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

      import Base: *
    *(x::Symbol, y::Symbol) = return Symbol(string(x) * string(y))

    import Base: clamp
    clamp(x::AbstractFloat, lower::AbstractFloat, upper::AbstractFloat) = if x < lower return lower elseif x > upper return upper else return x end

####### signal_components.jl

    macro add_signal(T, snake_case, Return_t)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case * :!

        Return_t = esc(Return_t)

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T)
                println(x)
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, $Return_t, ($T,)))($T(x[1][]))
                end)
            end
        )))

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T, data::Data_t) where Data_t
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, $Return_t, ($T, Data_t)))($T(x[1][]), data)
                end)
            end
        )))

        emit_signal_name = :emit_signal_ * snake_case

        push!(out.args, esc(:(
            function $emit_signal_name(x::$T) ::$Return_t
                return convert($Return_t, detail.$emit_signal_name(x._internal))
            end
        )))

        disconnect_signal_name = :disconnect_signal_ * snake_case * :!

        push!(out.args, esc(:(
            function $disconnect_signal_name(x::$T)
                detail.$disconnect_signal_name(x._internal)
            end
        )))

        set_signal_blocked_name = :set_signal_ * snake_case * :_blocked * :!

        push!(out.args, esc(:(
            function $set_signal_blocked_name(x::$T, b::Bool)
                detail.$set_signal_blocked_name(x._internal, b)
            end
        )))

        get_signal_blocked_name = :get_signal_ * snake_case * :_blocked

        push!(out.args, esc(:(
            function $get_signal_blocked_name(x::$T)
                return detail.$get_signal_blocked_name(x._internal)
            end
        )))

        push!(out.args, esc(:(export $connect_signal_name)))
        push!(out.args, esc(:(export $disconnect_signal_name)))
        push!(out.args, esc(:(export $set_signal_blocked_name)))
        push!(out.args, esc(:(export $get_signal_blocked_name)))
        push!(out.args, esc(:(export $emit_signal_name)))

        return out
    end

    macro add_signal(T, snake_case, Return_t, Arg1_t, arg1_name)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case * :!

        Return_t = esc(Return_t)

        Arg1_t = esc(Arg1_t)

        arg1_name = esc(arg1_name)

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T)
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, $Return_t, ($T, $Arg1_t)))($T(x[]))
                end)
            end
        )))

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T, data::Data_t) where Data_t
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, $Return_t, ($T, $Arg1_t, Data_t)))($T(x[]), data)
                end)
            end
        )))

        emit_signal_name = :emit_signal_ * snake_case

        push!(out.args, esc(:(
            function $emit_signal_name(x::$T, $arg1_name::$Arg1_t) ::$Return_t
                return convert($Return_t, detail.$emit_signal_name(x._internal, $arg1_name))
            end
        )))

        disconnect_signal_name = :disconnect_signal_ * snake_case * :!

        push!(out.args, esc(:(
            function $disconnect_signal_name(x::$T)
                detail.$disconnect_signal_name(x._internal)
            end
        )))

        set_signal_blocked_name = :set_signal_ * snake_case * :_blocked * :!

        push!(out.args, esc(:(
            function $set_signal_blocked_name(x::$T, b::Bool)
                detail.$set_signal_blocked_name(x._internal, b)
            end
        )))

        get_signal_blocked_name = :get_signal_ * snake_case * :_blocked

        push!(out.args, esc(:(
            function $get_signal_blocked_name(x::$T)
                return detail.$get_signal_blocked_name(x._internal)
            end
        )))

        push!(out.args, esc(:(export $connect_signal_name)))
        push!(out.args, esc(:(export $disconnect_signal_name)))
        push!(out.args, esc(:(export $set_signal_blocked_name)))
        push!(out.args, esc(:(export $get_signal_blocked_name)))
        push!(out.args, esc(:(export $emit_signal_name)))

        return out
    end

    macro add_signal(T, snake_case, Return_t, Arg1_t, arg1_name, Arg2_t, arg2_name)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case * :!

        Return_t = esc(Return_t)

        Arg1_t = esc(Arg1_t)
        Arg2_t = esc(Arg2_t)

        arg1_name = esc(arg1_name)
        arg2_name = esc(arg2_name)

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T)
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t)))($T(x[1][]), x[2], x[3])
                end)
            end
        )))

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T, data::Data_t) where Data_t
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t, Data_t)))($T(x[1][]), x[2], x[3], data)
                end)
            end
        )))

        emit_signal_name = :emit_signal_ * snake_case

        push!(out.args, esc(:(
            function $emit_signal_name(x::$T, $arg1_name::$Arg1_t, $arg2_name::$Arg2_t) ::$Return_t
                return convert($Return_t, detail.$emit_signal_name(x._internal, $arg1_name, $arg2_name))
            end
        )))

        disconnect_signal_name = :disconnect_signal_ * snake_case * :!

        push!(out.args, esc(:(
            function $disconnect_signal_name(x::$T)
                detail.$disconnect_signal_name(x._internal)
            end
        )))

        set_signal_blocked_name = :set_signal_ * snake_case * :_blocked * :!

        push!(out.args, esc(:(
            function $set_signal_blocked_name(x::$T, b::Bool)
                detail.$set_signal_blocked_name(x._internal, b)
            end
        )))

        get_signal_blocked_name = :get_signal_ * snake_case * :_blocked

        push!(out.args, esc(:(
            function $get_signal_blocked_name(x::$T)
                return detail.$get_signal_blocked_name(x._internal)
            end
        )))

        push!(out.args, esc(:(export $connect_signal_name)))
        push!(out.args, esc(:(export $disconnect_signal_name)))
        push!(out.args, esc(:(export $set_signal_blocked_name)))
        push!(out.args, esc(:(export $get_signal_blocked_name)))
        push!(out.args, esc(:(export $emit_signal_name)))

        return out
    end

    macro add_signal(T, snake_case, Return_t, Arg1_t, arg1_name, Arg2_t, arg2_name, Arg3_t, arg3_name)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case * :!

        Return_t = esc(Return_t)

        Arg1_t = esc(Arg1_t)
        Arg2_t = esc(Arg2_t)
        Arg3_t = esc(Arg3_t)

        arg1_name = esc(arg1_name)
        arg2_name = esc(arg2_name)
        arg3_name = esc(arg3_name)

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T)
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t, $Arg3_t)))($T(x[1][]), x[2], x[3], x[4])
                end)
            end
        )))

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T, data::Data_t) where Data_t
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t, $Arg3_t, Data_t)))($T(x[1][]), x[2], x[3], x[4], data)
                end)
            end
        )))

        emit_signal_name = :emit_signal_ * snake_case

        push!(out.args, esc(:(
            function $emit_signal_name(x::$T, $arg1_name::$Arg1_t, $arg2_name::$Arg2_t, $arg3_name::$Arg3_t) ::$Return_t
                return convert($Return_t, detail.$emit_signal_name(x._internal, $arg1_name, $arg2_name, $arg3_name))
            end
        )))

        disconnect_signal_name = :disconnect_signal_ * snake_case * :!

        push!(out.args, esc(:(
            function $disconnect_signal_name(x::$T)
                detail.$disconnect_signal_name(x._internal)
            end
        )))

        set_signal_blocked_name = :set_signal_ * snake_case * :_blocked * :!

        push!(out.args, esc(:(
            function $set_signal_blocked_name(x::$T, b::Bool)
                detail.$set_signal_blocked_name(x._internal, b)
            end
        )))

        get_signal_blocked_name = :get_signal_ * snake_case * :_blocked

        push!(out.args, esc(:(
            function $get_signal_blocked_name(x::$T)
                return detail.$get_signal_blocked_name(x._internal)
            end
        )))

        push!(out.args, esc(:(export $connect_signal_name)))
        push!(out.args, esc(:(export $disconnect_signal_name)))
        push!(out.args, esc(:(export $set_signal_blocked_name)))
        push!(out.args, esc(:(export $get_signal_blocked_name)))
        push!(out.args, esc(:(export $emit_signal_name)))

        return out
    end

    macro add_signal(T, snake_case, Return_t, Arg1_t, arg1_name, Arg2_t, arg2_name, Arg3_t, arg3_name, Arg4_t, arg4_name)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case * :!

        Return_t = esc(Return_t)

        Arg1_t = esc(Arg1_t)
        Arg2_t = esc(Arg2_t)
        Arg3_t = esc(Arg3_t)
        Arg4_t = esc(Arg4_t)

        arg1_name = esc(arg1_name)
        arg2_name = esc(arg2_name)
        arg3_name = esc(arg3_name)
        arg4_name = esc(arg4_name)

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T)
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t, $Arg3_t, $Arg4_t)))($T(x[1][]), x[2], x[3], x[4], x[5])
                end)
            end
        )))

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T, data::Data_t) where Data_t
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t, $Arg3_t, $Arg4_t, Data_t)))($T(x[1][]), x[2], x[3], x[4], x[5], data)
                end)
            end
        )))

        emit_signal_name = :emit_signal_ * snake_case

        push!(out.args, esc(:(
            function $emit_signal_name(x::$T, $arg1_name::$Arg1_t, $arg2_name::$Arg2_t, $arg3_name::$Arg3_t, $arg4_name::$Arg4_t) ::$Return_t
                return convert($Return_t, detail.$emit_signal_name(x._internal, $arg1_name, $arg2_name, $arg3_name, $arg4_name))
            end
        )))

        disconnect_signal_name = :disconnect_signal_ * snake_case * :!

        push!(out.args, esc(:(
            function $disconnect_signal_name(x::$T)
                detail.$disconnect_signal_name(x._internal)
            end
        )))

        set_signal_blocked_name = :set_signal_ * snake_case * :_blocked * :!

        push!(out.args, esc(:(
            function $set_signal_blocked_name(x::$T, b::Bool)
                detail.$set_signal_blocked_name(x._internal, b)
            end
        )))

        get_signal_blocked_name = :get_signal_ * snake_case * :_blocked

        push!(out.args, esc(:(
            function $get_signal_blocked_name(x::$T)
                return detail.$get_signal_blocked_name(x._internal)
            end
        )))

        push!(out.args, esc(:(export $connect_signal_name)))
        push!(out.args, esc(:(export $disconnect_signal_name)))
        push!(out.args, esc(:(export $set_signal_blocked_name)))
        push!(out.args, esc(:(export $get_signal_blocked_name)))
        push!(out.args, esc(:(export $emit_signal_name)))

        return out
    end

    macro add_signal_activate(x) return :(@add_signal $x activate Cvoid) end
    macro add_signal_shutdown(x) return :(@add_signal $x shutdown Cvoid) end

####### types.jl

    abstract type SignalEmitter end
    export SignalEmitter

    @export_type Adjustment SignalEmitter
    @export_type Application SignalEmitter

    abstract type Widget end
    export Widget

    abstract type EventController end
    export EventController

####### adjustment.jl

    function Adjustment(value::Number, lower::Number, upper::Number, increment::Number)
        return Adjustment(detail._Adjustment(
            convert(Cfloat, value),
            convert(Cfloat, lower),
            convert(Cfloat, upper),
            convert(Cfloat, increment)
        ));
    end

    @export_function Adjustment get_lower
    @export_function Adjustment get_upper
    @export_function Adjustment get_value
    @export_function Adjustment get_increment

    set_lower!(adjustment::Adjustment, x::Number) = detail.set_lower!(adjustment._internal, convert(Cfloat, x))
    export set_lower!

    set_upper!(adjustment::Adjustment, x::Number) = detail.set_upper!(adjustment._internal, convert(Cfloat, x))
    export set_upper!

    set_value!(adjustment::Adjustment, x::Number) = detail.set_value!(adjustment._internal, convert(Cfloat, x))
    export set_value!

    set_increment!(adjustment::Adjustment, x::Number) = detail.set_increment!(adjustment._internal, convert(Cfloat, x))
    export set_increment!

    function Base.show(io::IO, x::Adjustment)
        print(io, "Adjustment(" *
            string(get_value(x)) * ", " *
            string(get_lower(x)) * ", " *
            string(get_upper(x)) * ", " *
            string(get_increment(x)) * ")"
        );
    end

####### action.jl

####### alignment.jl

####### angle.jl

####### application.jl

    Application(id::String) = Application(detail._Application(id))

    import Base.run
    run(x::Application) = mousetrap.detail.run(x._internal)

    @export_function Application quit
    @export_function Application hold
    @export_function Application release
    @export_function Application mark_as_busy!
    @export_function Application unmark_as_busy!
    @export_function Application get_id

    #add_action(app::Application, action::Action) = detail.add_action(app._internal, action._internal)
    export add_action

    #get_action(app::Application, id::String) = return detail.get_action(app._internal, id)
    export get_action

    @export_function Application remove_action id String
    @export_function Application has_action id String

    @add_signal_activate Application
    @add_signal_shutdown Application

    Base.show(io::IO, x::Application) = print(io, "Application(" * get_id(x) * ")")

end # module mousetrap

using .mousetrap

for n in names(mousetrap) println(n) end

app = Application("test.app")

connect_signal_activate!(app, 1234) do app::Application, data
    println(data)
end

@show run(app)




