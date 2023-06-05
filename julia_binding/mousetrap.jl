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

    macro export_function(type, name, return_t, arg1_name, arg1_type, arg2_name, arg2_type, arg3_name, arg3_type, arg4_name, arg4_type)

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

        if !isdefined(detail, :($internal_name))
            throw(AssertionError("In mousetrap.@export_type: detail.$internal_name undefined"))
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

    macro export_type(name)

        internal_name = Symbol("_" * "$name")

        if !isdefined(detail, :($internal_name))
            throw(AssertionError("In mousetrap.@export_type: detail.$internal_name undefined"))
        end

        out = Expr(:block)
        mousetrap.eval(:(export $name))
        push!(out.args, quote
            struct $name
                _internal::detail.$internal_name
            end
        end)
        return out
    end

    macro export_enum(enum, block)

        @assert block isa Expr

        out = Expr(:toplevel)

        push!(out.args, (:(export $enum)))

        detail_enum_name = :_ * enum
        @assert isdefined(detail, detail_enum_name)

        push!(out.args, :(const $(esc(enum)) = mousetrap.detail.$detail_enum_name))
        for name in block.args
            if !(name isa Symbol)
                continue
            end

            push!(out.args, :(const $(esc(name)) = detail.$name))
            push!(out.args, :(export $name))
        end

        to_int_name = Symbol(enum) * :_to_int
        push!(out.args, :(Base.string(x::$enum) = return string(mousetrap.detail.$to_int_name(x))))
        push!(out.args, :(Base.convert(::Type{Integer}, x::$enum) = return Integer(mousetrap.detail.to_int_name(x))))

        return out
    end

    function show_aux(io::IO, x::T, fields::Symbol...) where T

        out = ""

        tostring(x) = string(x)
        tostring(x::String) = "\"" * x * "\""

        super = ""
        if T <: SignalEmitter
            super = tostring(SignalEmitter)
        elseif T <: Widget
            super = tostring(Widget)
        elseif T <: EventController
            super = tostring(EventController)
        end

        out *= "mousetrap." * tostring(T) * (!isempty(super) ? " <: " * super : "") * "\n"
        #out *= "  " * "internal" * " = @" * tostring(x._internal.cpp_object) * "\n"

        for field in fields
            getter = getproperty(mousetrap, Symbol("get_") * field)
            out *= "  " * tostring(field) * " = " * tostring(getter(x)) * "\n"
        end

        print(io, out, "end\n")
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
                typed_f = TypedFunction(f, $Return_t, ($T,))
                detail.$connect_signal_name(x._internal, function(x)
                    typed_f($T(x[1][]))
                end)
            end
        )))

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T, data::Data_t) where Data_t
                typed_f = TypedFunction(f, $Return_t, ($T, Data_t))
                detail.$connect_signal_name(x._internal, function(x)
                    typed_f($T(x[1][]), data)
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
                typed_f = TypedFunction(f, $Return_t, ($T, $Arg1_t))
                detail.$connect_signal_name(x._internal, function(x)
                    typed_f($T(x[]))
                end)
            end
        )))

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T, data::Data_t) where Data_t
                typed_f = TypedFunction(f, $Return_t, ($T, $Arg1_t, Data_t))
                detail.$connect_signal_name(x._internal, function(x)
                    typed_f($T(x[]), data)
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
                typed_f = TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t))
                detail.$connect_signal_name(x._internal, function(x)
                    typed_f($T(x[1][]), x[2], x[3])
                end)
            end
        )))

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T, data::Data_t) where Data_t
                typed_f = TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t, Data_t))
                detail.$connect_signal_name(x._internal, function(x)
                    typed_f($T(x[1][]), x[2], x[3], data)
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
                typed_f = TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t, $Arg3_t))
                detail.$connect_signal_name(x._internal, function(x)
                    typed_f($T(x[1][]), x[2], x[3], x[4])
                end)
            end
        )))

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T, data::Data_t) where Data_t
                typed_f = TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t, $Arg3_t, Data_t))
                detail.$connect_signal_name(x._internal, function(x)
                    typed_f($T(x[1][]), x[2], x[3], x[4], data)
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
                typed_f = TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t, $Arg3_t, $Arg4_t))
                detail.$connect_signal_name(x._internal, function(x)
                    typed_f($T(x[1][]), x[2], x[3], x[4], x[5])
                end)
            end
        )))

        push!(out.args, esc(:(
            function $connect_signal_name(f, x::$T, data::Data_t) where Data_t
                typed_f = TypedFunction(f, $Return_t, ($T, $Arg1_t, $Arg2_t, $Arg3_t, $Arg4_t, Data_t))
                detail.$connect_signal_name(x._internal, function(x)
                    typed_f($T(x[1][]), x[2], x[3], x[4], x[5], data)
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

    macro add_widget_signals(x)
        return quote
            @add_signal $x realize Cvoid
            @add_signal $x unrealize Cvoid
            @add_signal $x destroy Cvoid
            @add_signal $x hide Cvoid
            @add_signal $x show Cvoid
            @add_signal $x map Cvoid
            @add_signal $x unmap Cvoid
        end
    end

    macro add_signal_activate(x) return :(@add_signal $x activate Cvoid) end
    macro add_signal_shutdown(x) return :(@add_signal $x shutdown Cvoid) end
    macro add_signal_clicked(x) return :(@add_signal $x clicked Cvoid) end
    macro add_signal_toggled(x) return :(@add_signal $x toggled Cvoid) end
    macro add_signal_activate_default_widget(x) return :(@add_signal $x activate_default_widget Cvoid) end
    macro add_signal_activate_focused_widget(x) return :(@add_signal $x activate_focused_widget Cvoid) end
    macro add_signal_close_request(x) return :(@add_signal $x close_request WindowCloseRequestResult) end
    macro add_signal_items_changed(x) return :(@add_signal $x items_changed Cvoid Int32 position Int32 n_removed Int32 n_added) end
    macro add_signal_closed(x) return :(@add_signal $x closed Cvoid) end

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

    Base.show(io::IO, x::Adjustment) = mousetrap.show_aux(io, x, :value, :lower, :upper, :increment)

####### alignment.jl

    @export_enum Alignment begin
        ALIGNMENT_START
        ALIGNMENT_CENTER
        ALIGNMENT_END
    end

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
    @export_type Action SignalEmitter

    Application(id::String) = Application(detail._Application(id))

    import Base.run
    run(x::Application) = mousetrap.detail.run(x._internal)

    @export_function Application quit Cvoid
    @export_function Application hold Cvoid
    @export_function Application release Cvoid
    @export_function Application mark_as_busy! Cvoid
    @export_function Application unmark_as_busy! Cvoid
    @export_function Application get_id String

    add_action(app::Application, action::Action) = detail.add_action(app._internal, action._internal)
    export add_action

    get_action(app::Application, id::String) ::Action = return Action(detail.get_action(app._internal, id))
    export get_action

    @export_function Application remove_action Cvoid id String
    @export_function Application has_action Bool id String

    @add_signal_activate Application
    @add_signal_shutdown Application

    Base.show(io::IO, x::Application) = mousetrap.show(x, :id)

####### action.jl

    #@export_type Action SignalEmitter
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
        typed_f = TypedFunction(f, Cvoid, (Action, Data_t))
        detail.set_function!(action._internal, function (internal_ref)
            typed_f(Action(internal_ref[]), data)
        end)
    end

    function set_function!(f, action::Action)
        typed_f = TypedFunction(f, Cvoid, (Action,))
        detail.set_function!(action._internal, function (internal_ref)
            typed_f(Action(internal_ref[]))
        end)
    end
    export set_function!

    function set_stateful_function!(f, action::Action, data::Data_t) where Data_t
        typed_f = TypedFunction(f, Bool, (Action, Bool, Data_t))
        detail.set_stateful_function!(action._internal, function (internal_ref, state::Bool)
            typed_f(Action(internal_ref[]), state, data)
        end)
    end

    function set_stateful_function!(f, action::Action)
        typed_f = TypedFunction(f, Bool, (Action, Bool))
        detail.set_stateful_function!(action._internal, function (internal_ref, state::Bool)
            typed_f(Action(internal_ref[]), state)
        end)
    end
    export set_stateful_function!

    Base.show(io::IO, x::Action) = mousetrap.show_aux(io, x, :id, :enabled, :shortcuts, :is_stateful, :state)

####### aspect_frame.jl

    @export_type AspectFrame Widget
    function AspectFrame(ratio::AbstractFloat; child_x_alignment::AbstractFloat = 0.5, child_y_alignment::AbstractFloat = 0.5)
        return AspectFrame(detail._AspectFrame(ratio, child_x_alignment, child_y_alignment))
    end

    @export_function AspectFrame set_ratio! Cvoid ratio AbstractFloat
    @export_function AspectFrame get_ratio Cfloat
    @export_function AspectFrame set_child_x_alignment! Cvoid alignment AbstractFloat
    @export_function AspectFrame set_child_y_alignment! Cvoid alignment AbstractFloat
    @export_function AspectFrame get_child_x_alignment Cfloat
    @export_function AspectFrame get_child_y_alignment Cfloat

    @export_function AspectFrame remove_child! Cvoid

    set_child!(aspect_frame::AspectFrame, child::Widget) = detail.set_child!(aspect_frame._internal, child._internal.cpp_object)
    export set_child!

    Base.show(io::IO, x::AspectFrame) = mousetrap.show_aux(io, x, :ratio, :child_x_alignment, :child_y_alignment)

####### aspect_frame.jl

    @export_enum BlendMode begin
        BLEND_MODE_NONE
        BLEND_MODE_ADD
        BLEND_MODE_SUBTRACT
        BLEND_MODE_REVERSE_SUBTRACT
        BLEND_MODE_MULTIPLY
        BLEND_MODE_MIN
        BLEND_MODE_MAX
    end

    function set_current_blend_mode(blend_mode::BlendMode; allow_alpha_blending = true)
        detail.set_current_blend_mode(blend_mode, allow_alpha_blending)
    end
    export set_current_blend_mode

####### orientation.jl

    @export_enum Orientation begin
        ORIENTATION_HORIZONTAL
        ORIENTATION_VERTICAL
    end

####### box.jl

    @export_type Box Widget
    Box(orientation::Orientation) = Box(detail._Box(orientation))

    function push_back!(box::Box, widget::Widget)
        detail.push_back!(box._internal, widget._internal.cpp_object)
    end
    export push_back!
    
    function push_front!(box::Box, widget::Widget)
        detail.push_front!(box._internal, widget._internal.cpp_object)
    end
    export push_front!
    
    function insert_after!(box::Box, to_append::Widget, after::Widget)
        detail.push_front!(box._internal, to_append._internal.cpp_object, after._internal.cpp_object)
    end
    export insert_after!
    
    function remove!(box::Box, widget::Widget)
        detail.remove!(box._internal, widget._internal.cpp_object)
    end
    export remove!
    
    @export_function Box clear Cvoid
    @export_function Box set_homogeneous! Cvoid b Bool
    @export_function Box get_homogeneous Bool

    function set_spacing!(box::Box, spacing::Number) ::Cvoid
        detail.set_spacing!(box._internal, convert(Cfloat, spacing))
    end
    export set_spacing!

    @export_function Box get_spacing Cfloat
    @export_function Box get_n_items Cint
    @export_function Box get_orientation Orientation
    @export_function Box set_orientation! Cvoid orientation Orientation

    @add_widget_signals Box

####### button.jl

    @export_type Button Widget
    Button() = Button(detail._Button())

    @export_function Button set_has_frame! Cvoid b Bool
    @export_function Button get_has_frame Bool
    @export_function Button set_is_circular Cvoid b Bool
    @export_function Button get_is_circular Bool

    function set_child!(button::Button, child::Widget)
        detail.set_child!(button._internal, child._internal.cpp_object)
    end
    export set_child!

    @export_function Button remove_child! Cvoid

    function set_action!(button::Button, action::Action)
        detail.set_action!(button._internal, action._internal)
    end
    export set_action!

    @add_widget_signals Button
    @add_signal_activate Button
    @add_signal_clicked Button

####### center_box.jl

    @export_type CenterBox Widget
    CenterBox(orientation::Orientation) = CenterBox(detail._CenterBox(orientation))

    function set_start_child!(center_box::CenterBox, child::Widget)
        detail.set_start_child!(center_box._internal, child._internal.cpp_object)
    end
    export set_start_child!

    function set_center_child!(center_box::CenterBox, child::Widget)
        detail.set_center_child!(center_box._internal, child._internal.cpp_object)
    end
    export set_center_child!

    function set_end_child!(center_box::CenterBox, child::Widget)
        detail.set_end_child!(center_box._internal, child._internal.cpp_object)
    end
    export set_end_child!

    @export_function CenterBox remove_start_widget! Cvoid
    @export_function CenterBox remove_center_widget! Cvoid
    @export_function CenterBox remove_end_widget! Cvoid
    @export_function CenterBox get_orientation Orientation
    @export_function CenterBox set_orientation! Cvoid orientation Orientation

    @add_widget_signals CenterBox

####### check_button.jl

    @export_enum CheckButtonState begin
        CHECK_BUTTON_STATE_ACTIVE
        CHECK_BUTTON_STATE_INCONSISTENT
        CHECK_BUTTON_STATE_INACTIVE
    end

    @export_type CheckButton Widget
    CheckButton() = CheckButton(detail._CheckButton())

    @export_function CheckButton set_state! Cvoid state CheckButtonState
    @export_function CheckButton get_state CheckButtonState
    @export_function CheckButton get_is_active Bool

    if detail.GTK_MINOR_VERSION >= 8
        function set_child!(check_button::CheckButton, child::Widget)
            detail.set_child!(check_button._internal, child._internal.cpp_object)
        end
        export set_child!

        @export_function CheckButton remove_child! Cvoid
    end

    @add_widget_signals CheckButton
    @add_signal_toggled CheckButton
    @add_signal_activate CheckButton

####### key_file.jl

    @export_type KeyFile SignalEmitter
    KeyFile() = KeyFile(detail._KeyFile())
    KeyFile(path::String) = KeyFile(detail._KeyFile(path))

    const GroupID = String
    export GroupID

    const KeyID = String
    export KeyID

    @export_function KeyFile as_string String
    @export_function KeyFile create_from_file! Bool path String
    @export_function KeyFile create_from_string! Bool file String
    @export_function KeyFile save_to_file Bool path String
    @export_function KeyFile get_groups Vector{GroupID}
    @export_function KeyFile get_keys Vector{KeyID} group GroupID
    @export_function KeyFile has_key Bool group GroupID key KeyID
    @export_function KeyFile has_group Bool group GroupID
    @export_function KeyFile set_comment_above_group! Cvoid group GroupID comment String
    @export_function KeyFile set_comment_above_key! Cvoid group GroupID key KeyID comment String
    @export_function KeyFile get_comment_above_group String group GroupID
    @export_function KeyFile get_comment_above_key String group GroupID key KeyID

    export set_value!
    export get_value

    function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::Bool)
        detail.set_value_as_bool!(file._internal, group, key, value)
    end

    function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::AbstractFloat)
        detail.set_value_as_double!(file._internal, group, key, convert(Cdouble, value))
    end

    function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::Signed)
        detail.set_value_as_int!(file._internal, group, key, convert(Cint, value))
    end

    function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::Unsigned)
        detail.set_value_as_uint!(file._internal, group, key, convert(Cuint, value))
    end

    function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::String)
        detail.set_value_as_string!(file._internal, group, key, value)
    end

    #function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::RGBA)
    #    detail.set_value_as_float_list!(file._internal, group, key, Cfloat[value.r, value.g, value.b, value.a])
    #end

    #function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::Image)
    #   TODO
    #end

    ##

    function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::Vector{Bool})
        detail.set_value_as_bool_list!(file._internal, group, key, value)
    end

    function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::Vector{<: AbstractFloat})
        vec::Vector{Cdouble} = []
        for x in value
            push!(vec, convert(Cdouble, x))
        end
        detail.set_value_as_double_list!(file._internal, group, key, vec)
    end

    function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::Vector{<: Signed})
        vec::Vector{Cint} = []
        for x in value
            push!(vec, convert(Cint, x))
        end
        detail.set_value_as_int_list!(file._internal, group, key, vec)
    end

    function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::Vector{<: Unsigned})
        vec::Vector{Cuint} = []
        for x in value
            push!(vec, convert(Cuint, x))
        end
        detail.set_value_as_uint_list!(file._internal, group, key, vec)
    end

    function set_value!(file::KeyFile, group::GroupID, key::KeyID, value::Vector{String})
        detail.set_value_as_string_list!(file._internal, group, key, value)
    end

    ##

    function get_value(file::KeyFile, type::Type{Bool}, group::GroupID, key::KeyID)
        return detail.get_value_as_bool(file._internal, group, key)
    end

    function get_value(file::KeyFile, type::Type{<: AbstractFloat}, group::GroupID, key::KeyID)
        return detail.get_value_as_double(file._internal, group, key)
    end

    function get_value(file::KeyFile, type::Type{<: Signed}, group::GroupID, key::KeyID)
        return detail.get_value_as_int(file._internal, group, key)
    end

    function get_value(file::KeyFile, type::Type{<: Unsigned}, group::GroupID, key::KeyID)
        return detail.get_value_as_uint(file._internal, group, key)
    end

    function get_value(file::KeyFile, type::Type{String}, group::GroupID, key::KeyID)
        return detail.get_value_as_string(file._internal, group, key)
    end

    #function get_value(file::KeyFile, type::Type{RGBA}, group::GroupID, key::KeyID)
    #    vec = get_value(file, Vector{Cfloat}, group, key)
    #    return RGBA(vec[1], vec[2], vec[3], vec[4])
    #end

    #function get_value(file::KeyFile, type::Type{Image}, group::GroupID, key::KeyID)
    #    TODO
    #end

    ##

    function get_value(file::KeyFile, type::Type{Vector{Bool}}, group::GroupID, key::KeyID)
        return convert(Vector{Bool}, detail.get_value_as_bool_list(file._internal, group, key))
    end

    function get_value(file::KeyFile, type::Type{Vector{T}}, group::GroupID, key::KeyID) where T <: AbstractFloat
        return convert(Vector{T}, detail.get_value_as_double_list(file._internal, group, key))
    end

    function get_value(file::KeyFile, type::Type{Vector{T}}, group::GroupID, key::KeyID) where T <: Signed
        return convert(Vector{T}, detail.get_value_as_int_list(file._internal, group, key))
    end

    function get_value(file::KeyFile, type::Type{Vector{T}}, group::GroupID, key::KeyID) where T <: Unsigned
        return convert(Vector{T}, detail.get_value_as_uint_list(file._internal, group, key))
    end

    function get_value(file::KeyFile, type::Type{Vector{String}}, group::GroupID, key::KeyID)
        return convert(Vector{String}, detail.get_value_as_string_list(file._internal, group, key))
    end

