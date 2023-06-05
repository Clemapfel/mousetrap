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

    macro document(name, string)
        :(@doc $string $name)
    end

    macro export_enum(enum)
        out = Expr(:block)
        for instance in instances(eval(enum))
            push!(out.args, :(export $(Symbol(instance))))
        end

        push!(out.args, :(Base.convert(type::Type{$enum}, x::Integer) = return $enum(Base.convert(Int64, x))))
        return out
    end

    macro _export_function_arg0(type, name)
        mousetrap.eval(:(export $name))
        return :($name(x::$type) = detail.$name(x._internal))
    end

    macro _export_function_arg1(type, name, arg1_name, arg1_type)
        mousetrap.eval(:(export $name))
        return :($name(x::$type, $arg1_name::$arg1_type) = detail.$name(x._internal, $arg1_name))
    end

    macro _export_function_arg2(type, name, arg1_name, arg1_type, arg2_name, arg2_type)
        mousetrap.eval(:(export $name))
        return :($name(
            x::$type,
            $arg1_name::$arg1_type,
            $arg2_name::$arg2_type
        ) = detail.$name(x._internal, $arg1_name, $arg2_name))
    end

    macro _export_function_arg3(type, name, arg1_name, arg1_type, arg2_name, arg2_type, arg3_name, arg3_type)
        mousetrap.eval(:(export $name))
        return :($name(
            x::$type,
            $arg1_name::$arg1_type,
            $arg2_name::$arg2_type,
            $arg3_name::$arg3_type
        ) = detail.$name(x._internal, $arg1_name, $arg2_name, $arg3_name))
    end

    macro _export_function_arg4(type, name, arg1_name, arg1_type, arg2_name, arg2_type, arg3_name, arg3_type)
        mousetrap.eval(:(export $name))
        return :($name(
            x::$type,
            $arg1_name::$arg1_type,
            $arg2_name::$arg2_type,
            $arg3_name::$arg3_type,
            $arg4_name::$arg4_type
        ) = detail.$name(x._internal, $arg1_name, $arg2_name, $arg3_name, $arg4_name))
    end

    macro export_function(type, name, args...)

        @assert (length(args) == 0) || (length(args) % 2 == 0)

        if length(args) == 0
            return :(@_export_function_arg0($type, $name))
        elseif length(args) == 2
            return :(@_export_function_arg1($type, $name, $(args[1]), $(args[2])))
        elseif length(args) == 4
            return :(@_export_function_arg2($type, $name, $(args[1]), $(args[2]), $(args[3]), $(args[4])))
        elseif length(args) == 6
            return :(@_export_function_arg3($type, $name, $(args[1]), $(args[2]), $(args[3]), $(args[4]), $(args[5]), $(args[6])))
        elseif length(args) == 8
            return :(@_export_function_arg4($type, $name, $(args[1]), $(args[2]), $(args[3]), $(args[4]), $(args[5]), $(args[6]), $(args[7]), $(args[8])))
        else
            throw(AssertionError("In moustrap.export_function: Incompatible number of arguments"))
            return :()
        end
    end

    macro export_signal_emitter(name)
        mousetrap.eval(:(export $name))
        internal_name = Symbol("_" * "$name")
        return quote
            struct $name <: SignalEmitter
                _internal::detail.$internal_name
            end
        end
    end

    macro export_widget(name)
        mousetrap.eval(:(export $name))
        internal_name = Symbol("_" * "$name")
        return quote
            struct $name <: Widget
                _internal::detail.$internal_name
            end
        end
    end

    macro export_event_controller(name)
        mousetrap.eval(:(export $name))
        internal_name = Symbol("_" * "$name")
        return quote
            struct $name <: EventController
                _internal::detail.$internal_name
            end
        end
    end

    import Base: *
    *(x::Symbol, y::Symbol) = return Symbol(string(x) * string(y))

    import Base: clamp
    clamp(x::AbstractFloat, lower::AbstractFloat, upper::AbstractFloat) = if x < lower return lower elseif x > upper return upper else return x end

