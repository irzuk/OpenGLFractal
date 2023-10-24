#version 330 core
layout(location=0) in vec2 pos;
out vec2 position;

void main() {
	position = pos;
	gl_Position = vec4(pos, 0.0, 1.0);
}