####### color.jl

    abstract type Color end

    struct RGBA <: Color
        r::Cfloat
        g::Cfloat
        b::Cfloat
        a::Cfloat
    end
    export RGBA

    function RGBA(r::AbstractFloat, g::AbstractFloat, b::AbstractFloat, a::AbstractFloat)
        return RBGA(
            convert(Cfloat, r),
            convert(Cfloat, g),
            convert(Cfloat, b),
            convert(Cfloat, a)
        )
    end

    struct HSVA <: Color
        h::Cfloat
        s::Cfloat
        v::Cfloat
        a::Cfloat
    end
    export HSVA

    function HSVA(h::AbstractFloat, s::AbstractFloat, v::AbstractFloat, a::AbstractFloat)
        return HSVA(
            convert(Cfloat, h),
            convert(Cfloat, s),
            convert(Cfloat, v),
            convert(Cfloat, a)
        )
    end

    rgba_to_hsva(rgba::RGBA) = detail.rgba_to_hsva(rgba)
    export rgba_to_hsva

    hsva_to_rgba(hsva::HSVA) = detail.hsva_to_rgba(hsva)
    export hsva_to_rgba

    rgba_to_html_code(rgba::RGBA) = convert(String, detail.rgba_to_html_code(rgba))
    export rgba_to_html_code

    html_code_to_rgba(code::String) ::RGBA = return detail.html_code_to_rgba(code)
    export html_code_to_rgba

