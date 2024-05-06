#version 330

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normals;
layout(location = 2) in vec2 a_tex_coords;

out vec2 tex_coords;

uniform mat4 u_transform;

void main() {
  gl_Position = u_transform * vec4(a_pos, 1.0);
}