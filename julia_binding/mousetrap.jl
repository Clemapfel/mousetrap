module mousetrap

    """
    export all enum instances into current module
    """
    macro export_enum(enum)
        quote
            export $enum
            for i in instances($enum)
                export i
            end
        end
    end

    """
    """
    abstract type Delegate end

    struct _Delegate0{Return_t} <: Delegate
        _apply::Function
    end

    function invoke(x::_Delegate0{Return_t}) ::Return_t where {Return_t}
        return x._apply()
    end

    struct _Delegate1{Return_t, Arg1_t} <: Delegate
        _apply::Function
    end

    function invoke(x::_Delegate0{Return_t}, arg1::Arg1_t) ::Return_t where {Return_t, Arg1_t}
        return x._apply(arg1)
    end

    struct _Delegate2{Return_t, Arg1_t, Arg2_t} <: Delegate
            _apply::Function
    end

    struct _Delegate3{Return_t, Arg1_t, Arg2_t, Arg3_t} <: Delegate
            _apply::Function
    end

    struct _Delegate4{Return_t, Arg1_t, Arg2_t, Arg3_t, Arg4_t} <: Delegate
            _apply::Function
    end
end


struct Delegate{Return_t, Arg1_t, Arg2_t}
    _apply::Function
end

function invoke(x::Delegate{Return_t, Arg1_t, Arg2_t}, args...) ::Return_t where {Return_t, Arg1_t, Arg2_t}

    arg1::Union{Arg1_t, Nothing} = nothing
    arg2::Union{Arg2_t, Nothing} = nothing

    if length(args) > 0
        arg1 = convert(Arg1_t, args[1])
    end

    if length(args) > 1
        arg2 = convert(Arg2_t, args[2])
    end

    out = x._apply(arg1, arg2)
    return convert(Return_t, out)
end






















