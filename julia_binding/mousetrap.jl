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

###### vector.jl

    using StaticArrays

    """
    # Vector2{T}

    ## Public Fields
    x::T
    y::T
    """
    const Vector2{T} = SVector{2, T}
    export Vector2

    function Base.getproperty(v::Vector2{T}, symbol::Symbol) where T
        if symbol == :x
            return v[1]
        elseif symbol == :y
            return v[2]
        else
            throw(ErrorException("type Vector2 has no field " * string(symbol)))
        end
    end

    function Base.setproperty!(v::Vector2{T}, symbol::Symbol, value) where T
        if symbol == :x
            v[1] = convert(T, value)
        elseif symbol == :y
            v[2] = convert(T, value)
        else
            throw(ErrorException("type Vector2 has no field " * string(symbol)))
        end
    end

    const Vector2f = Vector2{Cfloat}
    export Vector2f

    const Vector2i = Vector2{Cint}
    export Vector2i

    const Vector2ui = Vector2{Csize_t}
    export Vector2ui

    """
    # Vector3{T}

    ## Public Fields
    x::T
    y::T
    z::T
    """
    const Vector3{T} = SVector{3, T}
    export Vector3

    function Base.getproperty(v::Vector3{T}, symbol::Symbol) where T
        if symbol == :x
            return v[1]
        elseif symbol == :y
            return v[2]
        elseif symbol == :z
            return v[3]
        else
            throw(ErrorException("type Vector3 has no field " * string(symbol)))
        end
    end

    function Base.setproperty!(v::Vector2{T}, symbol::Symbol, value) where T
        if symbol == :x
            v[1] = convert(T, value)
        elseif symbol == :y
            v[2] = convert(T, value)
        elseif symbol == :z
            v[3] = convert(T, value)
        else
            throw(ErrorException("type Vector2 has no field " * string(symbol)))
        end
    end

    const Vector3f = Vector3{Cfloat}
    export Vector3f

    const Vector3i = Vector3{Cint}
    export Vector3i

    const Vector3ui = Vector3{Csize_t}
    export Vector3ui

    """
    # Vector4{T}

    ## Public Fields
    x::T
    y::T
    z::T
    """
    const Vector4{T} = SVector{4, T}
    export Vector4

    function Base.getproperty(v::Vector4{T}, symbol::Symbol) where T
        if symbol == :x
            return v[1]
        elseif symbol == :y
            return v[2]
        elseif symbol == :z
            return v[3]
        elseif symbol == :w
            return v[4]
        else
            throw(ErrorException("type Vector4 has no field " * string(symbol)))
        end
    end

    function Base.setproperty!(v::Vector2{T}, symbol::Symbol, value) where T
        if symbol == :x
            v[1] = convert(T, value)
        elseif symbol == :y
            v[2] = convert(T, value)
        elseif symbol == :z
            v[3] = convert(T, value)
        elseif symbol == :w
            v[4] = convert(T, value)
        else
            throw(ErrorException("type Vector2 has no field " * string(symbol)))
        end
    end

    const Vector4f = Vector4{Cfloat}
    export Vector4f

    const Vector4i = Vector4{Cint}
    export Vector4i

    const Vector4ui = Vector4{Csize_t}
    export Vector4ui

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

    ## TODO: JUMP

    macro add_signal_activate(x) return :(@add_signal $x activate Cvoid) end
    macro add_signal_shutdown(x) return :(@add_signal $x shutdown Cvoid) end
    macro add_signal_clicked(x) return :(@add_signal $x clicked Cvoid) end
    macro add_signal_toggled(x) return :(@add_signal $x toggled Cvoid) end
    macro add_signal_activate_default_widget(x) return :(@add_signal $x activate_default_widget Cvoid) end
    macro add_signal_activate_focused_widget(x) return :(@add_signal $x activate_focused_widget Cvoid) end
    macro add_signal_close_request(x) return :(@add_signal $x close_request WindowCloseRequestResult) end
    macro add_signal_items_changed(x) return :(@add_signal $x items_changed Cvoid Int32 position Int32 n_removed Int32 n_added) end
    macro add_signal_closed(x) return :(@add_signal $x closed Cvoid) end
    macro add_signal_text_changed(x) return :(@add_signal $x text_changed Cvoid) end
    macro add_signal_redo(x) return :(@add_signal $x redo Cvoid) end
    macro add_signal_undo(x) return :(@add_signal $x undo Cvoid) end
    macro add_signal_drag_begin(x) return :(@add_signal $x drag_begin Cvoid Cdouble start_x Cdouble start_y)
    macro add_signal_drag(x) return :(@add_signal $x drag Cvoid Cdouble offset_x Cdouble offset_y)
    macro add_signal_drag_end(x) return :(@add_signal $x drag_end Cvoid Cdouble offset_x Cdouble offset_y)
    macro add_signal_click_pressed(x) return :(@add_signal $x click_pressed Cvoid Cint n_press Cdouble x Cdouble y)
    macro add_signal_click_released(x) return :(@add_signal $x click_released Cvoid Cint n_press Cdouble x Cdouble y)
    macro add_signal_click_stopped(x) return :(@add_signal $x click_stopped Cvoid)
    macro add_signal_focus_gained(x) return :(@add_signal $x focus_gained Cvoid)
    macro add_signal_focus_lost(x) return :(@add_signal $x focus_lost Cvoid)
    macro add_signal_key_pressed(x) return :(@add_signal $x key_pressed Bool Cint key_value Cint key_code ModifierState modifier)        
    macro add_signal_key_released(x) return :(@add_signal $x key_released Bool Cint key_value Cint key_code ModifierState modifier)        
    macro add_signal_modifiers_changed(x) return :(@add_signal $x modifiers_changed Bool Cint key_value Cint key_code ModifierState modifier)        
    macro add_signal_pressed(x) return :(@add_signal $x pressed Cvoid Cdouble x Cdouble y)
    macro add_signal_press_cancelled(x) return :(@add_signal $x press_cancelled Cvoid)
    macro add_signal_motion_enter(x) return :(@add_signal $x motion_enter Cvoid Cdouble x Cdouble y)
    macro add_signal_motion(x) return :(@add_signal $x motion Cvoid Cdouble x Cdouble y)
    macro add_signal_motion_leave(x) return :(@add_signal $x motion_leave Cvoid)
    macro add_signal_scale_changed(x) return :(@add_signal $x scale_changed Cvoid Cdouble scale)
    macro add_signal_rotation_changed(x) return :(@add_signal $x rotation_changed Cvoid Cdouble angle_absolute_rads Cdouble angle_delta_rads)
    macro add_signal_scroll_decelerate(x) return :(@add_signal $x scroll_decelerate Cvoid Cdouble x_velocity Cdouble y_velocity)
    macro add_signal_scroll_begin(x) return :(@add_signal $x scroll_begin Cvoid)
    macro add_signal_scroll(x) return :(@add_signal $x scroll Bool Cdouble delta_x Cdouble delta_y)
    macro add_signal_scroll_end(x) return :(@add_signal $x scroll_end Cvoid)
    macro add_signal_stylus_up(x) return :(@add_signal $x stylus_up Cvoid Cdouble x Cdouble y)
    macro add_signal_stylus_down(x) return :(@add_signal $x stylus_down Cvoid Cdouble x Cdouble y)
    macro add_signal_proximity(x) return :(@add_signal $x proximity Cvoid Cdouble x Cdouble y)
    macro add_signal_swipe(x) return :(@add_signal $x swipe Cvoid Cdouble x_velocity Cdouble y_velocity)
    macro add_signal_pan(x) return :(@add_signal $x pan Cvoid PanDirection direction Cdouble offset)






