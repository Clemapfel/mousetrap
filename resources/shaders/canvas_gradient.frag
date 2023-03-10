#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

#define DITHER_NONE 0
#define DITHER_2x2 1
#define DITHER_4x4 2
#define DITHER_8x8 3
#define PI 104348.f / 33215.f
// source: https://github.com/hughsk/glsl-dither

float luma(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

float dither2x2(vec2 position, float brightness)
{
    int x = int(mod(position.x, 2.0));
    int y = int(mod(position.y, 2.0));
    int index = x + y * 2;
    float limit = 0.0;

    if (x < 8) {
        if (index == 0) limit = 0.25;
        if (index == 1) limit = 0.75;
        if (index == 2) limit = 1.00;
        if (index == 3) limit = 0.50;
    }

    return brightness < limit ? 0.0 : 1.0;
}

vec3 dither2x2(vec2 position, vec3 color)
{
    return color * dither2x2(position, luma(color));
}

float dither4x4(vec2 position, float brightness)
{
    int x = int(mod(position.x, 4.0));
    int y = int(mod(position.y, 4.0));
    int index = x + y * 4;
    float limit = 0.0;

    if (x < 8) {
        if (index == 0) limit = 0.0625;
        if (index == 1) limit = 0.5625;
        if (index == 2) limit = 0.1875;
        if (index == 3) limit = 0.6875;
        if (index == 4) limit = 0.8125;
        if (index == 5) limit = 0.3125;
        if (index == 6) limit = 0.9375;
        if (index == 7) limit = 0.4375;
        if (index == 8) limit = 0.25;
        if (index == 9) limit = 0.75;
        if (index == 10) limit = 0.125;
        if (index == 11) limit = 0.625;
        if (index == 12) limit = 1.0;
        if (index == 13) limit = 0.5;
        if (index == 14) limit = 0.875;
        if (index == 15) limit = 0.375;
    }

    return brightness < limit ? 0.0 : 1.0;
}

vec3 dither4x4(vec2 position, vec3 color)
{
    return color * dither4x4(position, luma(color));
}

float dither8x8(vec2 position, float brightness)
{
    int x = int(mod(position.x, 8.0));
    int y = int(mod(position.y, 8.0));
    int index = x + y * 8;
    float limit = 0.0;

    if (x < 8) {
        if (index == 0) limit = 0.015625;
        if (index == 1) limit = 0.515625;
        if (index == 2) limit = 0.140625;
        if (index == 3) limit = 0.640625;
        if (index == 4) limit = 0.046875;
        if (index == 5) limit = 0.546875;
        if (index == 6) limit = 0.171875;
        if (index == 7) limit = 0.671875;
        if (index == 8) limit = 0.765625;
        if (index == 9) limit = 0.265625;
        if (index == 10) limit = 0.890625;
        if (index == 11) limit = 0.390625;
        if (index == 12) limit = 0.796875;
        if (index == 13) limit = 0.296875;
        if (index == 14) limit = 0.921875;
        if (index == 15) limit = 0.421875;
        if (index == 16) limit = 0.203125;
        if (index == 17) limit = 0.703125;
        if (index == 18) limit = 0.078125;
        if (index == 19) limit = 0.578125;
        if (index == 20) limit = 0.234375;
        if (index == 21) limit = 0.734375;
        if (index == 22) limit = 0.109375;
        if (index == 23) limit = 0.609375;
        if (index == 24) limit = 0.953125;
        if (index == 25) limit = 0.453125;
        if (index == 26) limit = 0.828125;
        if (index == 27) limit = 0.328125;
        if (index == 28) limit = 0.984375;
        if (index == 29) limit = 0.484375;
        if (index == 30) limit = 0.859375;
        if (index == 31) limit = 0.359375;
        if (index == 32) limit = 0.0625;
        if (index == 33) limit = 0.5625;
        if (index == 34) limit = 0.1875;
        if (index == 35) limit = 0.6875;
        if (index == 36) limit = 0.03125;
        if (index == 37) limit = 0.53125;
        if (index == 38) limit = 0.15625;
        if (index == 39) limit = 0.65625;
        if (index == 40) limit = 0.8125;
        if (index == 41) limit = 0.3125;
        if (index == 42) limit = 0.9375;
        if (index == 43) limit = 0.4375;
        if (index == 44) limit = 0.78125;
        if (index == 45) limit = 0.28125;
        if (index == 46) limit = 0.90625;
        if (index == 47) limit = 0.40625;
        if (index == 48) limit = 0.25;
        if (index == 49) limit = 0.75;
        if (index == 50) limit = 0.125;
        if (index == 51) limit = 0.625;
        if (index == 52) limit = 0.21875;
        if (index == 53) limit = 0.71875;
        if (index == 54) limit = 0.09375;
        if (index == 55) limit = 0.59375;
        if (index == 56) limit = 1.0;
        if (index == 57) limit = 0.5;
        if (index == 58) limit = 0.875;
        if (index == 59) limit = 0.375;
        if (index == 60) limit = 0.96875;
        if (index == 61) limit = 0.46875;
        if (index == 62) limit = 0.84375;
        if (index == 63) limit = 0.34375;
    }

    return brightness < limit ? 0.0 : 1.0;
}

vec3 dither8x8(vec2 position, vec3 color)
{
    return color * dither8x8(position, luma(color));
}

uniform vec4 _origin_color_rgba;
uniform vec4 _destination_color_rgba;

uniform vec2 _origin_point;
uniform vec2 _destination_point;

uniform int _dither_mode;
uniform int _is_circular;
uniform int _clamp_overhang;

uniform vec2 _canvas_size;

float atan2(in float y, in float x)
{
    bool s = (abs(x) > abs(y));
    return mix(PI / 2.0 - atan(x,y), atan(y,x), s);
}

float angle_rad(vec2 a, vec2 b)
{
    return atan2(a.x - b.x, a.y - b.y);
}

vec2 rotate(vec2 point, vec2 origin, float angle_rad)
{
    float s = sin(angle_rad);
    float c = cos(angle_rad);
    point -= origin;
    point *= mat2(c, -s, s, c);
    point += origin;

    return point;
}

float project(float lower, float upper, float value)
{
    return value * abs(upper - lower) + min(lower, upper);
}

void main()
{
    vec2 pos = _vertex_position.xy;
    pos.x *= (_canvas_size.x / _canvas_size.y);

    if (distance(pos, _origin_point) < 0.01)
    {
        _fragment_color = vec4(1, 1, 1, 1);
        return;
    }

    if (distance(pos, _destination_point) < 0.01)
    {
        _fragment_color = vec4(0, 1, 1, 1);
        return;
    }

    vec3 color;

    if (_is_circular == 1)
    {
        float point_distance = distance(_origin_point, _destination_point);
        float pos_distance = distance(pos, _origin_point);

        if (_clamp_overhang == 1)
            pos_distance = clamp(pos_distance, 0, point_distance);

        float mix_factor = pos_distance / point_distance;
        color = mix(_origin_color_rgba, _destination_color_rgba, mix_factor).xyz;
    }
    else
    {
        float x0 = pos.x;
        float x1 = _origin_point.x;
        float x2 = _destination_point.x;
        float y0 = pos.y;
        float y1 = _origin_point.y;
        float y2 = _destination_point.y;

        float angle = angle_rad(_origin_point, _destination_point);
        float length = abs((x2 - x1) * (y1 - y0) - (x1 - x0)*(y2 - y1)) / sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
        vec2 translated = vec2(x0, y0) + vec2(cos(angle), sin(angle)) * length;

        float mix_factor = distance(translated, _origin_point) / distance(_origin_point, _destination_point);
        color = mix(_origin_color_rgba, _destination_color_rgba, mix_factor).xyz;
    }

    if (_dither_mode == DITHER_NONE)
        _fragment_color = vec4(color, 1);
    else if (_dither_mode == DITHER_2x2)
        _fragment_color = vec4(dither2x2(gl_FragCoord.xy, color), 1);
    else if (_dither_mode == DITHER_4x4)
        _fragment_color = vec4(dither4x4(gl_FragCoord.xy, color), 1);
    else if (_dither_mode == DITHER_8x8)
        _fragment_color = vec4(dither8x8(gl_FragCoord.xy, color), 1);
    else
        discard;
}