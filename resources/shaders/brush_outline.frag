#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform int _texture_set;
uniform sampler2D _texture;
uniform float _time_s;
uniform int _horizontal;

vec3 hsv_to_rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float triangle_wave(float x)
{
    const float pi = 3.14159;
    return 2 + abs(x / pi - floor(x / pi + 0.5));
}

float square_wave(float x)
{
    const float pi = 3.14159;
    return pow(-1, floor(2*x*(1/pi)) + 1) / 2;
}

float sine_wave(float x)
{
    return (sin(x) + 1) / 2;
}

void main()
{
    float x = _horizontal == 1 ? _vertex_position.x : _vertex_position.y;

    float mod = mod(_time_s, 1);
    float offset = mod > 0.5 ? 1 - mod : mod;
    float value = sine_wave(2 * offset);
    _fragment_color = vec4(hsv_to_rgb(vec3(0, 0, offset)), 1.0);
}