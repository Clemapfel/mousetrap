//
// Created by clem on 4/16/23.
//

#include <mousetrap.hpp>
#include <jlcxx/jlcxx.hpp>
#include "julia_interface.cpp"

using namespace mousetrap;

#define add_type_method(Type, id) method(#id, &Type::id)
#define declare_is_subtype_of(A, B) template<> struct jlcxx::SuperType<A> { typedef B type; };
#define make_not_mirrored(Name) template<> struct jlcxx::IsMirroredType<Name> : std::false_type {};
#define add_enum(Name) add_bits<Name>(#Name, jlcxx::julia_type("CppEnum"))
#define add_enum_value(EnumName, PREFIX, VALUE_NAME) set_const(std::string(#PREFIX) + "_" + std::string(#VALUE_NAME),  EnumName::VALUE_NAME)

#define jl_size_t int64_t

// ### COLORS ###

RGBA unbox_rgba(jl_value_t* in)
{
    if (not jl_assert_type(in, "mousetrap.RGBA"))
        jl_throw_exception("In unbox<RGBA>: Argument #1 is of wrong type");

    return RGBA(
        jl_unbox_float32(jl_get_property(in, "r")),
        jl_unbox_float32(jl_get_property(in, "g")),
        jl_unbox_float32(jl_get_property(in, "b")),
        jl_unbox_float32(jl_get_property(in, "a"))
    );
}

jl_value_t* box_rgba(RGBA in)
{
    static auto* rgba = jl_get_function(jl_main_module, "mousetrap.RGBA");
    return jl_calln(rgba,
        jl_box_float32(in.r),
        jl_box_float32(in.g),
        jl_box_float32(in.b),
        jl_box_float32(in.a)
    );
}

void add_colors(jlcxx::Module& module)
{
    module.method("rgba_to_hsva", [](
        float r, float g, float b, float a, float* h_out, float* s_out, float* v_out, float* a_out
    ) -> void
    {
        auto out = RGBA(r, g, b, a).operator HSVA();
        *h_out = out.h;
        *s_out = out.s;
        *v_out = out.v;
        *a_out = out.a;
    });

    module.method("hsva_to_rgba", [](
        float h, float s, float v, float a, float* r_out, float* g_out, float* b_out, float* a_out
    ) -> void
    {
        auto out = HSVA(h, s, v, a).operator RGBA();
        *r_out = out.r;
        *g_out = out.g;
        *b_out = out.b;
        *a_out = out.a;
    });
}

// ### VECTOR ###

#define define_box_unbox_vector2(t, box_t) \
    Vector2##t unbox_vector2##t(jl_value_t* in) \
    { \
      if (not jl_assert_type(in, "mousetrap.Vector2" + std::string(#t))) \
        jl_throw_exception("In unbox<Vector2" + std::string(#t) + ">: Argument #1 is of wrong type"); \
         \
        return Vector2##t( \
        jl_unbox_##box_t(jl_get_property(in, "x")), \
        jl_unbox_##box_t(jl_get_property(in, "y")) \
        ); \
    } \
    \
    jl_value_t* box_vector2##t(Vector2##t in) \
    { \
        static auto* vector2##t = jl_get_function(jl_main_module, ("mousetrap.Vector2" + std::string(#t)).c_str()); \
        return jl_call2(vector2##t, jl_box_##box_t(in.x), jl_box_##box_t(in.y)); \
    }\

define_box_unbox_vector2(f, float32)
define_box_unbox_vector2(ui, int64) // sic
define_box_unbox_vector2(i, int32)

/// ### IMAGE ###

void add_image(jlcxx::Module& module)
{
    auto image = module.add_type<Image>("Image");

    module.method("create", [](Image& image, jl_size_t width, jl_size_t height, jl_value_t* color){
        image.create(width, height, unbox_rgba(color));
    });

    image.add_type_method(Image, create_from_file);
    image.add_type_method(Image, save_to_file);

    module.method("get_size", [](Image& image) -> jl_value_t*{
       return box_vector2ui(image.get_size());
    });

    module.add_enum(InterpolationType);
    module.add_enum_value(InterpolationType, INTERPOLATION, NEAREST);
    module.add_enum_value(InterpolationType, INTERPOLATION, TILES);
    module.add_enum_value(InterpolationType, INTERPOLATION, BILINEAR);
    module.add_enum_value(InterpolationType, INTERPOLATION, HYPERBOLIC);

    module.method("as_scaled", [](const Image& image, jl_size_t size_x, jl_size_t size_y, InterpolationType type){
        return image.as_scaled(size_x, size_y, type);
    });

    module.method("as_cropped", [](const Image& image, int64_t offset_x, int64_t offset_y, jl_size_t size_x, jl_size_t size_y, InterpolationType type){
        return image.as_scaled(size_x, size_y, type);
    });

    image.add_type_method(Image, as_flipped);

    module.method("set_pixel", [](Image& image, jl_size_t x, jl_size_t y, jl_value_t* rgba){
        image.set_pixel(x, y, unbox_rgba(rgba));
    });

    module.method("get_pixel", [](Image& image, jl_size_t x, jl_size_t y) -> jl_value_t* {
        return box_rgba(image.get_pixel(x, y));
    });
}

// ### MAIN ###

JLCXX_MODULE define_julia_module(jlcxx::Module& module)
{
    add_colors(module);
    add_image(module);
}