####### window.jl

    @export_enum WindowCloseRequestResult begin
        WINDOW_CLOSE_REQUEST_RESULT_ALLOW_CLOSE
        WINDOW_CLOSE_REQUEST_RESULT_PREVENT_CLOSE
    end

    @export_type Window Widget
    Window(app::Application) = Window(detail._Window(app._internal))

    function set_application!(window::Window, app::Application)
        detail.set_application!(window._internal, app._internal)
    end
    export set_application!

    @export_function Window set_maximized! Cvoid
    @export_function Window set_fullscreen! Cvoid
    @export_function Window present! Cvoid
    @export_function Window set_hide_on_close! Cvoid b Bool
    @export_function Window close! Cvoid

    function set_child!(window::Window, child::Widget)
        detail.set_child!(window._internal, child._internal.cpp_object)
    end
    export set_child!

    @export_function Window remove_child! Cvoid
    @export_function Window set_destroy_with_parent! Cvoid b Bool
    @export_function Window get_destroy_with_parent Bool
    @export_function Window set_title! Cvoid title String
    @export_function Window get_title String

    function set_titlebar_widget!(window::Window, titlebar::Widget)
        detail.set_titlebar_widget!(window._internal, titlebar._internal.cpp_object)
    end
    export set_titlebar_widget!

    @export_function Window remove_titlebar_widget! Cvoid
    @export_function Window set_is_modal! Cvoid b Bool
    @export_function Window get_is_modal Bool

    function set_transient_for(self::Window, other::Window)
        detail.set_transient_for!(self._itnernal, other._internal)
    end
    export set_transient_for!

    @export_function Window set_is_decorated! Cvoid b Bool
    @export_function Window get_is_decorated Bool
    @export_function Window set_has_close_button! Cvoid b Bool
    @export_function Window get_has_close_button Bool
    @export_function Window set_startup_notification_identifier! Cvoid id String
    @export_function Window set_focus_visible! Cvoid b Bool
    @export_function Window get_focus_visible Bool

    function set_default_widget!(window::Window, widget::Widget)
        detail.set_default_widget!(window._internal, widget._internal.cpp_object)
    end
    export set_default_widget!

    @add_widget_signals Window
    @add_signal_close_request Window
    @add_signal_activate_default_widget Window
    @add_signal_activate_focused_widget Window