####### types.jl

    abstract type SignalEmitter end
    export SignalEmitter

    abstract type Widget end
    export Widget

    abstract type EventController end
    export EventController

####### log.jl

    const LogDomain = String;
    export LogDomain

    macro debug(domain, message)
        @assert domain isa mousetrap.LogDomain
        @assert message isa String
        return :(mousetrap.detail.log_debug($message, $domain))
    end
    export debug

    macro info(domain, message)
        @assert domain isa mousetrap.LogDomain
        @assert message isa String
        return :(mousetrap.detail.log_info($message, $domain))
    end
    export info

    macro warning(domain, message)
        @assert domain isa mousetrap.LogDomain
        @assert message isa String
        return :(mousetrap.detail.log_warning($message, $domain))
    end
    export warning

    macro critical(domain, message)
        @assert domain isa mousetrap.LogDomain
        @assert message isa String
        return :(mousetrap.detail.log_critical($message, $domain))
    end
    export critical

    macro fatal(domain, message)
        @assert domain isa mousetrap.LogDomain
        @assert message isa String
        return :(mousetrap.detail.log_fatal($message, $domain))
    end
    export fatal

    set_surpress_debug(domain::LogDomain, b::Bool) = detail.log_set_surpress_debug(domain, b)
    export set_surpress_debug

    set_surpress_info(domain::LogDomain, b::Bool) = detail.log_set_surpress_info(domain, b)
    export set_surpress_info

    get_surpress_debug(dbut thomain::LogDomain) ::Bool = detail.log_get_surpress_debug(domain, b)
    export set_surpress_debug

    get_surpress_info(domain::LogDomain) ::Bool = detail.log_get_surpress_info(domain, b)
    export set_surpress_info

    set_log_file(path::String) = detail.log_set_file(path)
    export set_log_file

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

