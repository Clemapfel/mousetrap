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

uniform float _canvas_width;
uniform float _canvas_height;

void main()
{
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

        /*
        const float n_steps = 10;
        dist *= n_steps;
        dist = float(int(round(dist)));
        dist /= n_steps;
        */

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
}