####### icon.jl

    @export_type Icon
    Icon() = Icon(detail._Icon())

    @export_type IconTheme
    IconTheme(window::Window) = IconTheme(detail._IconTheme(window._internal))

    const IconID = String

    # Icon

    @export_function Icon create_from_file! Bool path String

    function create_from_theme!(icon::Icon, theme::IconTheme, id::IconID, square_resolution::Unsigned, scale::Unsigned = Unsigned(1))
        detail.create_from_theme!(icon._internal, theme._internal.cpp_object, id, square_resolution, scale)
    end
    export create_from_theme!

    @export_function Icon get_name IconID

    import Base.==
    ==(a::Icon, b::Icon) = return detail.compare_icons(a._internal, b_internal)

    import Base.!=
    !=(a::Icon, b::Icon) = return !Base.==(a, b)

    @export_function Icon get_size Vector2i

    # IconTheme

    function get_icon_names(theme::IconTheme) ::Vector{String}
        return detail.get_icon_names(theme._internal)
    end
    export get_icon_names

    has_icon(theme::IconTheme, icon::Icon) = detail.has_icon_icon(theme._internal, icon._internal)
    has_icon(theme::IconTheme, id::IconID) = detail.has_icon_id(theme._internal, id)
    export has_icon

    @export_function IconTheme add_resource_path! Cvoid path String
    @export_function IconTheme set_resource_path! Cvoid path String

