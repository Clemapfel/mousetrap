#version 330 core

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

layout(location = 0) out vec4 _fragment_color;

uniform int _texture_set;
uniform sampler2D _texture;

// this shader routes the fragment shader output to COLOR_ATTACHMENT_0, which is the location mousetrap::RenderTexture attaches to

void main()
{
    if (_texture_set != 1)
        _fragment_color = _vertex_color;
    else
        _fragment_color = texture2D(_texture, _texture_coordinates) * _vertex_color;
}
