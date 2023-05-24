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
                     throw(AssertionError("Object is not invokable as function with signature `$signature`"))
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

    macro export_function(name, type)
        mousetrap.eval(:(export $name))
        return :($name(x::$type) = detail.$name(x._internal))
    end

    macro export_signal_emitter(name)
        mousetrap.eval(:(export $name))
        return quote
            struct $name <: SignalEmitter
                _internal::detail.$name
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

####### signal_emitter.jl

    @document SignalEmitter "TODO"
    abstract type SignalEmitter end
    export SignalEmitter

####### application.jl

    @document Application "TODO"
    const Application = detail.Application
    export Application

    @document get_id "TODO"
    const get_id = detail.get_id

####### signal_components.jl

    struct SignalTask
        f::TypedFunction
        data
    end
    export SignalTask

    function (task::SignalTask)(x)
        task.f(x, task.data)
    end

    function connect_signal_activate(x::T, f, data) where {T}
        detail.connect_signal_activate(x, SignalTask(TypedFunction(f, Cvoid, (Application, Any)), data))
    end
    function connect_signal_activate(x::T, f) where {T}
        detail.connect_signal_activate(x, SignalTask(TypedFunction(f, Cvoid, (Application,)), nothing))
    end
    export connect_signal_activate
end

using .mousetrap;

app = Application("test.app")

function on_activate_f(app::Application, data)
    detail.test_initialize(app)
end
mousetrap.connect_signal_activate(app, on_activate_f, nothing)
mousetrap.detail.run(app)




