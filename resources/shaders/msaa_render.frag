#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;
uniform sampler2D _texture;

void main()
{
    vec2 pos = _texture_coordinates.xy;
    pos.y = 1 - pos.y;
    _fragment_color = texture2D(_texture, pos) * _vertex_color;
}