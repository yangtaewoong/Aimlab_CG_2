#version 330 core

in vec4 out_Color;
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	float ambientLight = 0.5;
	vec3 ambient = ambientLight * lightColor;

	vec3 result = ambient * objectColor;

	FragColor = vec4 (result, 1.0);
	FragColor = out_Color;
}