####### cursor_type.jl

    @export_enum CursorType begin
        CURSOR_TYPE_NONE
        CURSOR_TYPE_DEFAULT
        CURSOR_TYPE_HELP
        CURSOR_TYPE_POINTER
        CURSOR_TYPE_CONTEXT_MENU
        CURSOR_TYPE_PROGRESS
        CURSOR_TYPE_WAIT
        CURSOR_TYPE_CELL
        CURSOR_TYPE_CROSSHAIR
        CURSOR_TYPE_TEXT
        CURSOR_TYPE_MOVE
        CURSOR_TYPE_NOT_ALLOWED
        CURSOR_TYPE_GRAB
        CURSOR_TYPE_GRABBING
        CURSOR_TYPE_ALL_SCROLL
        CURSOR_TYPE_ZOOM_IN
        CURSOR_TYPE_ZOOM_OUT
        CURSOR_TYPE_COLUMN_RESIZE
        CURSOR_TYPE_ROW_RESIZE
        CURSOR_TYPE_NORTH_RESIZE
        CURSOR_TYPE_NORTH_EAST_RESIZE
        CURSOR_TYPE_EAST_RESIZE
        CURSOR_TYPE_SOUTH_EAST_RESIZE
        CURSOR_TYPE_SOUTH_RESIZE
        CURSOR_TYPE_SOUTH_WEST_RESIZE
        CURSOR_TYPE_WEST_RESIZE
        CURSOR_TYPE_NORTH_WEST_RESIZE
    end

    const CURSOR_TYPE_HORIZONTAL_RESIZE = CURSOR_TYPE_ROW_RESIZE
    const CURSOR_TYPE_VERTICAL_RESIZE = CURSOR_TYPE_COLUMN_RESIZE

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

####### blend_mode.jl

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

####### check_button.jl

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

####### entry.jl

    @export_type Entry Widget
    Entry() = Entry(detail._Entry())

    @export_function Entry get_text String
    @export_function Entry set_text! Cvoid text String
    @export_function Entry set_max_length! Cvoid n Integer
    @export_function Entry get_max_length Int64
    @export_function Entry set_has_frame! Cvoid b Bool
    @export_function Entry get_has_frame Bool
    @export_function Entry set_text_visible! Cvoid b Bool
    @export_function Entry get_text_visible Bool

    function set_primary_icon!(entry::Entry, icon::Icon)
        detail.set_primary_icon!(entry._internal, icon._internal)
    end
    export set_primary_icon!

    @export_function Entry remove_primary_icon! Cvoid

    function set_secondary_icon!(entry::Entry, icon::Icon)
        detail.set_secondary_icon!(entry._internal, icon._internal)
    end
    export set_secondary_icon!

    @export_function Entry remove_secondary_icon! Cvoid

    @add_widget_signals Entry
    @add_signal_activate Entry
    @add_signal_text_changed Entry

####### expander.jl

    @export_type Expander Widget
    Expander() = Expander(detail._Expander())

    function set_child!(expander::Expander, child::Widget)
        detail.set_child!(expander._internal, child._internal.cpp_object)
    end
    export set_child!

    @export_function Expander remove_child! Cvoid

    function set_label_widget!(expander::Expander, child::Widget)
        detail.set_label_widget!(expander._internal, child._internal.cpp_object)
    end
    export set_label_widget!

    @export_function Expander remove_label_widget! Cvoid

    @add_widget_signals Expander
    @add_signal_activate Expander

