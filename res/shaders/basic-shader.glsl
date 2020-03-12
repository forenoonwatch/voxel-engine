#include "common.glh"

varying vec3 in_color;

#if defined(VS_BUILD)

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in mat4 transform;

const vec3 LIGHT_DIR = normalize(vec3(-0.5, 1.0, 1.0));

void main() {
    const float light_power = fma(clamp(dot(normal, LIGHT_DIR), 0.0, 1.0), 0.8, 0.2);

    gl_Position = transform * vec4(position, 1.0);
    in_color = color * light_power;
}

#elif defined(FS_BUILD)

layout (location = 0) out vec4 out_color;

void main() {
    out_color = vec4(vec3(in_color), 1.0);
}

#endif