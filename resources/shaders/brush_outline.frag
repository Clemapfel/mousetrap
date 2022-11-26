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

void main()
{
    float value = (sin(40 * (_horizontal == 1 ? _vertex_position.x : _vertex_position.y) + _time_s * 10) + 1) / 2.0;
    _fragment_color = vec4(hsv_to_rgb(vec3(0, 0, value)), 1.0);
}