####### fixed.jl

    @export_type Fixed Widget
    Fixed() = Fixed(detail._Fixed())

    add_child!(fixed::Fixed, child::Widget, position::Vector2f) = detail.add_child!(fixed._internal, child._internal.cpp_object, position)
    export add_child!

    remove_child!(fixed::Fixed, child::Widget) = detail.remove_child!(fixed._internal, child._internal.cpp_object)
    export remove_child!

    set_child_position!(fixed::Fixed, child::Widget, position::Vector2f) = detail.set_child_position!(fixed._internal, child._internal.cpp_object, position)
    export set_child_position!

    get_child_position(fixed::Fixed, child::Widget) ::Vector2f = detail.get_child_position(fixed._internal, child._internal.cpp_object)
    export get_child_position!

    @add_widget_signals Fixed

####### level_bar.jl

    @export_enum LevelBarMode begin
        LEVEL_BAR_MODE_CONTINUOUS
        LEVEL_BAR_MODE_DISCRETE
    end

    @export_type LevelBar Widget
    LevelBar(min::AbstractFloat, max::AbstractFloat) = LevelBar(detail._internal(min, max))

    @export_function LevelBar add_marker! Cvoid name String value AbstractFloat
    @export_function LevelBar remove_mark! Cvoid name String value AbstractFloat
    @export_function LevelBar set_inverted! Cvoid b Bool
    @export_function LevelBar get_inverted Bool
    @export_function LevelBar set_mode! Cvoid mode LevelBarMode
    @export_function LevelBar get_mode LevelBarMode
    @export_function LevelBar set_min_value! Cvoid value AbstractFloat
    @export_function LevelBar get_min_value Cfloat
    @export_function LevelBar set_max_value! Cvoid value AbstractFloat
    @export_function LevelBar set_value! Cvoid value AbstractFloat
    @export_function LevelBar get_value Cfloat
    @export_function LevelBar set_orientation! Cvoid orientation Orientation
    @export_function LevelBar get_orientation Orientation

    @add_widget_signals LevelBar

####### label.jl

    @export_enum JustifyMode begin
        JUSTIFY_MODE_LEFT
        JUSTIFY_MODE_RIGHT
        JUSTIFY_MODE_CENTER
        JUSTIFY_MODE_FILL
    end

    @export_enum EllipsizeMode begin
        ELLIPSIZE_MODE_NONE
        ELLIPSIZE_MODE_START
        ELLIPSIZE_MODE_MIDDLE
        ELLIPSIZE_MODE_END
    end

    @export_enum LabelWrapMode begin
        LABEL_WRAP_MODE_NONE
        LABEL_WRAP_MODE_ONLY_ON_WORD
        LABEL_WRAP_MODE_ONLY_ON_CHAR
        LABEL_WRAP_MODE_WORD_OR_CHAR
    end

    @export_type Label Widget
    Label() = Label(detail._Label())
    Label(formatted_string::String) = Label(detail._Label(formatted_string))

    @export_function Label set_text! Cvoid text String
    @export_function Label get_text String
    @export_function Label set_use_markup! Cvoid b Bool
    @export_function Label get_use_markup Bool
    @export_function Label set_ellipsize_mode! Cvoid mode EllipsizeMode
    @export_function Label get_ellipsize_mode EllipsizeMode
    @export_function Label set_wrap_mode mode! Cvoid LabelWrapMode
    @export_function Label get_wrap_mode LabelWrapMode
    @export_function Label set_justify_mode! mode Cvoid JustifyMode
    @export_function Label get_justify_mode JustifyMode
    @export_function Label set_max_width_chars! Cvoid n Unsigned
    @export_function Label get_max_width_chars UInt64
    @export_function Label set_x_alignment! Cvoid x AbstractFloat
    @export_function Label get_x_alignment Cfloat
    @export_function Label set_y_alignment! Cvoid x AbstractFloat
    @export_function Label get_y_alignment Cfloat
    @export_function Label set_selectable! Cvoid b Bool
    @export_function Label get_selectable Bool

    @add_widget_signals Label

