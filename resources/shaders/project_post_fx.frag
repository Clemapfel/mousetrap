#version 330 core

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform int _texture_set;
uniform sampler2D _texture;

uniform float _h_offset;
uniform float _s_offset;
uniform float _v_offset;
uniform float _r_offset;
uniform float _g_offset;
uniform float _b_offset;
uniform float _a_offset;

uniform int _flip_horizontally;
uniform int _flip_vertically;

uniform int _apply_flip;
uniform int _apply_color_offset;

vec3 rgb_to_hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv_to_rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float flip(float coord)
{
    return 1 - coord;
}

void main()
{
    vec2 pos = vec2(
        _apply_flip == 1 && _flip_horizontally == 1 ? flip(_texture_coordinates.x) : _texture_coordinates.x,
        _apply_flip == 1 && _flip_vertically == 1 ? flip(_texture_coordinates.y) : _texture_coordinates.y
    );

    vec4 color = texture2D(_texture, pos);

    if (_apply_color_offset != 1)
    {
        _fragment_color = color;
        return;
    }

    vec3 as_hsv = rgb_to_hsv(color.rgb);
    as_hsv.x = fract(as_hsv.x + _h_offset);
    as_hsv.y = clamp(as_hsv.y + _s_offset, 0, 1);
    as_hsv.z = clamp(as_hsv.z + _v_offset, 0, 1);

    vec3 as_rgb = hsv_to_rgb(as_hsv);
    as_rgb.x = clamp(as_rgb.x + _r_offset, 0, 1);
    as_rgb.y = clamp(as_rgb.y + _g_offset, 0, 1);
    as_rgb.z = clamp(as_rgb.z + _b_offset, 0, 1);

    float a = color.a == 0 ? color.a : clamp(color.a + _a_offset, 0, 1);
    _fragment_color = vec4(as_rgb.rgb, a * _vertex_color.a);
}