####### image.jl

    @export_enum InterpolationType begin
        INTERPOLATION_TYPE_NEAREST
        INTERPOLATION_TYPE_TILES
        INTERPOLATION_TYPE_BILINEAR
        INTERPOLATION_TYPE_HYPERBOLIC
    end

    @export_type Image
    Image() = Image(detail._Image())
    Image(path::String) = Image(detail._Image(path))
    Image(width::Unsigned, height::Unsigned, color::RGBA = RGBA(0, 0, 0, 1)) = Image(detail._Image(width, height, color))

    function create!(image::Image, width::Unsigned, height::Unsigned, color::RGBA = RGBA(0, 0, 0, 1))
        detail.create!(image._internal, width, height, color)
    end
    export create!

    @export_function Image create_from_file! Bool path String
    @export_function Image save_to_file Bool path String
    @export_function Image get_n_pixels Int64
    @export_function Image get_size Vector2f

    function as_scaled(image::Image, size_x::Unsigned, size_y::Unsigned, interpolation::InterpolationType)
        return Image(detail.as_scaled(image._internal, size_x, size_y, interpolation))
    end
    export as_scaled

    function as_cropped(image::Image, offset_x::Integer, offset_y::Integer, new_width::Unsigned, new_height::Unsigned)
        return Image(detail.as_cropped(image._internal, offset_x, offset_y, new_width, new_height))
    end
    export as_cropped

    function set_pixel!(image::Image, x::Unsigned, y::Unsigned, color::RGBA)
        detail.set_pixel_rgba!(image._internal, x, y, color)
    end
    function set_pixel!(image::Image, x::Unsigned, y::Unsigned, color::HSVA)
        detail.set_pixel_hsva!(image._internal, x, y, color)
    end
    export set_pixel!

    function get_pixel(image::Image, x::Unsigned, y::Unsigned) ::RGBA
        return detail.get_pixel(image._internal, x, y)
    end
    export get_pixel