####### text_view.jl

    @export_type TextView Widget

    TextView() = TextView(detail._TextView())

    @export_function TextView get_text String
    @export_function TextView set_text! Cvoid text String
    @export_function TextView set_cursor_visible! Cvoid b Bool
    @export_function TextView get_cursor_visible Bool
    @export_function TextView undo! Cvoid
    @export_function TextView redo! Cvoid
    @export_function TextView set_was_modified! Cvoid b Bool
    @export_function TextView get_was_modified Bool
    @export_function TextView set_editable! Cvoid b Bool
    @export_function TextView get_editable Bool
    @export_function TextView set_justify_mode! Cvoid mode JustifyMode
    @export_function TextView get_justify_mode JustifyMode
    @export_function TextView set_left_margin! Cvoid margin AbstractFloat
    @export_function TextView get_left_margin Cfloat
    @export_function TextView set_right_margin! Cvoid margin AbstractFloat
    @export_function TextView get_right_margin Cfloat
    @export_function TextView set_left_margin! Cvoid margin AbstractFloat
    @export_function TextView get_top_margin Cfloat
    @export_function TextView set_left_margin! Cvoid margin AbstractFloat
    @export_function TextView get_top_margin Cfloat
    @export_function TextView set_bottom_margin! Cvoid margin AbstractFloat
    @export_function TextView get_bottom_margin Cfloat

    @add_widget_signals TextView
    @add_signal_undo TextView
    @add_signal_redo TextView
    @add_signal_text_changed TextView

####### frame.jl

    @export_type Frame
    Frame() = Frame(detail._Frame())

    set_child!(frame::Frame, child::Widget) = detail.set_child!(fixed._internal, child._internal.cpp_object)
    export set_child!

    set_label_widget!(frame::Frame, label::Widget) = detail.set_label_widget!(frame._internal, label._internal.cpp_object)
    export set_label_widget!

    @export_function Frame remove_child! Cvoid
    @export_function Frame remove_label_widget! Cvoid
    @export_function Frame set_label_x_alignment! Cvoid x AbstractFloat
    @export_function Frame get_label_x_alignment! Cfloat

    @add_widget_signals Frame

####### overlay.jl

    @export_type Overlay Widget
    Overlay() = Overlay(detail._Overlay())

    set_child!(overlay::Overlay, child::Widget) = detail.set_child!(overlay._internal, child._internal.cpp_object)
    export set_child!

    function add_overlay!(overlay::Overlay, child::Widget; include_in_measurement::Bool = true, clip::Bool = true)
        detail.add_overlay!(overlay._internal, child._internal.cpp_object, include_in_measurement, clip)
    end
    export add_overlay!

    remove_overlay!(overlay::Overlay, child::Widget) = detail.remove_overlay(overlay._internal, child._internal.cpp_object)
    export remove_overlay!

    @add_widget_signals Overlay

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

###### event_controller.jl

    abstract type EventController end
    export EventController

    abstract type SingleClickGesture <: EventController end
    export SingleClickGesture

    @export_enum PropagationPhase begin
        PROPAGATION_PHASE_NONE
        PROPAGATION_PHASE_CAPTURE
        PROPAGATION_PHASE_BUBBLE
        PROPAGATION_PHASE_TARGET
    end

    set_propagation_phase!(controller::EventController) = detail.set_propagation_phase!(controller._internal.cpp_object)
    export set_propagation_phase!

    get_propagation_phase(controller::EventController) = return detail.get_propagation_phase(controller._internal.cpp_object)
    export get_propagation_phase

    @export_enum ButtonID begin
        BUTTON_ID_NONE
        BUTTON_ID_ANY
        BUTTON_ID_01
        BUTTON_ID_02
        BUTTON_ID_03
        BUTTON_ID_04
        BUTTON_ID_05
        BUTTON_ID_06
        BUTTON_ID_07
        BUTTON_ID_08
        BUTTON_ID_09
    end

    get_current_button(gesture::SingleClickGesture) ::ButtonID = return detail.get_current_button(gesture._internal.cpp_object)
    export get_current_button

    set_only_listens_to_button!(gesture::SingleClickGesture, button::ButtonID) = detail.set_only_listens_to_button!(gesture._internal.cpp_object) 
    export set_only_listens_to_button

    get_only_listens_to_button(gesture::SingleClickGesture) = return detail.get_only_listens_to_button(gesture._internal.cpp_object)
    export get_only_listens_to_button

    set_touch_only!(gesture::SingleClickGesture) = detail.set_touch_only!(gesture._internal.cpp_object)
    export set_touch_only!

    get_touch_only(gesture::SingleClickGesture) = detail.get_touch_only(gesture._internal.cpp_object)
    export get_touch_only

