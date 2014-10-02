#version 150

in vec3 _color;
in vec2 _texcoord;

uniform sampler2D tex1;
uniform sampler2D tex2;

out vec4 outColor;

void main() {
	//    outColor = texture(tex, _texcoord) * vec4(1.0, 0.0, 1.0, 1.0);

	vec4 color_tex1 = texture(tex1, _texcoord);
	vec4 color_tex2 = texture(tex2, _texcoord);

	outColor = mix(color_tex1, color_tex2, 0.5);
}