####### log.jl

    const LogDomain = String;
    export LogDomain

    const USER_DOMAIN = "julia"

    macro debug(log_domain, message)
        :(mousetrap.detail.log_debug($(string(log_domain)), $(string(message))))
    end
    macro debug(message)
        :(mousetrap.detail.log_debug($(string(mousetrap.USER_DOMAIN)), $(string(message))))
    end
    export debug

    macro info(log_domain, message)
        :(mousetrap.detail.log_info($(string(log_domain)), $(string(message))))
    end
    macro info(message)
        :(mousetrap.detail.log_info($(string(mousetrap.USER_DOMAIN)), $(string(message))))
    end
    export info

    macro warning(log_domain, message)
        :(mousetrap.detail.log_warning($(string(log_domain)), $(string(message))))
    end
    macro warning(message)
        :(mousetrap.detail.log_warning($(string(mousetrap.USER_DOMAIN)), $(string(message))))
    end
    export warning

    macro critical(log_domain, message)
        :(mousetrap.detail.log_critical($(string(log_domain)), $(string(message))))
    end
    macro critical(message)
        :(mousetrap.detail.log_critical($(string(mousetrap.USER_DOMAIN)), $(string(message))))
    end
    export critical

    macro fatal(log_domain, message)
        :(mousetrap.detail.log_fatal($(string(log_domain)), $(string(message))))
    end
    macro fatal(message)
        :(mousetrap.detail.log_fatal($(string(mousetrap.USER_DOMAIN)), $(string(message))))
    end
    export fatal

    function log_set_surpress_debug(domain::LogDomain, b::Bool)
        detail.log_set_surpress_debug(domain, b)
    end
    export log_set_surpress_debug

    function log_set_surpress_info(domain::LogDomain, b::Bool)
        detail.log_set_surpress_info(domain, b)
    end
    export log_set_surpress_info

    function log_set_file(path::String) ::Bool
        return detail.log_set_file(path)
    end
    export log_set_file

####### forward_declarations.jl

    abstract type SignalEmitter end
    export SignalEmitter

    abstract type Widget end
    export Widget

    abstract type EventController end;
    export EventController

    @export_signal_emitter Application
    @export_signal_emitter Action
    @export_signal_emitter FrameClock
    @export_signal_emitter Adjustment

    @export_event_controller MotionEventController

    @export_widget AspectFrame
    @export_widget Box
    @export_widget Button
    @export_widget CenterBox
    @export_widget CheckButton
    @export_widget Window

####### signal_components.jl

    macro _add_signal_arg0(T, snake_case, Return_t)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case

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

        disconnect_signal_name = :disconnect_signal_ * snake_case

        push!(out.args, esc(:(
            function $disconnect_signal_name(x::$T)
                detail.$disconnect_signal_name(x._internal)
            end
        )))

        set_signal_blocked_name = :set_signal_ * snake_case * :_blocked

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

    macro _add_signal_arg1(T, snake_case, Return_t, Arg1_t, arg1_name)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case

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

        disconnect_signal_name = :disconnect_signal_ * snake_case

        push!(out.args, esc(:(
            function $disconnect_signal_name(x::$T)
                detail.$disconnect_signal_name(x._internal)
            end
        )))

        set_signal_blocked_name = :set_signal_ * snake_case * :_blocked

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

    macro _add_signal_arg2(T, snake_case, Return_t, Arg1_t, arg1_name, Arg2_t, arg2_name)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case

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

        disconnect_signal_name = :disconnect_signal_ * snake_case

        push!(out.args, esc(:(
            function $disconnect_signal_name(x::$T)
                detail.$disconnect_signal_name(x._internal)
            end
        )))

        set_signal_blocked_name = :set_signal_ * snake_case * :_blocked

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

    macro _add_signal_arg3(T, snake_case, Return_t, Arg1_t, arg1_name, Arg2_t, arg2_name, Arg3_t, arg3_name)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case

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

        disconnect_signal_name = :disconnect_signal_ * snake_case

        push!(out.args, esc(:(
            function $disconnect_signal_name(x::$T)
                detail.$disconnect_signal_name(x._internal)
            end
        )))

        set_signal_blocked_name = :set_signal_ * snake_case * :_blocked

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

    macro _add_signal_arg4(T, snake_case, Return_t, Arg1_t, arg1_name, Arg2_t, arg2_name, Arg3_t, arg3_name, Arg4_t, arg4_name)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case

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

        disconnect_signal_name = :disconnect_signal_ * snake_case

        push!(out.args, esc(:(
            function $disconnect_signal_name(x::$T)
                detail.$disconnect_signal_name(x._internal)
            end
        )))

        set_signal_blocked_name = :set_signal_ * snake_case * :_blocked

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

    macro add_signal(T, name, return_t, args...)
        if length(args) == 0
            return :(@_add_signal_arg0($T, $name, $return_t))
        elseif length(args) == 2
            return :(@_add_signal_arg1($T, $name, $return_t, $(args[1]), $(args[2])))
        elseif length(args) == 4
            return :(@_add_signal_arg2($T, $name, $return_t, $(args[1]), $(args[2]), $(args[3]), $(args[4])))
        elseif length(args) == 6
            return :(@_add_signal_arg3($T, $name, $return_t, $(args[1]), $(args[2]), $(args[3]), $(args[4]), $(args[5]), $(args[6])))
        elseif length(args) == 8
            return :(@_add_signal_arg4($T, $name, $return_t, $(args[1]), $(args[2]), $(args[3]), $(args[4]), $(args[5]), $(args[6]), $(args[7]), $(args[8])))
        else
            throw(AssertionError("In moustrap.add_signal: Incompatible number of arguments"))
            return :()
        end
    end

    macro add_widget_signals(T)
        out = Expr(:block)
        push!(out.args, :(@add_signal $T realize Cvoid))
        push!(out.args, :(@add_signal $T unrealize Cvoid))
        push!(out.args, :(@add_signal $T destroy Cvoid))
        push!(out.args, :(@add_signal $T hide Cvoid))
        push!(out.args, :(@add_signal $T show Cvoid))
        push!(out.args, :(@add_signal $T map Cvoid))
        push!(out.args, :(@add_signal $T unmap Cvoid))
        return out
    end

    # TODO: documentation for all signal-related methods


