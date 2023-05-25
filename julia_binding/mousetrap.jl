module mousetrap

####### typed_function.jl

    """
    # TypedFunction

    Object used to invoke an arbitrary function using the given signature. This wrapper will automatically convert any arguments and return values to the given types unless absolutely impossible, at which point an assertion error will be triggered on construction

    ### Example

    ```jl
    f(x::Integer) = return string(x)

    # possible signature
    as_typed = TypedFunction(f, Int64, (Integer,))
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
                     throw(AssertionError("Object $f is not invokable as function with signature `$signature`"))
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

    function safe_call(scope, f, args...)
        try
            f(args...)
        catch e
            printstyled(stderr, "[ERROR] "; bold = true, color = :red)
            printstyled(stderr, "In " * scope * ": "; bold = true)
            Base.showerror(stderr, e, catch_backtrace())
            print(stderr, "\n");
        end
    end

    """
    Change the docstring of an object to given string
    """
    macro document(name, string)
        :(@doc $string $name)
    end

    macro export_function(type, name)
        mousetrap.eval(:(export $name))
        return :($name(x::$type) = detail.$name(x._internal))
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
        quote
            struct $name <: SignalEmitter
                _internal::detail.$name
            end
        end
    end

    import Base: *
    *(x::Symbol, y::Symbol) = return Symbol(string(x) * string(y))

####### signal_emitter.jl

    @document SignalEmitter "TODO"
    abstract type SignalEmitter end
    export SignalEmitter

####### signal_components.jl

    macro add_signal(T, snake_case)

        out = Expr(:block)

        connect_signal_name = :connect_signal_ * snake_case
        
        push!(out.args, esc(:(
            function $connect_signal_name(x::$T, f)
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, Cvoid, ($T,)))($T(x[]))
                end)
            end
        )))
        
        push!(out.args, esc(:(
            function $connect_signal_name(x::$T, f, data) where Data_t
                detail.$connect_signal_name(x._internal, function(x)
                    (TypedFunction(f, Cvoid, ($T, Any)))($T(x[]), data)
                end)
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
        
        emit_signal_name = :emit_signal_ * snake_case
        
        push!(out.args, esc(:(
            function $emit_signal_name(x::$T)
                return detail.$emit_signal_name(x._internal)
            end        
        )))
        
        push!(out.args, esc(:(export $connect_signal_name)))
        push!(out.args, esc(:(export $disconnect_signal_name)))
        push!(out.args, esc(:(export $set_signal_blocked_name)))
        push!(out.args, esc(:(export $get_signal_blocked_name)))
        push!(out.args, esc(:(export $emit_signal_name)))

        return out
    end

    # TODO: @document to generate documentation for all signals

####### action.jl

    @document Action "TODO"
    @export_signal_emitter Action
    Action(id::String, app::Application) = detail._Action(id, app._internal)

    @document activate
    @export_function Action activate

    function set_function(action::Action, f, data::Data_t) where Data_t
        detail.set_function(action._internal, TypedFunction(Cvoid, (detail._Action, Data_t)) do (x::detail._Action)
            f(Action(x), data)
        end)
    end

    @document get_id
    @export_function Action get_id

    @document get_state
    @export_function Action get_state

    @document set_state
    @export_function Action set_state

    @document add_shortcut
    @export_function Action add_shortcut

    @document clear_shortcuts
    @export_function Action clear_shortcuts

    @document set_enabled
    @export_function Action set_enabled

    @document get_enabled
    @export_function Action get_enabled

    @document get_is_stateful
    @export_function Action get_is_stateful

    @add_signal(Action, activated)

####### application.jl

    @document Application "TODO"
    @export_signal_emitter Application
    Application(id::String) = Application(detail._Application(id))

    import Base.run

    @document run "TODO"
    run(x::Application) = mousetrap.detail.run(x._internal)

    @document quit "TODO"
    @export_function Application quit

    @document hold "TODO"
    @export_function Application hold

    @document release "TODO"
    @export_function Application release

    @document mark_as_busy "TODO"
    @export_function Application mark_as_busy

    @document unmark_as_busy "TODO"
    @export_function Application unmark_as_busy

    @document add_action "TODO"
    add_action(app::Application, action::Action) = detail.add_action(app._internal, action._internal)
    export add_action

    @document get_action "TODO"
    get_action(app::Application, id::String) = return detail.get_action(app._internal, id)
    export get_action

    @document remove_action "TODO"
    @export_function Application remove_action

    @document has_action "TODO"
    @export_function Application has_action

    @add_signal(Application, activate)
    @add_signal(Application, shutdown)
end

####### main.jl

using .mousetrap;

app = Application("test.app")

connect_signal_activate(app, function(app::Application, data)
    mousetrap.detail.test_initialize(app._internal)

    action = Action("test.action", app)
    set_function(action, (x::Action, data) -> println(get_id(x), " ", data), 1234)
    activate(action)
end, 1234)

run(app)




