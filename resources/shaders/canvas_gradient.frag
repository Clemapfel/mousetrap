#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform vec4 _origin_color_rgba;
uniform vec4 _destination_color_rgba;

uniform vec2 _origin_point;
uniform vec2 _destination_point;

uniform int _dither_mode;
uniform int _is_circular;

void main()
{

    vec3 color = vec3(_texture_coordinates.x * 4);
    _fragment_color = vec4(color, 1);
}