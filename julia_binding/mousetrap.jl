module mousetrap

    ### TypedFunction.jl

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

    (instance::TypedFunction)(args...) = Base.convert(instance._return_t, instance._apply([Base.convert(instance._arg_ts[i], args[i]) for i in 1:length(args)]...))

    ### Detail.jl

    module detail
        using CxxWrap
        function __init__() @initcxx end
        @wrapmodule("./libjulia_binding.so")
    end

    ### Common.jl

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

    ### SignalEmitter.jl

    """
    """
    abstract type SignalEmitter end

    ### Application.jl

    @export_signal_emitter(Application)
    @doc "TODO" Application

    Application(id::String) = Application(detail.Application(id))

    @export_function(run, Application)
    @doc "TODO" run

    @export_function(get_id, Application)
    @doc "TODO" get_id

    for n in names(mousetrap) println(n, " ", typeof(getproperty(mousetrap, n))) end
end

using .mousetrap;

println(@doc Application)

app = Application("test.id")
mousetrap.run(app)







