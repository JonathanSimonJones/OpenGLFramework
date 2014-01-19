#version 150

in vec3 position;
in vec3 color;

out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 overrideColor;
uniform float Alpha;

void main()
{
	Color = vec4(color, Alpha);
	gl_Position = proj * view * model * vec4(position, 1.0);
}