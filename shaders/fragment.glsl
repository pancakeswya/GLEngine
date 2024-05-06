#version 330

out vec4 frag_color;

in vec2 tex_coords;
uniform sampler2D map_kd;

void main() {
  frag_color = texture(map_kd, tex_coords);
}