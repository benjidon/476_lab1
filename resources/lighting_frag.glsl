#version 330 core 
in vec3 fragNor;
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;
out vec4 color;

in vec3 lightDir;
in vec3 viewDir;

void main()
{
   vec3 normal = normalize(fragNor);	
   vec3 lv = normalize(vec3(1, 1, 1));
   vec3 rv = -1.0 * lv + 2 * (dot(lv, normal)) * normal;
   
   // question about camera pos
   vec3 cameraPos = normalize(viewDir);
   vec3 h = normalize(normalize(lightDir) + cameraPos);

   // Map normal in the range [-1, 1] to color in range [0, 1]
	vec3 DefuseColor = MatDif * max(0, dot(normalize(lightDir), normal)) * vec3(1, 1, 1);
   
   vec3 Specular = MatSpec * pow(dot(normal, h), shine) * vec3(1, 1, 1); 

   vec3 Ambient =  MatAmb * vec3(1, 1, 1); 

   color = vec4(DefuseColor + Ambient + Specular, 1.0);
}
