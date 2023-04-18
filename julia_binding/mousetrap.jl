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

    module detail
        using CxxWrap
        function __init__()
            @initcxx
        end
        @wrapmodule("./libmousetrap_julia_binding.so")
    end

    """
    """
    struct RGBA
        r::Float32
        g::Float32
        b::Float32
        a::Float32

        RGBA(r::Real, g::Real, b::Real, a::Real) = new(Float32(r), Float32(g), Float32(b), Float32(a))
    end
    export RGBA

    """
    """
    struct HSVA
        h::Float32
        s::Float32
        v::Float32
        a::Float32

        HSVA(h::Real, s::Real, v::Real, a::Real) = new(Float32(h), Float32(s), Float32(v), Float32(a))
    end
    export HSVA

    """
    """
    function rgba_to_hsva(rgba::RGBA) ::HSVA

        h = Ref{Float32}(0)
        s = Ref{Float32}(0)
        v = Ref{Float32}(0)
        a = Ref{Float32}(0)

        detail.rgba_to_hsva(rgba.r, rgba.g, rgba.b, rgba.a, h, s, v, a)
        return HSVA(h[], s[], v[], a[])
    end
    export rgba_to_hsva

    """
    """
    function hsva_to_rgba(hsva::HSVA) ::RGBA

        r = Ref{Float32}(0)
        g = Ref{Float32}(0)
        b = Ref{Float32}(0)
        a = Ref{Float32}(0)

        detail.hsva_to_rgba(hsva.h, hsva.s, hsva.v, hsva.a, r, g, b, a)
        return RGBA(r[], g[], b[], a[])
    end
    export hsva_to_rgba

    """
    """
    invert(rgba::RGBA) ::RGBA = RGBA(1 - rgba.r, 1 - rgba.g, 1 - rgba.b, rgba.a)
    invert(hsva::HSVA) ::HSVA = HSVA(1 - hsva.h, hsva.s, 1 - hsva.v, hsva.a)
    export invert

    """
    """
    const Image = detail.Image
    export Image

    """
    """
    create(image::Image, x::Integer, y::Integer, rgba::RGBA) = detail.create(image, x, y, rgba)
    export create

    """
    """
    const create_from_file = detail.create_from_file
    export create_from_file

    """
    """
    const save_to_file = detail.save_to_file
    export save_to_file

    """
    """
    const get_size = detail.get_size
    export get_size

    """
    """
    @enum InterpolationType begin
        NEAREST = detail.INTERPOLATION_NEAREST
        TILES = detail.INTERPOLATION_TILES
        BILINEAR = detail.INTERPOLATION_BILINEAR
        HYPERBOLIC = detail.INTERPOLATION_HYPERBOLIC
    end
    @export_enum InterpolationType

    """
    """
    as_scaled(image::Image, new_width::Integer, new_height::Integer, interpolation::InterpolationType) = detail.as_scaled(image, UInt64(new_with), UInt64(new_height), interpolation)
    export as_scaled

    """
    """
    as_cropped(image::Image, offset_x::Integer, offset_y::Integer, new_width::Integer, new_height::Integer) = detail.as_cropped(image, Int32(offset_x), Int32(offset_y), UInt64(new_with), UInt64(new_height))
    export as_cropped

    """
    """
    set_pixel(image::Image, x::Integer, y::Integer, rgba::RGBA) = detail.set_pixel(image, UInt64(x), UInt64(y), rgba)
    set_pixel(image::Image, x::Integer, y::Integer, hsva::HSVA) = detail.set_pixel(image, UInt64(x), UInt64(y), hsva_to_rgba(hsva))
    export set_pixel

    """
    """
    get_pixel(image::Image, x::Integer, y::Integer) ::RGBA = return detail.get_pixel(image, UInt64(x), UInt64(y))
    export get_pixel
end

using .mousetrap
using Test

image = Image()
create(image, 100, 100, RGBA(1, 0, 0, 0))
set_pixel(image, 10, 10, RGBA(0, 1, 0, 0)
@test get_pixel(image, 9, 9) == RGBA(1, 0, 0, 0)
@test get_pixel(image, 10, 10) == RGBA(0, 1, 0, 0)
@test get_size(image).x == 100 && get_size(image).y == 100