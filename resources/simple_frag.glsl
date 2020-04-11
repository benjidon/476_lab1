#version 330 core 
in vec3 fragNor;
out vec4 color;

uniform vec3 difCol;
in vec3 lightDir;
void main()
{
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(vec3(1, -1, 1));
	color = vec4(0.1*difCol + difCol*(max(0, dot(normal, light))), 1.0);
}
