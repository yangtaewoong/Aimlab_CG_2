#version 330 core
layout (location = 0) in vec3 vPos; //--- 위치 변수: attribute position 0
layout (location = 1) in vec4 vColor; //--- 컬러 변수: attribute position 1

uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelTransform;

out vec4 out_Color;

void main()
{
	gl_Position =  projection * view * modelTransform * vec4(vPos, 1.0);

	out_Color = vColor;
}