####### image_display.jl

    @export_type ImageDisplay Widget
    ImageDisplay(path::String) = ImageDisplay(detail._ImageDisplay(path))
    ImageDisplay(image::Image) = ImageDisplay(detail._ImageDisplay(image._internal))
    ImageDisplay(icon::Icon) = ImageDisplay(detail._ImageDisplay(icon._internal))

    @export_function ImageDisplay create_from_file! Cvoid path String

    create_from_image!(image_display::ImageDisplay, image::Image) = detail.create_from_image!(image_display._internal, image._internal)
    export create_from_image!

    create_from_icon!(image_display::ImageDisplay, icon::Icon) = detail.create_from_icon!(image_display._internal, icon._internal)
    export create_from_icon!

    @export_function ImageDisplay clear! Cvoid
    @export_function ImageDisplay set_scale! Cvoid scale Cint

    @add_widget_signals ImageDisplay

####### relative_position.jl

    @export_enum RelativePosition begin
        RELATIVE_POSITION_ABOVE
        RELATIVE_POSITION_BELOW
        RELATIVE_POSITION_LEFT_OF
        RELATIVE_POSITION_RIGHT_OF
    end

####### menu_model.jl

    @export_type MenuModel SignalEmitter
    MenuModel() = MenuModel(detail._MenuModel())

    add_action!(model::MenuModel, label::String, action::Action) = detail.add_action!(model._internal, label, action._internal)
    export add_action!

    add_widget!(model::MenuModel, widget::Widget) = detail.add_widget!(model._internal, widget._internal.cpp_object)
    export add_widget!

    @export_enum SectionFormat begin
        SECTION_FORMAT_NORMAL
        SECTION_FORMAT_HORIZONTAL_BUTTONS
        SECTION_FORMAT_HORIZONTAL_BUTTONS_LEFT_TO_RIGHT
        SECTION_FORMAT_HORIZONTAL_BUTTONS_RIGHT_TO_LEFT
        SECTION_FORMAT_CIRCULAR_BUTTONS
        SECTION_FORMAT_INLINE_BUTTONS
    end

    function add_section!(model::MenuModel, title::String, to_add::MenuModel, section_format::SectionFormat = SECTION_FORMAT_NORMAL)
        detail.add_section!(model._internal, title, to_add._internal, section_format)
    end
    export add_section!

    add_submenu!(model::MenuModel, label::String, to_add::MenuModel) = detail.add_submenu!(model._internal, label, to_add._internal)
    export add_submenu!

    add_icon!(model::MenuModel, icon::Icon) = detail.add_icon!(model._internal, icon._internal)
    export add_icon!

    @add_signal_items_changed MenuModel

