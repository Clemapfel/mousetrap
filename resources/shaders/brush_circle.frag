#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform float _background_value;
uniform float _margin;

void main()
{
    // assumes canvas is always square

    if (distance(vec2(0, 0), _vertex_position.xy) < 1 - _margin)
        _fragment_color = vec4(vec3(1), 1);
    else
        _fragment_color = vec4(vec3(_background_value), 1);
}