####### widget.jl

    macro define_set_margin(which)
        name = Symbol("set_margin_" * string(which))
        mousetrap.eval(:(export $name))
        return :(
            function $name(x::Widget, margin::Number)
                detail.$name(x._internal.cpp_object, convert(Cfloat, margin))
            end
        )
    end

    @define_set_margin(top)
    @define_set_margin(bottom)
    @define_set_margin(left)
    @define_set_margin(right)
    @define_set_margin(horizontal)
    @define_set_margin(vertical)
    @define_set_margin(Symbol(""))
 
    get_margin_top(widget) = return detail.get_margin_top(widget._internal.cpp_object)
    export get_margin_top
 
    get_margin_bottom(widget) = return detail.get_margin_top(widget._internal.cpp_object)
    export get_margin_bottom

    get_margin_start(widget) = return detail.get_margin_top(widget._internal.cpp_object)
    export get_margin_start

    get_margin_end(widget) = return detail.get_margin_top(widget._internal.cpp_object)
    export get_margin_end

    function set_expand_horizontally(widget::Widget, b::Bool)
        detail.set_expand_horizontally(widget._internal.cpp_object, b)
    end
    export set_expand_horizontally
 
    function set_expand_vertically(widget::Widget, b::Bool)
        detail.set_expand_vertically(widget._internal.cpp_object, b)
    end
    export set_expand_vertically
 
    function set_expand(widget::Widget, b::Bool)
        detail.set_expand(widget._internal.cpp_object, b)
    end
    export set_expand
 
    function get_expand_horizontally(widget::Widget, b::Bool)
        return detail.get_expand_horizontally(widget._internal.cpp_object)
    end
    export set_expand_horizontally
 
    function set_expand_vertically(widget::Widget, b::Bool)
        return detail.set_expand_vertically(widget._internal.cpp_object)
    end
    export set_expand_vertically

    @enum Alignment begin
        ALIGNMENT_START = detail.ALIGNMENT_START
        ALIGNMENT_CENTER = detail.ALIGNMENT_CENTER
        ALIGNMENT_END = detail.ALIGNMENT_END
    end
    @export_enum Alignment

    function set_horizontal_alignment(widget::Widget, alignment::Alignment)
        detail.set_horizontal_alignment(widget._internal.cpp_object, Cint(alignment))
    end
    export set_horizontal_alignment

    function set_vertical_alignment(widget::Widget, alignment::Alignment)
        detail.set_vertical_alignment(widget._internal.cpp_object, Cint(alignment))
    end
    export set_vertical_alignment

    function set_alignment(widget::Widget, alignment::Alignment)
        detail.set_alignment(widget._internal.cpp_object, Cint(alignment))
    end
    export set_alignment
 
    get_vertical_alignment(widget::Widget) = return Alignment(detail.get_vertical_alignment(widget._internal.cpp_object))
    export get_vertical_alignment
 
    get_vertical_alignment(widget::Widget) = return Alignment(detail.get_vertical_alignment(widget._internal.cpp_object))
    export get_horizontal_alignment

    set_opacity(widget::Widget, x::Number) = detail.set_opacity(widget._internal.cpp_object, Cfloat(clamp(x, 0, 1)))
    export set_opacity
 
    get_opacity(widget::Widget) = return detail.get_opacity(widget._internal.cpp_object)
    export get_opacity
 
    get_is_visible(widget::Widget) = return detail.get_is_visible(widget._internal.cpp_object)
    export get_is_visible
 
    set_is_visible(widget::Widget, b::Bool) = detail.set_is_visible(widget._internal.cpp_object, b)
    export set_is_visible
 
    set_tooltip_text(widget::Widget, text::String) = detail.set_tooltip_text(widget._internal.cpp_object, text)
    export set_tooltip_text
 
    set_tooltip_widget(parent::Widget, tooltip::Widget) = detail.set_tooltip_widget(parent._internal.cpp_object, tooltip._internal.cpp_object)
    export set_tooltip_widget
 
    remove_tooltip_widget(widget::Widget) = detail.remove_tooltip_widget(parent._internal.cpp_object)
    export remove_tooltip_widget

    @enum CursorType begin
        CURSOR_TYPE_NONE = detail.CURSOR_TYPE_NONE
        CURSOR_TYPE_DEFAULT = detail.CURSOR_TYPE_DEFAULT
        CURSOR_TYPE_HELP = detail.CURSOR_TYPE_HELP
        CURSOR_TYPE_POINTER = detail.CURSOR_TYPE_POINTER
        CURSOR_TYPE_CONTEXT_MENU = detail.CURSOR_TYPE_CONTEXT_MENU
        CURSOR_TYPE_PROGRESS = detail.CURSOR_TYPE_PROGRESS
        CURSOR_TYPE_WAIT = detail.CURSOR_TYPE_WAIT
        CURSOR_TYPE_CELL = detail.CURSOR_TYPE_CELL
        CURSOR_TYPE_CROSSHAIR = detail.CURSOR_TYPE_CROSSHAIR
        CURSOR_TYPE_TEXT = detail.CURSOR_TYPE_TEXT
        CURSOR_TYPE_MOVE = detail.CURSOR_TYPE_MOVE
        CURSOR_TYPE_NOT_ALLOWED = detail.CURSOR_TYPE_NOT_ALLOWED
        CURSOR_TYPE_GRAB = detail.CURSOR_TYPE_GRAB
        CURSOR_TYPE_GRABBING = detail.CURSOR_TYPE_GRABBING
        CURSOR_TYPE_ALL_SCROLL = detail.CURSOR_TYPE_ALL_SCROLL
        CURSOR_TYPE_ZOOM_IN = detail.CURSOR_TYPE_ZOOM_IN
        CURSOR_TYPE_ZOOM_OUT = detail.CURSOR_TYPE_ZOOM_OUT
        CURSOR_TYPE_COLUMN_RESIZE = detail.CURSOR_TYPE_COLUMN_RESIZE
        CURSOR_TYPE_ROW_RESIZE = detail.CURSOR_TYPE_ROW_RESIZE
        CURSOR_TYPE_NORTH_RESIZE = detail.CURSOR_TYPE_NORTH_RESIZE
        CURSOR_TYPE_NORTH_EAST_RESIZE = detail.CURSOR_TYPE_NORTH_EAST_RESIZE
        CURSOR_TYPE_EAST_RESIZE = detail.CURSOR_TYPE_EAST_RESIZE
        CURSOR_TYPE_SOUTH_EAST_RESIZE = detail.CURSOR_TYPE_SOUTH_EAST_RESIZE
        CURSOR_TYPE_SOUTH_RESIZE = detail.CURSOR_TYPE_SOUTH_RESIZE
        CURSOR_TYPE_SOUTH_WEST_RESIZE = detail.CURSOR_TYPE_SOUTH_WEST_RESIZE
        CURSOR_TYPE_WEST_RESIZE = detail.CURSOR_TYPE_WEST_RESIZE
        CURSOR_TYPE_NORTH_WEST_RESIZE = detail.CURSOR_TYPE_NORTH_WEST_RESIZE
    end
    @export_enum CursorType

    set_cursor(widget::Widget, cursor_type::CursorType) = detail.set_cursor(widget._internal.cpp_object, Cint(cursor_type))
    export set_cursor

    # TODO: set_cursor_from_image(widget::Widget, image::Image, alignment::Vector2f)
    export set_cursor_from_image

    hide(widget::Widget) = detail.hide(widget._internal.cpp_object)
    export hide

    show(widget::Widget) = detail.hide(widget._internal.cpp_object)
    export show

    add_controller(widget::Widget, controller::EventController) = detail.add_controller(widget._internal.cpp_object, controller._internal.cpp_object)
    export add_controller

    remove_controller(widget::Widget, controller::EventController) = detail.remove_controller(widget._internal.cpp_object, controller._internal.cpp_object)
    export remove_controller

    set_is_focusable(widget::Widget, b::Bool) = detail.set_is_focusable(widget._internal.cpp_object, b)
    export set_is_focusable

    get_is_focusable(widget::Widget) = return detail.get_is_focusable(widget._internal.cpp_object)
    export get_is_focusable

    grab_focus(widget::Widget) = detail.grab_focus(widget._internal.cpp_object)
    export grab_focus

    get_has_focus(widget::Widget) = detail.get_has_focus(widget._internal.cpp_object)
    export get_has_focus

    set_focus_on_click(widget::Widget, b::Bool) = detail.set_focus_on_click(widget._internal.cpp_object, b)
    export set_focus_on_click

    get_focus_on_click(widget::Widget) = return detail.get_focus_on_click(widget._internal.cpp_object)
    export get_focus_on_click

    get_is_realized(widget::Widget) = return detail.get_is_realized(widget._internal.cpp_object)
    export get_is_realized

    get_minimum_size(widget::Widget) ::Vector2f = return detail.get_minimum_size(widget._internal.cpp_object)
    export get_minimum_size

    get_natural_size(widget::Widget) ::Vector2f = return detail.get_natural_size(widget._internal.cpp_object)
    export get_natural_size

    get_position(widget::Widget) ::Vector2f = return detail.get_position(widget._internal.cpp_object)
    export get_position

    get_allocated_size(widget::Widget) ::Vector2f = return detail.get_allocated_size(widget._internal.cpp_object)
    export get_allocated_size

    unparent(widget::Widget) = detail.unparent(widget._internal.cpp_object)
    export unparent

    set_can_respond_to_input(widget::Widget, b::Bool) = detail.set_can_respond_to_input(widget._internal.cpp_object, b)
    export set_can_respond_to_input

    get_can_respond_to_input(widget::Widget) = return detail.get_can_respond_to_input(widget._internal.cpp_object)
    export get_can_respond_to_input

    set_hide_on_overflow(widget::Widget, b::Bool) = detail.set_hide_on_overflow(widget._internal.cpp_object, b)
    export set_hide_on_overflow

    get_hide_on_overflow(widget::Widget) = return detail.get_hide_on_overflow(widget._internal.cpp_object)
    export get_hide_on_overflow

    # TODO get_clipboard(widget::Widget) ::Clipboard = Clipboard(detail.get_clipboard(widget._internal.cpp_object))
    export get_clipboard

