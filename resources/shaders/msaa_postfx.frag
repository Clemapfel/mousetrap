#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform int _texture_set;
uniform sampler2D _texture;

void main()
{
    vec4 color = texture2D(_texture, _texture_coordinates);
    const float intensity = 0.8;

    color.rgb *= (-1 * intensity) / log(color.rgb);
    color.rgb = clamp(color.rgb, 0, 1);
    _fragment_color = color * _vertex_color;
}