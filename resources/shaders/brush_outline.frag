#version 130

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform int _texture_set;
uniform sampler2D _texture;
uniform vec2 _texture_size;

uniform int _outline_visible;
uniform vec4 _outline_color_rgba;

void main()
{
    vec2 pos = vec2(_texture_coordinates.x, 1 - _texture_coordinates.y); // because of render texture

    float pixel_h = 1.f / max(_texture_size.x, _texture_size.y);
    float pixel_w = 1.f / max(_texture_size.x, _texture_size.y);

    bool center = texture2D(_texture, vec2(pos.x + 0 * pixel_w, pos.y + 0 * pixel_h)).a > 0;
    bool top = texture2D(_texture, vec2(pos.x + 0 * pixel_w, pos.y - 1 * pixel_h)).a > 0;
    bool bottom = texture2D(_texture, vec2(pos.x + 0 * pixel_w, pos.y + 1 * pixel_h)).a > 0;
    bool left = texture2D(_texture, vec2(pos.x - 1 * pixel_w, pos.y + 0 * pixel_h)).a > 0;
    bool right = texture2D(_texture, vec2(pos.x + 1 * pixel_w, pos.y + 0 * pixel_h)).a > 0;

    /*
    bool top_left = texture2D(_texture, vec2(pos.x - 1 * pixel_w, pos.y - 1 * pixel_h)).a > 0;
    bool top_right = texture2D(_texture, vec2(pos.x + 1 * pixel_w, pos.y - 1 * pixel_h)).a > 0;
    bool bottom_right = texture2D(_texture, vec2(pos.x + 1 * pixel_w, pos.y + 1 * pixel_h)).a > 0;
    bool bottom_left = texture2D(_texture, vec2(pos.x - 1 * pixel_w, pos.y + 1 * pixel_h)).a > 0;
    */

    if ((_outline_visible == 1) && !center && (top || right || bottom || left)) // || top_right || bottom_right || bottom_left || top_left))
        _fragment_color = _outline_color_rgba;
    else
        _fragment_color = texture2D(_texture, pos) * _vertex_color;

}