###### drag_event_controller.jl

    @export_type DragEventController SingleClickGesture
    DragEventController() = DragEventController(detail._DragEventController())

    get_start_position(controller::DragEventController) = return detail.get_start_position(controller._internal)
    export get_start_position

    get_current_offset(controller::DragEventController) = return detail.get_current_offset(controller._internal)
    export get_current_offset

    @add_signal_drag_begin DragEventController
    @add_signal_drag DragEventController
    @add_signal_drag_end DragEventController

###### click_event_controller.jl

    @export_type ClickEventController SingleClickGesture
    ClickEventController() = ClickEventController(detail._ClickEventController())

    @add_signal_click_pressed ClickEventController
    @add_signal_click_released ClickEventController
    @add_signal_click_stopped ClickEventController

###### focus_event_controller.jl

    @export_type FocusEventController EventController
    FocusEventController() = FocusEventController(detail._FocusEventController())

    @export_function FocusEventController self_or_child_is_focused Bool
    @export_function FocusEventController self_is_focused Bool

    @add_signal_focus_gained FocusEventController
    @add_signal_focus_lost FocusEventController

###### key_event_controller.jl

    @export_type KeyEventController EventController
    KeyEventController() = KeyEventController(detail._KeyEventController())

    @export_function KeyEventController Bool trigger String

    const ModifierState = detail._ModifierState
    export ModifierState

    const KeyCode = Cint
    export KeyCode

    const KeyValue = Cint
    export KeyValue

    @add_signal_key_pressed KeyEventController
    @add_signal_key_released KeyEventController
    @add_signal_modifiers_changed KeyEventController

    shift_pressed(modifier_state::ModifierState) ::Bool = return detail.shift_pressed(modifier_state);
    export shift_pressed

    control_pressed(modifier_state::ModifierState) ::Bool = return detail.control_pressed(modifier_state);
    export shift_pressed

    alt_pressed(modifier_state::ModifierState) ::Bool = return detail.alt_pressed(modifier_state);
    export alt_pressed

    shift_pressed(modifier_state::ModifierState) ::Bool = return detail.shift_pressed(modifier_state);
    export shift_pressed

    mouse_button_01_pressed(modifier_state::ModifierState) ::Bool = return detail.mouse_button_01_pressed(modifier_state);
    export shift_mouse_button_01_pressedpressed

    mouse_button_02_pressed(modifier_state::ModifierState) ::Bool = return detail.mouse_button_02_pressed(modifier_state);
    export mouse_button_02_pressed

###### long_press_event_controller.jl

    @export_type LongPressEventController SingleClickGesture
    LongPressEventController() = LongPressEventController(detail._LongPressEventController())

    @export_function LongPressEventController set_delay_factor Cvoid factor AbstractFloat
    @export_function LongPressEventController get_delay_factor Cfloat

    @add_signal_pressed LongPressEventController
    @add_signal_press_cancelled LongPressEventController

###### motion_event_controller.jl

    @export_type MotionEventController EventController
    MotionEventController() = MotionEventController(detail._MotionEventController())

    @add_signal_motion_enter MotionEventController
    @add_signal_motion MotionEventController
    @add_signal_motion_leave MotionEventController

###### pinch_zoom_event_controller.jl

    @export_type PinchZoomEventController EventController
    PinchZoomEventController() = PinchZoomEventController(detail._PinchZoomEventController())

    @export_function PinchZoomEventController get_scale_delta Cfloat

    @add_signal_scale_changed PinchZoomEventController

###### rotate_event_controller.jl

    @export_type RotateEventController EventController
    RotateEventController() = RotateEventController(detail._RotateEventController())

    get_angle_delta(controller::RotateEventController) ::Angle = return radians(detail.get_angle_delta(controller._internal))

    @add_signal_rotation_changed RotateEventController
    
