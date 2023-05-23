#
# TODO COPYRIGHT
#

"""
"""
abstract type TypedFunction end
export TypedFunction

function match_return_type(f::Function, return_type::Type, arg_ts...)

    actual_return_types = Base.return_types(f, arg_ts)
    for arg_t in arg_ts

        match_found = false
        for type in actual_return_types
             if type <: return_type || !empty(Base.return_types(Base.convert, (Type{return_type}, type)))
                match_found = true
                break;
            end
        end

        if !match_found
             signature = "("
             for i in 1:length(arg_ts)
                signature = signature * string(arg_ts[i]) * ((i < length(arg_ts)) ? ", " : ")")
             end
             signature = signature * " -> $return_type"
             throw(AssertionError("Object is not invokable as function with signature `$signature`"))
        end
    end
end

### 0 Arg

mutable struct _TypedFunction0{Return_t} <: TypedFunction

    apply::Function

    function _TypedFunction0(f::Function, return_type::Type)
        match_return_type(f, return_type)
        return new{return_type}(f);
    end
end
function (typed::_TypedFunction0{Return_t})() where {Return_t}
    return typed.apply()
end

### 1 Arg

mutable struct _TypedFunction1{Return_t, Arg1_t} <: TypedFunction

    apply::Function

    function _TypedFunction1(f::Function, return_type::Type, arg1_type::Type)
        match_return_type(f, return_type, arg1_type)
        return new{return_type, arg1_type}(f);
    end
end

function (typed::_TypedFunction1{Return_t, Arg1_t})(arg1) where {Return_t, Arg1_t}
     typed.apply(Base.convert(Arg1_t, arg1))
end

### 2 Arg

mutable struct _TypedFunction2{Return_t, Arg1_t, Arg2_t} <: TypedFunction

    apply::Function

    function _TypedFunction2(f::Function, return_type::Type, arg1_type::Type, arg2_type::Type)
        match_return_type(f, return_type, arg1_type, arg2_type)
        return new{return_type, arg1_type, arg2_type}(f);
    end
end

function (typed::_TypedFunction2{Return_t, Arg1_t, Arg2_t})(arg1, arg2) where {Return_t, Arg1_t, Arg2_t}
     typed.apply(
        Base.convert(Arg1_t, arg1),
        Base.convert(Arg2_t, arg2)
     )
end

### 3 Arg

mutable struct _TypedFunction3{Return_t, Arg1_t, Arg2_t, Arg3_t} <: TypedFunction

    apply::Function

    function _TypedFunction3(f::Function, return_type::Type, arg1_type::Type, arg2_type::Type, arg3_type::Type)
        match_return_type(f, return_type, arg1_type, arg2_type, arg3_type)
        return new{return_type, arg1_type, arg2_type, arg3_type}(f);
    end
end

function (typed::_TypedFunction3{Return_t, Arg1_t, Arg2_t})(arg1, arg2, arg3) where {Return_t, Arg1_t, Arg2_t, Arg3_t}
     typed.apply(
        Base.convert(Arg1_t, arg1),
        Base.convert(Arg2_t, arg2),
        Base.convert(Arg3_t, arg3)
     )
end

### 4 Arg

mutable struct _TypedFunction4{Return_t, Arg1_t, Arg2_t, Arg3_t, Arg4_t} <: TypedFunction

    apply::Function

    function _TypedFunction4(f::Function, return_type::Type, arg1_type::Type, arg2_type::Type, arg3_type::Type, arg4_type::Type)
        match_return_type(f, return_type, arg1_type, arg2_type, arg3_type, arg4_type)
        return new{return_type, arg1_type, arg2_type, arg3_type, arg4_type}(f);
    end
end

function (typed::_TypedFunction4{Return_t, Arg1_t, Arg2_t})(arg1, arg2, arg3, arg4) where {Return_t, Arg1_t, Arg2_t, Arg3_t, Arg4_t}
     typed.apply(
        Base.convert(Arg1_t, arg1),
        Base.convert(Arg2_t, arg2),
        Base.convert(Arg3_t, arg3),
        Base.convert(Arg4_t, arg4)
     )
end

forward_as_typed_function(f::Function, return_type) = _TypedFunction0(f, return_type, )
forward_as_typed_function(f::Function, return_type, arg1_type) = _TypedFunction1(f, return_type, arg1_type)
forward_as_typed_function(f::Function, return_type, arg1_type, arg2_type) = _TypedFunction2(f, return_type, arg1_type, arg2_type)
forward_as_typed_function(f::Function, return_type, arg1_type, arg2_type, arg3_type) = _TypedFunction3(f, return_type, arg1_type, arg2_type, arg3_type)
forward_as_typed_function(f::Function, return_type, arg1_type, arg2_type, arg3_type, arg4_type) = _TypedFunction4(f, return_type, arg1_type, arg2_type, arg3_type, arg4_type)



















