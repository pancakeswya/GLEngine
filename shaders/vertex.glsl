#version 330

layout(location = 0) in vec3 pos;

uniform mat4 transform;

out vec3 color;

void main() {
  gl_Position = transform * vec4(pos, 1.0);
  color = vec3(1.0f, 0.4f, 0.6f);
}