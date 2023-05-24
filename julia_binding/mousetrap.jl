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
    @export_signal_emitter(Application)
    Application(id::String) = Application(detail.Application(id))

    @document run "TODO"
    @export_function(run, Application)

    @document quit "TODO"
    @export_function(quit, Application)
    
    @document hold "TODO"
    @export_function(hold, Application)
    
    @document release "TODO"
    @export_function(release, Application)
    
    @document mark_as_busy "TODO"
    @export_function(mark_as_busy, Application)

    @document unmark_as_busy "TODO"
    @export_function(unmark_as_busy, Application)
    
    @document get_id "TODO"
    @export_function(get_id, Application)

    @document add_action "TODO"
    # TODO: add_action

    @document remove_action "TODO"
    # TODO: remove_action

    @document get_action "TODO"
    # TODO: get_action

    @document has_action "TODO"
    # TODO: has_action
end

using .mousetrap;

println(@doc Application)







