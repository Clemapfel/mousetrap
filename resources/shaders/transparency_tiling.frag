#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform int _texture_set;
uniform sampler2D _texture;

uniform vec2 _canvas_size;

void main()
{
    const vec4 light = vec4(vec3(0.4), 1);
    const vec4 dark = vec4(vec3(0.6), 1);

    vec2 pos = _vertex_position.xy;
    pos.x *= (_canvas_size.x / _canvas_size.y);

    float y_index = fract(pos.x);
    float x_index = fract(pos.y);

    if (y_index > 0.5)
    {
        if (x_index > 0.5)
            _fragment_color = dark;
        else if (x_index < 0.5)
            _fragment_color = light;
    }
    else if (y_index < 0.5)
    {
        if (x_index > 0.5)
            _fragment_color = light;
        else if (x_index < 0.5)
            _fragment_color = dark;
    }
}