####### tick_callback.jl

    using Dates

    function get_frame_clock(widget::Widget)
        return FrameClock(detail.get_frame_clock(widget._internal.cpp_object))
    end
    export get_frame_clock

    function get_frame_time(clock::FrameClock) ::Dates.Microsecond
        return Dates.Microsecond(detail.get_frame_time(clock._internal))
    end
    export get_frame_time

    function get_time_since_last_frame(clock::FrameClock) ::Dates.Microsecond
        return Dates.Microsecond(detail.get_time_since_last_frame(clock._internal))
    end
    export get_time_since_last_frame

    @enum TickCallbackResult begin
        TICK_CALLBACK_RESULT_CONTINUE = detail.TICK_CALLBACK_RESULT_CONTINUE
        TICK_CALLBACK_RESULT_DISCONTINUE = detail.TICK_CALLBACK_RESULT_DISCONTINUE
    end
    @export_enum TickCallbackResult

    @export_function FrameClock get_fps

    function set_tick_callback(f, widget::Widget, data::Data_t) where Data_t
        wrapped = TypedFunction(f, TickCallbackResult, (FrameClock, Data_t))
        detail.set_tick_callback(widget._internal.cpp_object, function (frame_clock_internal::Ref{detail._FrameClock})
            return (wrapped)(FrameClock(frame_clock_internal[]), data)
        end)
    end

    function set_tick_callback(f, widget::Widget)
        wrapped = TypedFunction(f, TickCallbackResult, (FrameClock,))
        detail.set_tick_callback(widget._internal.cpp_object, function (frame_clock_internal::Ref{detail._FrameClock})
            return (wrapped)(FrameClock(frame_clock_internal[]))
        end)
    end
    export set_tick_callback

    remove_tick_callback(widget::Widget) = detail.remove_tick_callback(widget._internal.cpp_object)
    export remove_tick_callback