####### popover_menu.jl

    @export_type PopoverMenu Widget
    PopoverMenu(model::MenuModel) = PopoverMenu(detail._PopoverMenu(model._internal))

    @add_widget_signals PopoverMenu
    @add_signal_closed PopoverMenu

###### popover.jl

    @export_type Popover Widget
    Popover() = Popover(detail._Popover())

    @export_function Popover popup! Cvoid
    @export_function Popover popdown! Cvoid
    @export_function Popover present! Cvoid

    function set_child!(popover::Popover, child::Widget)
        detail.set_child!(popover._internal, child._internal.cpp_object)
    end
    export set_child!

    @export_function Popover remove_child! Cvoid

    function attach_to!(popover::Popover, attachment::Widget)
        detail.atach_to!(popover._internal, child._internal.cpp_object)
    end
    export attach_to!

    @export_function Popover set_relative_position! Cvoid position RelativePosition
    @export_function Popover get_relative_position RelativePosition
    @export_function Popover set_autohide! Cvoid b Bool
    @export_function Popover get_autohide Bool
    @export_function Popover set_has_base_arrow! Cvoid b Bool
    @export_function Popover get_has_base_arrow Bool

    @add_widget_signals Popover
    @add_signal_closed Popover

###### popover_button.jl

    @export_type PopoverButton Widget
    PopoverButton() = PopoverButton(detail._PopoverButton())

    set_child!(popover_button::PopoverButton, child::Widget) = detail.set_child!(popover_button._internal, child._internal.cpp_object)
    export set_child!

    @export_function PopoverButton remove_child! Cvoid

    function set_popover!(popover_button::PopoverButton, popover::Popover)
        detail.set_popover!(popover_button._internal, popover._internal)
    end
    export set_popover!

    function set_popover_menu!(popover_button::PopoverButton, popover_menu::PopoverMenu)
        detail.set_popover_menu!(popover_button._internal, popover_menu._internal)
    end
    export set_popover_menu!

    @export_function PopoverButton remove_popover! Cvoid
    @export_function PopoverButton set_popover_position! Cvoid position RelativePosition
    @export_function PopoverButton get_relative_position RelativePosition
    @export_function PopoverButton set_always_show_arrow! Cvoid b Bool
    @export_function PopoverButton get_always_show_arrow Bool
    @export_function PopoverButton set_has_frame Cvoid b Bool
    @export_function PopoverButton get_has_frame Bool
    @export_function PopoverButton popoup! Cvoid
    @export_function PopoverButton popdown! Cvoid
    @export_function PopoverButton set_is_circular! Cvoid b Bool
    @export_function PopoverButton get_is_circular Bool

    @add_widget_signals PopoverButton
    @add_signal_activate PopoverButton

###### menubar.jl

    @export_type MenuBar Widget
    MenuBar(model::MenuModel) = MenuBar(detail._MenuBar(model._internal))

    @add_widget_signals MenuBar

end # module mousetrap

using .mousetrap

using CxxWrap

app = Application("example.app")

connect_signal_activate!(app) do app::Application

    window = Window(app)

    action_01 = Action("action_01", app)
    set_function!(action_01) do action::Action
        println("01")
    end

    action_02 = Action("action_02", app)
    set_stateful_function!(action_02) do action::Action, b::Bool
        println("02", b)
        return !b
    end

    model = MenuModel()
    section = MenuModel()

    add_action!(section, "01", action_01)
    add_action!(section, "02", action_02)
    add_submenu!(model, "section", section)

    popover = PopoverMenu(model)
    popover_button = PopoverButton()
    set_popover_menu!(popover_button, popover)

    menubar = MenuBar(model)

    set_child!(window, menubar)
    present!(window)
end

exit(run(app))




