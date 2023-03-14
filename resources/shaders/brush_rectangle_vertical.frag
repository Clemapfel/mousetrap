#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform float _half_margin;

void main()
{
    // assumes canvas is always square

    vec2 pos = _vertex_position.xy;

    float w = 1 - 2 * _half_margin;
    float h = 1/3.f * w;

    if (pos.x > -w && pos.x < +w && pos.y < +h && pos.y > -h)
        _fragment_color = vec4(vec3(1), 1);
    else
        _fragment_color = vec4(vec3(1), 0);
}