####### action.jl

    Action(id::String, app::Application) = Action(detail._Action(id, app._internal.cpp_object))
    Base.show(io::IO, x::Action) = print(io, "Application(" * get_id(x) * ")")

    @export_function Action activate

    function set_function(f, action::Action,)
        detail.set_function(action._internal, function(instance_ref)
            TypedFunction(f, Cvoid, (Action,))(Action(instance_ref[]))
        end)
    end

    function set_function(f, action::Action, data::Data_t) where Data_t
        detail.set_function(action._internal, function(instance_ref)
            TypedFunction(f, Cvoid, (Action, Data_t))(Action(instance_ref[]), data)
        end)
    end
    export set_function

    function set_stateful_function(f, action::Action; initial_state = true)
        detail.set_stateful_function(action._internal, function(instance_ref, state)
            TypedFunction(f, Bool, (Action, Bool))(Action(instance_ref[]), state)
        end, initial_state)
    end
    export set_stateful_function

    @export_function Action get_id
    @export_function Action get_state
    @export_function Action set_state state Bool
    @export_function Action add_shortcut shortcut String
    @export_function Action clear_shortcuts
    @export_function Action get_shortcuts
    @export_function Action set_enabled b Bool
    @export_function Action get_enabled
    @export_function Action get_is_stateful

    @add_signal Action activated Cvoid

