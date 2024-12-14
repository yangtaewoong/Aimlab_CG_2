#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelTransform;

void main()
{
	gl_Position =  projection * view * modelTransform * vec4(vPos, 1.0);

	FragPos = vec3(modelTransform * vec4(vPos, 1.0));

	Normal = mat3(transpose(inverse(modelTransform))) * vec3(vNormal);
}