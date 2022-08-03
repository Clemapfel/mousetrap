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

uniform int _texture_set;
uniform sampler2D _texture;

uniform vec4 _current_color_hsv;
uniform vec2 _canvas_size;

bool compare_eps(float current, float target, float eps)
{
    return abs(current - target) < eps;
}

float sign (vec2 p1, vec2 p2, vec2 p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

vec2 translate_by_angle(vec2 center, float radius, float as_radians)
{
    return vec2(center.x + cos(as_radians) * radius,  center.y + sin(as_radians) * radius);
}

void main()
{
    vec2 pos = _vertex_position.xy;
    pos = (pos + 1) / 2;
    pos.y = 1 - pos.y;

    vec3 color_hsv = _current_color_hsv.xyz;

    float rotation_offset = 90;
    const vec2 center = vec2(0.5, 0.30);
    const float radius = 0.5;

    vec2 p1 = translate_by_angle(center, radius, radians(rotation_offset + 1/3.f * 360));
    vec2 p2 = translate_by_angle(center, radius, radians(rotation_offset + 2/3.f * 360));
    vec2 p3 = translate_by_angle(center, radius, radians(rotation_offset + 3/3.f * 360));

    float d1 = sign(pos, p1, p2);
    float d2 = sign(pos, p2, p3);
    float d3 = sign(pos, p3, p1);

    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    if (!(has_neg && has_pos))
        _fragment_color = vec4(hsv_to_rgb(color_hsv), 1);
    else
        _fragment_color = vec4(0);

    /*
    vec2 pos = _vertex_position.xy;
    pos.x *= (_canvas_width / (_canvas_height));
    float dist = distance(pos, vec2(0,0));

    float hue = 0.1;

    // outer transparency
    if (dist > 0.99 || (dist > 0.6 && dist < 0.7))
    {
        _fragment_color = vec4(0, 0, 0, 0);
        return;
    }

    // inner saturation/value
    else if (dist < 0.66)
    {
        dist *= 1 / 0.66;

        const float n_steps = 10;
        dist *= n_steps;
        dist = float(int(round(dist)));
        dist /= n_steps;

        vec3 s_ramp = vec3(hue, dist, 1);
        vec3 v_ramp = vec3(hue, 1, 0.95 - dist);

        vec3 hsv = mix(s_ramp, v_ramp, dist);
        _fragment_color = vec4(hsv_to_rgb(hsv), 1);
        return;
    }

    // 360 hue circle
    else
    {
        vec3 hsv = vec3(degrees(atan(pos.y, pos.x)) / 360, 1, 1);

        if (abs(hsv.x - hue) < 0.002)
        _fragment_color = vec4(1, 1, 1, 1);
        else
        _fragment_color = vec4(hsv_to_rgb(hsv), 1);
    }
*/
}