###### scroll_event_controller.jl

    @export_type ScrollEventController EventController
    ScrollEventController(; emit_vertical = true, emit_horizontal = true) = ScrollEventController(detail._ScrollEventController(emit_vertical, emit_horizontal))

    @add_signal_kinetic_scroll ScrollEventController
    @add_signal_scroll_begin ScrollEventController
    @add_signal_scroll ScrollEventController
    @add_signal_scroll_end ScrollEventController

###### shortcut_event_controller.jl

    @export_type ShortcutEventController EventController
    ShortcutEventController() = ShortcutEventController(detail._ShortcutEventController())

    add_action!(shortcut_controller::ShortcutEventController, action::Action) = detail.add_action!(shortcut_controller._internal, action._internal)
    export add_action!

    @export_enum ShortcutScope begin
        SHORTCUT_SCOPE_LOCAL
        SHORTCUT_SCOPE_MANAGED
        SHORTCUT_SCOPE_GLOBAL
    end

    set_scope!(controller::ShortcutController, scope::ShortcutScope) = detail.set_scope!(controller._internal, scope)
    export set_scope!

    get_scope(controller::ShortcutController) ::ShortcutScope = return detail.get_scope(controller._internal)
    export get_scope

###### stylus_event_controller.jl

    @export_enum ToolType begin
        TOOL_TYPE_UNKNOWN
        TOOL_TYPE_PEN
        TOOL_TYPE_ERASER
        TOOL_TYPE_BRUSH
        TOOL_TYPE_PENCIL
        TOOL_TYPE_AIRBRUSH
        TOOL_TYPE_LENS
        TOOL_TYPE_MOUSE
    end

    @export_enum DeviceAxis begin
        DEVICE_AXIS_X
        DEVICE_AXIS_Y
        DEVICE_AXIS_DELTA_X
        DEVICE_AXIS_DELTA_Y
        DEVICE_AXIS_PRESSURE
        DEVICE_AXIS_X_TILT
        DEVICE_AXIS_Y_TILT
        DEVICE_AXIS_WHEEL
        DEVICE_AXIS_DISTANCE
        DEVICE_AXIS_ROTATION
        DEVICE_AXIS_SLIDER
    end

    device_axis_to_string(axis::DeviceAxis) ::String = detail.device_axis_to_string(axis)
    export device_axis_to_string

    @export_type StylusEventController SingleClickGesture
    StylusEventController() = StylusEventController(detail._StylusEventController())

    @export_function StylusEventController get_hardware_id Csize_t
    @export_function StylusEventController get_tool_type ToolType
    @export_function StylusEventController has_axis Bool DeviceAxis axis

    @add_signal_stylus_up StylusEventController
    @add_signal_stylus_down StylusEventController
    @add_signal_proximity StylusEventController
    @add_signal_motion StylusEventController

###### swipe_event_controller.jl

    @export_type SwipeEventController SingleClickGesture
    SwipeEventController(orientation::Orientation) = SwipeEventController(detail._SwipeEventController(orientation))

    get_velocity(swipe_controller::SwipeEventController) ::Vector2f = return detail.get_velocity(swipe_controller._internal)
    export get_velocity

    @add_signal_swipe SwipeEventController

###### pan_event_controller.jl

    @export_enum PanDirection begin
        PAN_DIRECTION_LEFT
        PAN_DIRECTION_RIGHT
        PAN_DIRECTION_TOP
        PAN_DIRECTION_BOTTOM
    end

    @export_type PanEventController SingleClickGesture
    PanEventController(orientation::Orientation) = PanEventController(detail._PanEventController(orientation))

    set_orientation!(pan_controller::PanEventController) = detail.set_orientation!(pan_controller._internal)
    export set_orientation!

    get_orientation(pan_controller::PanEventController) ::Orientation = detail.get_orientation(pan_controller._internal)
    export get_orientation

    @add_signal_pan PanEventController

###### menubar.jl

    @export_type MenuBar Widget
    MenuBar(model::MenuModel) = MenuBar(detail._MenuBar(model._internal))

    @add_widget_signals MenuBar

###### key_code.jl

    # TODO

end # module mousetrap

using .mousetrap

using CxxWrap

app = Application("example.app")

connect_signal_activate!(app) do app::Application

    window = Window(app)

    label = TextView()
    set_text!(label, "abc\ndef")

    set_child!(window, label)
    present!(window)
end

exit(run(app))




