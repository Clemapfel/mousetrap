#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform vec2 _canvas_size;

void main()
{
    const vec4 light = vec4(vec3(0.4), 1);
    const vec4 dark = vec4(vec3(0.6), 1);

    vec2 pos = _vertex_position.xy;
    pos *= _canvas_size;

    float square_size = 20;

    float y_index = mod(pos.x / square_size, 2);
    float x_index = mod(pos.y / square_size, 2);

    if (y_index > 1)
    {
        if (x_index > 1)
            _fragment_color = dark;
        else if (x_index < 1)
            _fragment_color = light;
    }
    else if (y_index < 1)
    {
        if (x_index > 1)
            _fragment_color = light;
        else if (x_index < 1)
            _fragment_color = dark;
    }

    _fragment_color *= _vertex_color;
}