####### application.jl

    Application(id::String) = Application(detail._Application(id))

    Base.show(io::IO, x::Application) = print(io, "Application(" * get_id(x) * ")")

    import Base.run
    run(x::Application) = mousetrap.detail.run(x._internal)

    @export_function Application quit
    @export_function Application hold
    @export_function Application release
    @export_function Application mark_as_busy
    @export_function Application unmark_as_busy
    @export_function Application get_id

    add_action(app::Application, action::Action) = detail.add_action(app._internal, action._internal)
    export add_action

    get_action(app::Application, id::String) = return detail.get_action(app._internal, id)
    export get_action

    @export_function Application remove_action id String
    @export_function Application has_action id String

    @add_signal Application activate Cvoid
    @add_signal Application shutdown Cvoid
    
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

    set_lower(adjustment::Adjustment, x::Number) = detail.set_lower(adjustment._internal, convert(Cfloat, x))
    export set_lower

    set_upper(adjustment::Adjustment, x::Number) = detail.set_upper(adjustment._internal, convert(Cfloat, x))
    export set_upper

    set_value(adjustment::Adjustment, x::Number) = detail.set_value(adjustment._internal, convert(Cfloat, x))
    export set_value

    set_increment(adjustment::Adjustment, x::Number) = detail.set_increment(adjustment._internal, convert(Cfloat, x))
    export set_increment

    function Base.show(io::IO, x::Adjustment) 
        print(io, "Adjustment(" * 
            string(get_value(x)) * ", " *
            string(get_lower(x)) * ", " *
            string(get_upper(x)) * ", " *
            string(get_increment(x)) * ")"
        );
    end

    @add_signal Adjustment value_changed Cvoid
    @add_signal Adjustment properties_changed Cvoid

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

####### aspect_frame.jl

    function AspectFrame(x::AbstractFloat; child_x_align::AbstractFloat = 0.5, child_y_align::AbstractFloat = 0.5)
        return AspectFrame(detail._AspectFrame(convert(Cfloat, x), convert(Cfloat, child_x_align), convert(Cfloat, child_y_align)))
    end

    @export_function AspectFrame set_ratio
    @export_function AspectFrame get_ratio

    set_child_x_alignment(aspect_frame::AspectFrame, x::AbstractFloat) = detail.set_child_x_alignment(aspect_frame._internal, convert(Cfloat, x))
    export set_child_x_alignment

    set_child_y_alignment(aspect_frame::AspectFrame, x::AbstractFloat) = detail.set_child_y_alignment(aspect_frame._internal, convert(Cfloat, x))
    export set_child_y_alignment

    @export_function AspectFrame get_child_x_alignment
    @export_function AspectFrame get_child_y_alignment

    function set_child(parent::AspectFrame, child::Widget)
        detail.set_child(parent._internal, child._internal.cpp_object)
    end
    export set_child

    @export_function AspectFrame remove_child

    Base.show(io::IO, x::AspectFrame) = print(io, "AspectFrame(" * string(get_ratio(x)) * ")")

####### blend_mode.jl

    @enum BlendMode begin
        BLEND_MODE_NONE = detail.BLEND_MODE_NONE
        BLEND_MODE_NORMAL = detail.BLEND_MODE_NORMAL
        BLEND_MODE_ADD = detail.BLEND_MODE_ADD
        BLEND_MODE_SUBTRACT = detail.BLEND_MODE_SUBTRACT
        BLEND_MODE_REVERSE_SUBTRACT = detail.BLEND_MODE_REVERSE_SUBTRACT
        BLEND_MODE_MULTIPLY = detail.BLEND_MODE_MULTIPLY
        BLEND_MODE_MAX = detail.BLEND_MODE_MAX
        BLEND_MODE_MIN = detail.BLEND_MODE_MIN
    end
    @export_enum BlendMode

####### orientation.jl

    @enum Orientation begin
        ORIENTATION_HORIZONTAL = detail.ORIENTATION_HORIZONTAL
        ORIENTATION_VERTICAL = detail.ORIENTATION_VERTICAL
    end
    @export_enum Orientation

