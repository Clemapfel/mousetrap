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

    function safe_call(scope, f, args...)
        try
            f(args...)
        catch e
            printstyled(stderr, "[ERROR] "; bold = true, color = :red)
            printstyled(stderr, "In " * scope * ": "; bold = true)
            Base.showerror(stderr, e, catch_backtrace())
            print(stderr, "\n" = detail.CURSOR_TYPE
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
        quote
            struct $name <: SignalEmitter
                _internal::detail.$name
            end
        end
    end

    import Base: *
    *(x::Symbol, y::Symbol) = return Symbol(string(x) * string(y))

    import Base: clamp
    clamp(x::AbstractFloat, lower::AbstractFloat, upper::AbstractFloat) = if x < lower return lower elseif x > upper return upper else return x end

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

    # TODO: documentation for all signal-related methods

####### forward_declarations.jl

    @export_signal_emitter Application
    @export_signal_emitter Action
    @export_signal_emitter FrameClock

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

####### widget.jl

    @document Widget "TODO"
    abstract type Widget end
    export Widget

    macro define_set_margin(which)
        name = Symbol("set_margin_" * string(which))
        mousetrap.eval(:(export $name))
        return :(
            function $name(x::Widget, margin::Number)
                detail.$name(x._internal.cpp_object, convert(Cfloat, margin) = detail.CURSOR_TYPE
            end
        )
    end

    @document set_margin_top "TODO"
    @define_set_margin(top)

    @document set_margin_bottom "TODO"
    @define_set_margin(bottom)

    @document set_margin_left "TODO"
    @define_set_margin(left)

    @document set_margin_right "TODO"
    @define_set_margin(right)

    @document set_margin_horizontal "TODO"
    @define_set_margin(horizontal)

    @document set_margin_vertical "TODO"
    @define_set_margin(vertical)

    @document set_margin "TODO"
    @define_set_margin(Symbol(""))
    
    @document get_margin_top "TODO"
    get_margin_top(widget) = return detail.get_margin_top(widget._internal.cpp_object)
    export get_margin_top
    
    @document get_margin_bottom "TODO"
    get_margin_bottom(widget) = return detail.get_margin_top(widget._internal.cpp_object)
    export get_margin_bottom

    @document get_margin_start "TODO"
    get_margin_start(widget) = return detail.get_margin_top(widget._internal.cpp_object)
    export get_margin_start

    @document get_margin_end "TODO"
    get_margin_end(widget) = return detail.get_margin_top(widget._internal.cpp_object)
    export get_margin_end

    @document set_expand_horizontally "TODO"
    function set_expand_horizontally(widget::Widget, b::Bool)
        detail.set_expand_horizontally(widget._internal.cpp_object, b)
    end
    export set_expand_horizontally
    
    @document set_expand_vertically "TODO"
    function set_expand_vertically(widget::Widget, b::Bool)
        detail.set_expand_vertically(widget._internal.cpp_object, b)
    end
    export set_expand_vertically
    
    @document set_expand "TODO"
    function set_expand(widget::Widget, b::Bool)
        detail.set_expand(widget._internal.cpp_object, b)
    end
    export set_expand
    
    @document set_expand_horizontally "TODO"
    function get_expand_horizontally(widget::Widget, b::Bool)
        return detail.get_expand_horizontally(widget._internal.cpp_object)
    end
    export set_expand_horizontally
    
    @document set_expand_vertically "TODO"
    function set_expand_vertically(widget::Widget, b::Bool)
        return detail.set_expand_vertically(widget._internal.cpp_object)
    end
    export set_expand_vertically

    @document Alignment "TODO"
    @enum Alignment begin
        ALIGNMENT_START = detail.ALIGNMENT_START
        ALIGNMENT_CENTER = detail.ALIGNMENT_CENTER
        ALIGNMENT_END = detail.ALIGNMENT_END
    end
    @export_enum Alignment

    @document ALIGNMENT_START "TODO"
    @document ALIGNMENT_CENTER "TODO"
    @document ALIGNMENT_END "TODO"

    @document set_horizontal_alignment "TODO"
    function set_horizontal_alignment(widget::Widget, alignment::Alignment)
        detail.set_horizontal_alignment(widget._internal.cpp_object, Cint(alignment))
    end
    export set_horizontal_alignment

    @document set_vertical_alignment "TODO"
    function set_vertical_alignment(widget::Widget, alignment::Alignment)
        detail.set_vertical_alignment(widget._internal.cpp_object, Cint(alignment))
    end
    export set_vertical_alignment

    @document set_alignment "TODO"
    function set_alignment(widget::Widget, alignment::Alignment)
        detail.set_alignment(widget._internal.cpp_object, Cint(alignment))
    end
    export set_alignment
    
    @document get_vertical_alignment "TODO"
    get_vertical_alignment(widget::Widget) = return Alignment(detail.get_vertical_alignment(widget._internal.cpp_object))
    export get_vertical_alignment
    
    @document get_horizontal_alignment "TODO"
    get_vertical_alignment(widget::Widget) = return Alignment(detail.get_vertical_alignment(widget._internal.cpp_object))
    export get_horizontal_alignment

    @document set_opacity "TODO"
    set_opacity(widget::Widget, x::Number) = detail.set_opacity(widget._internal.cpp_object, Cfloat(clamp(x, 0, 1)))
    export set_opacity
    
    @document get_opacity "TODO"
    get_opacity(widget::Widget) = return detail.get_opacity(widget._internal.cpp_object)
    export get_opacity
    
    @document get_is_visible "TODO"
    get_is_visible(widget::Widget) = return detail.get_is_visible(widget._internal.cpp_object)
    export get_is_visible
    
    @document set_is_visible "TODO"
    set_is_visible(widget::Widget, b::Bool) = detail.set_is_visible(widget._internal.cpp_object, b)
    export set_is_visible
    
    @document set_tooltip_text "TODO"
    set_tooltip_text(widget::Widget, text::String) = detail.set_tooltip_text(widget._internal.cpp_object, text)
    export set_tooltip_text
    
    @document set_tooltip_widget "TODO"
    set_tooltip_widget(parent::Widget, tooltip::Widget) = detail.set_tooltip_widget(parent._internal.cpp_object, tooltip._internal.cpp_object)
    export set_tooltip_widget
    
    @document remove_tooltip_widget "TODO"
    remove_tooltip_widget(widget::Widget) = detail.remove_tooltip_widget(parent._internal.cpp_object)
    export remove_tooltip_widget

    @document CursorType "TODO"
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

    @document CURSOR_TYPE_NONE = "TODO"
    @document CURSOR_TYPE_DEFAULT = "TODO"
    @document CURSOR_TYPE_HELP = "TODO"
    @document CURSOR_TYPE_POINTER = "TODO"
    @document CURSOR_TYPE_CONTEXT_MENU = "TODO"
    @document CURSOR_TYPE_PROGRESS = "TODO"
    @document CURSOR_TYPE_WAIT = "TODO"
    @document CURSOR_TYPE_CELL = "TODO"
    @document CURSOR_TYPE_CROSSHAIR = "TODO"
    @document CURSOR_TYPE_TEXT = "TODO"
    @document CURSOR_TYPE_MOVE = "TODO"
    @document CURSOR_TYPE_NOT_ALLOWED = "TODO"
    @document CURSOR_TYPE_GRAB = "TODO"
    @document CURSOR_TYPE_GRABBING = "TODO"
    @document CURSOR_TYPE_ALL_SCROLL = "TODO"
    @document CURSOR_TYPE_ZOOM_IN = "TODO"
    @document CURSOR_TYPE_ZOOM_OUT = "TODO"
    @document CURSOR_TYPE_COLUMN_RESIZE = "TODO"
    @document CURSOR_TYPE_ROW_RESIZE = "TODO"
    @document CURSOR_TYPE_NORTH_RESIZE = "TODO"
    @document CURSOR_TYPE_NORTH_EAST_RESIZE = "TODO"
    @document CURSOR_TYPE_EAST_RESIZE = "TODO"
    @document CURSOR_TYPE_SOUTH_EAST_RESIZE = "TODO"
    @document CURSOR_TYPE_SOUTH_RESIZE = "TODO"
    @document CURSOR_TYPE_SOUTH_WEST_RESIZE = "TODO"
    @document CURSOR_TYPE_WEST_RESIZE = "TODO"
    @document CURSOR_TYPE_NORTH_WEST_RESIZE = "TODO"

    @document set_cursor "TODO"
    set_cursor(widget::Widget, cursor_type::CursorType) = detail.set_cursor(widget._internal.cpp_object, Cint(cursor_type))
    export set_cursor

    @document set_cursor_from_image
    # TODO: set_cursor_from_image(widget::Widget, image::Image, alignment::Vector2f)
    export set_cursor_from_image

    @document hide
    hide(widget::Widget) = detail.hide(widget._internal.cpp_object)
    export hide

    @document show
    show(widget::Widget) = detail.hide(widget._internal.cpp_object)
    export show

    @document add_controller
    # TODO: add_controller(widget::Widget, controller::EventController)
    export add_controller

    @document remove_controller
    # TODO: remove_controller(widget::Widget, controller::EventController)
    export remove_controller

    @document set_is_focusable
    set_is_focusable(widget::Widget, b::Bool) = detail.set_is_focusable(widget._internal.cpp_object, b)
    export set_is_focusable

    @document get_is_focusable
    get_is_focusable(widget::Widget) = return detail.get_is_focusable(widget._internal.cpp_object)
    export get_is_focusable

    @document grab_focus
    grab_focus(widget::Widget) = detail.grab_focus(widget._internal.cpp_object)
    export grab_focus

    @document get_has_focus
    get_has_focus(widget::Widget) = detail.get_has_focus(widget._internal.cpp_object)
    export get_has_focus

    @document set_focus_on_click
    set_focus_on_click(widget::Widget, b::Bool) = detail.set_focus_on_click(widget._internal.cpp_object, b)
    export set_focus_on_click

    @document get_focus_on_click
    get_focus_on_click(widget::Widget) = return detail.get_focus_on_click(widget._internal.cpp_object)
    export get_focus_on_click

    @document get_is_realized
    get_is_realized(widget::Widget) = return detail.get_is_realized(widget._internal.cpp_object)
    export get_is_realized

    @document get_minimum_size
    get_minimum_size(widget::Widget) ::Vector2f = return detail.get_minimum_size(widget._internal.cpp_object)
    export get_minimum_size

    @document get_natural_size
    get_natural_size(widget::Widget) ::Vector2f = return detail.get_natural_size(widget._internal.cpp_object)
    export get_natural_size

    @document get_position
    get_position(widget::Widget) ::Vector2f = return detail.get_position(widget._internal.cpp_object)
    export get_position

    @document get_allocated_size
    get_allocated_size(widget::Widget) ::Vector2f = return detail.get_allocated_size(widget._internal.cpp_object)
    export get_allocated_size

    @document unparent
    unparent(widget::Widget) = detail.unparent(widget._internal.cpp_object)
    export unparent

    @document set_can_respond_to_input
    set_can_respond_to_input(widget::Widget, b::Bool) = detail.set_can_respond_to_input(widget._internal.cpp_object, b)
    export set_can_respond_to_input

    @document get_can_respond_to_input
    get_can_respond_to_input(widget::Widget) = return detail.get_can_respond_to_input(widget._internal.cpp_object)
    export get_can_respond_to_input

    @document set_hide_on_overflow
    set_hide_on_overflow(widget::Widget, b::Bool) = detail.set_hide_on_overflow(widget._internal.cpp_object, b)
    export set_hide_on_overflow

    @document get_hide_on_overflow
    get_hide_on_overflow(widget::Widget) = return detail.get_hide_on_overflow(widget._internal.cpp_object)
    export get_hide_on_overflow

    @document get_clipboard
    # TODO get_clipboard(widget::Widget) ::Clipboard = Clipboard(detail.get_clipboard(widget._internal.cpp_object))
    export get_clipboard

####### tick_callback.jl

    @document FrameClock "TODO"

    @document set_tick_callback "TODO"
    function set_tick_callback(widget::Widget, f, data)
    end
    export set_tick_callback

    @document remove_tick_callback "TODO"
    remove_tick_callback(widget::Widget) = detail.remove_tick_callback(widget._internal.cpp_object)
    export remove_tick_callback

####### action.jl

    @document Action "TODO"
    Action(id::String, app::Application) = Action(detail._Action(id, app._internal.cpp_object))

    Base.show(io::IO, x::Action) = print(io, "Application(" * get_id(x) * ")")

    @document activate "TODO"
    @export_function Action activate

    @document set_function "TODO"
    function set_function(action::Action, f)
        detail.set_function(action._internal, function(instance_ref)
            TypedFunction(f, Cvoid, (Action,))(Action(instance_ref[]))
        end)
    end

    function set_function(action::Action, f, data::Data_t) where Data_t
        detail.set_function(action._internal, function(instance_ref)
            TypedFunction(f, Cvoid, (Action, Data_t))(Action(instance_ref[]), data)
        end)
    end
    export set_function

    @document set_stateful_function "TODO"
    function set_stateful_function(action::Action, f; initial_state = true)
        detail.set_stateful_function(action._internal, function(instance_ref, state)
            TypedFunction(f, Bool, (Action, Bool))(Action(instance_ref[]), state)
        end, initial_state)
    end
    export set_stateful_function

    @document get_id "TODO"
    @export_function Action get_id

    @document get_state "TODO"
    @export_function Action get_state

    @document set_state "TODO"
    @export_function Action set_state state Bool

    @document add_shortcut "TODO"
    @export_function Action add_shortcut shortcut String

    @document clear_shortcuts "TODO"
    @export_function Action clear_shortcuts

    @document get_shortcuts "TODO"
    @export_function Action get_shortcuts

    @document set_enabled "TODO"
    @export_function Action set_enabled b Bool

    @document get_enabled "TODO"
    @export_function Action get_enabled

    @document get_is_stateful "TODO"
    @export_function Action get_is_stateful

    @add_signal(Action, activated)

####### application.jl

    @document Application "TODO"
    Application(id::String) = Application(detail._Application(id))

    Base.show(io::IO, x::Application) = print(io, "Application(" * get_id(x) * ")")

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
    
####### window.jl

    @document Window "TODO"
    
end

####### main.jl

using .mousetrap;

app = Application("test.app")

connect_signal_activate(app, function(app::Application, data)
    mousetrap.detail.test_initialize(app._internal)

    action = Action("test.action", app)
    set_function(action, (x::Action, data) -> println(get_id(x), " ", data), 1234)

    add_shortcut(action, "m")
    add_shortcut(action, "<Control>c")
    #println(get_shortcuts(action))
    activate(action)
end, 1234)

run(app)




