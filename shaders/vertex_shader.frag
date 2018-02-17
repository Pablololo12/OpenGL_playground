#version 330 core
out vec4 color;
in vec3 f_color;
void main()
{
	color = vec4(f_color,1.0);
}