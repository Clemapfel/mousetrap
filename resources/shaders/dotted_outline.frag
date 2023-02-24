#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform float _time_s;
uniform int _direction;
uniform vec2 _canvas_size;
uniform int _animation_paused;

void main()
{
    float coord;
    float offset_factor;
    float frequency;

    const float base_frequency = 21;
    float speed = _animation_paused == 1 ? 0 : 0.5;

    // left: top to bottom
    if (_direction == 1)
    {
        coord = _vertex_position.y;
        offset_factor = +1;
        frequency = base_frequency * (_canvas_size.y / _canvas_size.x);
    }
    // right: bottom to top
    else if (_direction == 3)
    {
        coord = _vertex_position.y;
        offset_factor = -1;
        frequency = base_frequency * (_canvas_size.y / _canvas_size.x);
    }
    // bottom: left to right
    else if (_direction == 2)
    {
        coord = _vertex_position.x;
        offset_factor = -1;
        frequency = base_frequency;
    }
    // top: bottom to top
    else if (_direction == 4)
    {
        coord = _vertex_position.x;
        offset_factor = +1;
        frequency = base_frequency;
    }

    const vec3 color_a = vec3(1);
    const vec3 color_b = vec3(0.5, 0.5, 0.5);

    const bool clockwise = true;
    offset_factor *= clockwise ? -1 : 1;

    if (fract(coord * frequency + offset_factor * _time_s * speed) > 0.5)
        _fragment_color = vec4(color_a * _vertex_color.rgb, _vertex_color.a);
    else
        _fragment_color = vec4(color_b * _vertex_color.rgb, _vertex_color.a);
}