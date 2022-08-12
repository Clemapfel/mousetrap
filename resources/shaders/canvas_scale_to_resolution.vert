#version 330

layout (location = 0) in vec3 _vertex_position_in;
layout (location = 1) in vec4 _vertex_color_in;
layout (location = 2) in vec2 _vertex_texture_coordinates_in;

uniform mat4 _transform;

out vec4 _vertex_color;
out vec2 _texture_coordinates;
out vec3 _vertex_position;

uniform vec2 _resolution;

void main()
{
    vec4 vertex_pos = _transform * vec4(_vertex_position_in, 1.0);
    vertex_pos.x /= _resolution.x / _resolution.y;

    gl_Position = vertex_pos;
    _vertex_position = vertex_pos.xyz;

    _vertex_color = _vertex_color_in;

    vec2 texture_pos = _vertex_texture_coordinates_in;
    texture_pos.x /= _resolution.x / _resolution.y;

    _texture_coordinates = texture_pos;
}