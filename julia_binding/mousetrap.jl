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

    macro export_enum(enum)

        out = Expr(:block)
        for instance in instances(eval(enum))
            mousetrap.eval(:(export $(Symbol(instance))))
        end

        push!(out.args, :(Base.convert(type::Type{$enum}, x::Integer) = return $enum(Base.convert(Int64, x))))
        return out
    end

    macro export_function(type, name, return_t)

        return_t = esc(return_t)
        mousetrap.eval(:(export $name))
        return :($name(x::$type) = Base.convert($return_t, detail.$name(x._internal)))
    end

    macro export_function(type, name, return_t, arg1_name, arg1_type)

        return_t = esc(return_t)
        arg1_name = esc(arg1_name)
        arg1_type = esc(arg1_type)

        mousetrap.eval(:(export $name))
        out = :($name(x::$type, $arg1_name::$arg1_type) = Base.convert($return_t, detail.$name(x._internal, $arg1_name)))
        return out
    end

    macro export_function(type, name, return_t, arg1_name, arg1_type, arg2_name, arg2_type)

        return_t = esc(return_t)
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
            ) = Base.convert(return_t, detail.$name(x._internal, $arg1_name, $arg2_name)))
    end

    macro export_function(type, name, return_t, arg1_name, arg1_type, arg2_name, arg2_type, arg3_name, arg3_type)

        return_t = esc(return_t)
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
            )  = Base.convert($return_t, detail.$name(x._internal, $arg1_name, $arg2_name, $arg3_name)))
    end

    macro export_function(type, name, return_t, arg1_name, arg1_type, arg2_name, arg2_type, arg3_name, arg3_type)

        return_t = esc(return_t)
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
            )  = Base.convert($return_t, detail.$name(x._internal, $arg1_name, $arg2_name, $arg3_name, $arg4_name)))
    end

    macro export_type(name, super)

        super = esc(super)
        internal_name = Symbol("_" * "$name")

        if !isdefined(mousetrap.detail, internal_name)
            return
        end

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
            function $set_signal_blocked_name(x::$T, b)
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
            function $set_signal_blocked_name(x::$T, b)
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
            function $set_signal_blocked_name(x::$T, b)
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
            function $set_signal_blocked_name(x::$T, b)
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
            function $set_signal_blocked_name(x::$T, b)
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

    abstract type Widget end
    export Widget

    abstract type EventController end
    export EventController

    # forward declarations:

####### adjustment.jl

    @export_type Adjustment SignalEmitter

    function Adjustment(value::Number, lower::Number, upper::Number, increment::Number)
        return Adjustment(detail._Adjustment(
            convert(Cfloat, value),
            convert(Cfloat, lower),
            convert(Cfloat, upper),
            convert(Cfloat, increment)
        ));
    end

    @export_function Adjustment get_lower Float32
    @export_function Adjustment get_upper Float32
    @export_function Adjustment get_value Float32
    @export_function Adjustment get_increment Float32

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

####### alignment.jl

    @enum Alignment begin
        ALIGNMENT_START = detail.ALIGNMENT_START
        ALIGNMENT_CENTER = detail.ALIGNMENT_CENTER
        ALIGNMENT_END = detail.ALIGNMENT_END
    end
    @export_enum Alignment

####### angle.jl

    struct Angle
        _rads::Cfloat
    end
    export Angle

    degrees(x::Number) = return Angle(convert(Cfloat, deg2rad(x)))
    export degrees

    radians(x::Number) = return Angle(convert(Cfloat, x))
    export radians

    as_degrees(angle::Angle) = return rad2deg(angle._rads)
    export as_degrees

    as_radians(angle::Angle) = return angle._rads
    export as_radians

    import Base: +
    +(a::Angle, b::Angle) = return Angle(a._rads + b._rads)

    import Base: -
    -(a::Angle, b::Angle) = return Angle(a._rads - b._rads)

    import Base: *
    *(a::Angle, b::Angle) = return Angle(a._rads * b._rads)

    import Base: /
    /(a::Angle, b::Angle) = return Angle(a._rads / b._rads)

    import Base: ==
    ==(a::Angle, b::Angle) = return a._rads == b._rads

    import Base: !=
    !=(a::Angle, b::Angle) = return a._rads != b._rads

####### application.jl

    @export_type Application SignalEmitter
    Application(id::String) = Application(detail._Application(id))

    import Base.run
    run(x::Application) = mousetrap.detail.run(x._internal)

    @export_function Application quit Cvoid
    @export_function Application hold Cvoid
    @export_function Application release Cvoid
    @export_function Application mark_as_busy! Cvoid
    @export_function Application unmark_as_busy! Cvoid
    @export_function Application get_id String

    #add_action(app::Application, action::Action) = detail.add_action(app._internal, action._internal)
    export add_action

    #get_action(app::Application, id::String) = return detail.get_action(app._internal, id)
    export get_action

    @export_function Application remove_action Cvoid id String
    @export_function Application has_action Bool id String

    @add_signal_activate Application
    @add_signal_shutdown Application

    Base.show(io::IO, x::Application) = print(io, "Application(" * get_id(x) * ")")

####### action.jl

    @export_type Action SignalEmitter
    Action(id::String, app::Application) = Action(detail._Action(id, app._internal.cpp_object))

    @export_function Action get_id String
    @export_function Action set_state! Cvoid b Bool
    @export_function Action get_state Bool
    @export_function Action activate Cvoid
    @export_function Action add_shortcut! Cvoid shortcut String

    get_shortcuts(action::Action) ::Vector{String} = return detail.get_shortcuts(action._internal)[]
    export get_shortcuts

    @export_function Action clear_shortcuts! Cvoid
    @export_function Action set_enabled! Cvoid b Bool
    @export_function Action get_enabled Bool
    @export_function Action get_is_stateful Bool

    function set_function!(f, action::Action, data::Data_t) where Data_t
        detail.set_function!(action._internal, function (internal_ref)
            TypedFunction(f, Cvoid, (Action, Data_t))(Action(internal_ref[]), data)
        end)
    end

    function set_function!(f, action::Action)
        detail.set_function!(action._internal, function (internal_ref)
            TypedFunction(f, Cvoid, (Action,))(Action(internal_ref[]))
        end)
    end
    export set_function!

    function set_stateful_function!(f, action::Action, data::Data_t) where Data_t
        detail.set_stateful_function!(action._internal, function (internal_ref, state::Bool)
            TypedFunction(f, Bool, (Action, Bool, Data_t))(Action(internal_ref[]), state, data)
        end)
    end

    function set_stateful_function!(f, action::Action)
        detail.set_stateful_function!(action._internal, function (internal_ref, state::Bool)
            TypedFunction(f, Bool, (Action, Bool))(Action(internal_ref[]), state)
        end)
    end
    export set_stateful_function!

end # module mousetrap

using .mousetrap

app = Application("test.app")

connect_signal_activate!(app, 1234) do app::Application, data
    action = Action("test", app)
    set_stateful_function!(action) do action::Action, state
        println(state)
        return !state
    end
    add_shortcut!(action, "<Control>h")
    add_shortcut!(action, "<Control>d")
    println(get_shortcuts(action))
    activate(action)
end

@show run(app)




