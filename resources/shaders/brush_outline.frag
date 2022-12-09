#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform float _time_s;
uniform int _direction;

void main()
{
    float coord;
    float offset_factor;

    vec3 color = vec3(0, 0, 0);

    // left: top to bottom
    if (_direction == 1)
    {
        coord = _vertex_position.y;
        offset_factor = +1;
    }
    // right: bottom to top
    else if (_direction == 3)
    {
        coord = _vertex_position.y;
        offset_factor = -1;
    }
    // bottom: left to right
    else if (_direction == 2)
    {
        coord = _vertex_position.x;
        offset_factor = -1;
    }
    // top: bottom to top
    else if (_direction == 4)
    {
        coord = _vertex_position.x;
        offset_factor = +1;
    }

    float frequency = 21;
    float speed = 1;

    vec3 color_a = vec3(1);
    vec3 color_b = vec3(1) - color_a;

    if (fract(coord * frequency + offset_factor * _time_s * speed) > 0.5)
        _fragment_color = vec4(color_a, _vertex_color.a);
    else
        _fragment_color = vec4(color_b, _vertex_color.a);
}