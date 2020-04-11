#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out float visibility;

uniform vec3 lightPos;

out vec3 fragNor;
out vec3 lightDir;
out vec3 viewDir;

const float density = 0.007;
const float gradient = 1.5;

void main()
{
	gl_Position = P * V * M * vertPos;
	fragNor = (M * vec4(vertNor, 0.0)).xyz;
	//lightDir = lightPos - (M*vertPos).xyz;
    lightDir = vec3(-1, 1 , 1);
    viewDir = -1 * (V * M * vertPos).xyz;
    
}

