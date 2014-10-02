#version 150

in vec2 position;
in vec3 color;
in vec2 texcoord;
out vec3 _color;
out vec2 _texcoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	_color = color;
	_texcoord = texcoord;
	
	gl_Position = proj * view * model * vec4(position, 0.0, 1.0);
}