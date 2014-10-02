#ifndef __SHADERS_SOURCE__
#define __SHADERS_SOURCE__

const char* __externVertexSource = 
"#version 150\n"
"\n"
"in vec2 position;\n"
"in vec3 color;\n"
"in vec2 texcoord;\n"
"out vec3 _color;\n"
"out vec2 _texcoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"
"\n"
"void main() {\n"
"	_color = color;\n"
"	_texcoord = texcoord;\n"
"	\n"
"	gl_Position = proj * view * model * vec4(position, 0.0, 1.0);\n"
"}";
const char* __externFragmentSource = 
"#version 150\n"
"\n"
"in vec3 _color;\n"
"in vec2 _texcoord;\n"
"\n"
"uniform sampler2D tex1;\n"
"uniform sampler2D tex2;\n"
"\n"
"out vec4 outColor;\n"
"\n"
"void main() {\n"
"	//    outColor = texture(tex, _texcoord) * vec4(1.0, 0.0, 1.0, 1.0);\n"
"\n"
"	vec4 color_tex1 = texture(tex1, _texcoord);\n"
"	vec4 color_tex2 = texture(tex2, _texcoord);\n"
"\n"
"	outColor = mix(color_tex1, color_tex2, 0.1);\n"
"}";

#endif
