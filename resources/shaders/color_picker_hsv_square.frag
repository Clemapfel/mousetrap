#version 130

vec3 rgb_to_hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv_to_rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform vec4 _current_color_hsva;
uniform int _vertical;

uniform vec2 _square_top_left;
uniform vec2 _square_size;

void main()
{
    vec3 current_hsv = _current_color_hsva.xyz;
    vec2 pos = (_vertex_position.xy + vec2(1)) / 2;

    vec2 dist = vec2(
        distance(pos.x, _square_top_left.x + _square_size.x) / _square_size.x,
        distance(pos.y, _square_top_left.y) / _square_size.y
    );

    vec3 left_right = vec3(_current_color_hsva.x, 0, _square_top_left.x + _square_size.x  - dist.x);
    vec3 top_bottom = vec3(_current_color_hsva.x, dist.y, _square_top_left.x + _square_size.x - dist.x);

    vec3 hsv = mix(left_right, top_bottom, dist.y);
    _fragment_color = vec4(hsv_to_rgb(hsv), 1);
}