####### box.jl

    Box(orientation::Orientation) = return Box(detail._Box(Cint(orientation)))

    push_back(box::Box, child::Widget) = detail.push_back(box._internal, child._internal.cpp_object)
    export push_back

    push_front(box::Box, child::Widget) = detail.push_front(box._internal, child._internal.cpp_object)
    export push_front

    insert_after(box::Box, to_insert::Widget, after::Widget) = detail.insert_after(box._internal, to_insert._internal.cpp_object, after._internal.cpp_object)
    export insert_after

    remove(box::Box, to_remove::Widget) = detail.remove(box._internal, to_remove._internal.cpp_object)
    export remove

    @export_function Box clear
    @export_function Box set_homogeneous homogeneous Bool
    @export_function Box get_homogeneous
    @export_function Box set_spacing spacing Number
    @export_function Box get_spacing
    @export_function Box get_n_items

    get_orientation(box::Box) = return Orientation(detail.get_orientation(box._internal))
    export get_orientation

    set_orientation(box::Box, orientation::Orientation) = detail.set_orientation(box._internal, Cint(orientation))
    export set_orientation

    @add_widget_signals(Box)

####### button.jl

    Button() = Button(detail._Button())

    @export_function Button set_has_frame b Bool
    @export_function Button get_has_frame
    @export_function Button set_is_circular b Bool
    @export_function Button get_is_circular
    @export_function Button remove_child

    set_child(button::Button, child::Widget) = detail.set_child(button._internal, child._internal.cpp_object)
    export set_child

    set_action(button::Button, action::Action) = detail.set_action(button._internal, action._internal)
    export set_action

####### center_box.jl

    CenterBox(orientation::Orientation) = CenterBox(detail._CenterBox(Cint(orientation)))

    set_start_child(center_box::CenterBox, child::Widget) = detail.set_start_child(center_box._internal, child._internal.cpp_object)
    export set_start_child

    set_center_child(center_box::CenterBox, child::Widget) = detail.set_center_child(center_box._internal, child._internal.cpp_object)
    export set_center_child

    set_end_child(center_box::CenterBox, child::Widget) = detail.set_end_child(center_box._internal, child._internal.cpp_object)
    export set_end_child

    @export_function CenterBox remove_start_widget
    @export_function CenterBox remove_center_widget
    @export_function CenterBox remove_end_widget

    get_orientation(center_box::CenterBox) = Orientation(detail.get_orientation(center_box._internal))
    export get_orientation

    set_orientation(center_box::CenterBox, orientation::Orientation) = detail.set_orientation(center_box._internal, Cint(orientation))
    export set_orientation

####### check_button.jl

    CheckButton() = CheckButton(detail._CheckButton())

    @enum CheckButtonState begin
        CHECK_BUTTON_STATE_ACTIVE = detail.CHECK_BUTTON_STATE_ACTIVE
        CHECK_BUTTON_STATE_INACTIVE = detail.CHECK_BUTTON_STATE_INACTIVE
        CHECK_BUTTON_STATE_INCONSISTENT = detail.CHECK_BUTTON_STATE_INCONSISTENT
    end
    @export_enum CheckButtonState

    set_state(check_button::CheckButton, state::CheckButtonState) = detail.set_state(check_button._internal, Cint(state))
    export set_state

    get_state(check_button::CheckButton) ::CheckButtonState = CheckButtonState(detail.get_state(check_button._internal))
    export get_state

    @export_function CheckButton get_active

    if detail.GTK_MINOR_VERSION >= 8

        set_child(check_button::CheckButton, widget::Widget) = detail.set_child(check_button._internal, widget._internal.cpp_object)
        export set_child

        @export_function CheckButton remove_child

    end

####### window.jl

    struct RGBA
        r::Cfloat
        g::Cfloat
        b::Cfloat
        a::Cfloat
    end
    RGBA(r::AbstractFloat, g::AbstractFloat, b::AbstractFloat, a::AbstractFloat) = RGBA(convert(Cfloat, r), convert(Cfloat, g), convert(Cfloat, b), convert(Cfloat, a))
    export RGBA

    struct HSVA
        h::Cfloat
        s::Cfloat
        v::Cfloat
        a::Cfloat
    end
    HSVA(h::AbstractFloat, s::AbstractFloat, v::AbstractFloat, a::AbstractFloat) = HSVA(convert(Cfloat, h), convert(Cfloat, s), convert(Cfloat, v), convert(Cfloat, a))
    export HSVA

    rgba_to_hsva(rgba::RGBA) ::HSVA = return detail.rgba_to_hsva(rgba)
    export rgba_to_hsva

    hsva_to_rgba(hsva::HSVA) ::RGBA = return detail.hsva_to_rgba(hsva)
    export hsva_to_rgba

    Base.convert(::Type{HSVA}, x::RGBA) = return rgba_to_hsva(x)
    Base.convert(::Type{RGBA}, x::HSVA) = return hsva_to_rgba(x)

####### event_controller.jl

    @enum PropagationPhase begin
        PROPAGATION_PHASE_NONE = detail.PROPAGATION_PHASE_NONE
        PROPAGATION_PHASE_CAPTURE = detail.PROPAGATION_PHASE_CAPTURE
        PROPAGATION_PHASE_BUBBLE = detail.PROPAGATION_PHASE_BUBBLE
        PROPAGATION_PHASE_TARGET = detail.PROPAGATION_PHASE_TARGET
    end

    set_propagation_phase(controller::EventController, phase::PropagationPhase) = detail.set_propagation_phase(controller._internal, Cint(phase))
    get_propagation_phase(controller::EventController) = return PropagationPhase(detail.get_propagation_phase(controller._internal))

    @enum ButtonID begin
        BUTTON_ID_NONE = detail.BUTTON_ID_NONE
        BUTTON_ID_ANY = detail.BUTTON_ID_ANY
        BUTTON_ID_BUTTON_01 = detail.BUTTON_ID_BUTTON_01
        BUTTON_ID_BUTTON_02 = detail.BUTTON_ID_BUTTON_02
        BUTTON_ID_BUTTON_03 = detail.BUTTON_ID_BUTTON_03
        BUTTON_ID_BUTTON_04 = detail.BUTTON_ID_BUTTON_04
        BUTTON_ID_BUTTON_05 = detail.BUTTON_ID_BUTTON_05
        BUTTON_ID_BUTTON_06 = detail.BUTTON_ID_BUTTON_06
        BUTTON_ID_BUTTON_07 = detail.BUTTON_ID_BUTTON_07
        BUTTON_ID_BUTTON_08 = detail.BUTTON_ID_BUTTON_08
        BUTTON_ID_BUTTON_09 = detail.BUTTON_ID_BUTTON_09
    end

    macro export_single_click_gesture(T)
        out = Expr(:block)
        push!(out.args, :(
            get_current_button(gesture::T) = ButtonID(detail.get_current_button(gesture._internal))
        ));

        push!(out.args, :(
            set_only_listens_to_button(gesture::T, button::ButtonID) = detail.set_only_listens_to_button(gesture._internal, Cint(button))
        ));

        push!(out.args, :(
            get_only_listens_to_button(gesture::T) = ButtonID(detail.get_only_listens_to_button(gesture._internal))
        ));

        push!(out.args, :(
            set_touch_only(gesture::T, b::Bool) = detail.set_touch_only(gesture._internal, b)
        ));

        push!(out.args, :(
            get_touch_only(gesture::T) = return detail.get_touch_only(gesture._internal)
        ));

        return out
    end

####### motion_event_controller.jl

    @export_event_controller MotionEventController
    MotionEventController() = MotionEventController(detail._MotionEventController())

    @add_signal MotionEventController motion_enter Cvoid Cdouble x Cdouble y
    @add_signal MotionEventController motion Cvoid Cdouble x Cdouble y
    @add_signal MotionEventController motion_leave Cvoid

####### window.jl

    Window(app::Application) = Window(detail._Window(app._internal))
    export Window

    @export_function Window set_maximized
    @export_function Window set_fullscreen
    @export_function Window present
    @export_function Window close
    @export_function Window set_hide_on_close
    @export_function Window remove_child
    @export_function Window set_destroy_with_parent
    @export_function Window get_destroy_with_parent
    @export_function Window set_title
    @export_function Window get_title
    @export_function Window remove_titlebar_widget
    @export_function Window set_is_modal
    @export_function Window get_is_modal
    @export_function Window set_is_decorated
    @export_function Window get_is_decorated
    @export_function Window set_has_close_button
    @export_function Window get_has_close_button

    set_startup_notification_identifier(window::Window, title::String) = detail.set_startup_notification_identifier(window._internal, title)
    export set_startup_notification_identifier

    @export_function Window set_focus_visible
    @export_function Window get_focus_visible

    function set_child(window::Window, child::Widget)
        detail.set_child(window._internal, child._internal.cpp_object)
    end
    export set_child

    function set_titlebar_widget(window::Window, widget::Widget)
        detail.set_titlebar_widget(window._internal, widget._internal.cpp_object)
    end
    export set_titlebar_widget

    function set_default_widget(window::Window, widget::Widget)
        detail.set_default_widget(window._internal, widget._internal.cpp_object)
    end
    export set_default_widget

    @add_signal Window close_request Cvoid
    @add_signal Window activate_default_widget Cvoid
    @add_signal Window activate_focused_widget Cvoid
    @add_widget_signals(Window)
end

####### main.jl

using .mousetrap;

app = Application("test.app")

function on_motion(instance::MotionEventController, x::Cdouble, y::Cdouble)
   println(x, " ", y)
end

connect_signal_activate(app) do (app::Application)
    window = Window(app)

    motion_controller = MotionEventController()
    connect_signal_motion(on_motion, motion_controller)
    emit_signal_motion(motion_controller, 12.0, 13.0)
    add_controller(window, motion_controller)
    present(window)
end

run(app)




