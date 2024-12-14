#pragma once
#include "shape.h"

namespace shape
{
	
	class Cube : public Shape
	{
	public:
		int faceVertexCnt{};
		glm::vec3 center{};
		Model model;
		glm::vec4* color;
		glm::mat4 transform_change = glm::mat4(1.0f);
		glm::mat4 transform_self = glm::mat4(1.0f);
		glm::mat4 transform_world = glm::mat4(1.0f);
		glm::mat4 rotation_self = glm::mat4(1.0f);
		glm::mat4 rotation_world = glm::mat4(1.0f);
		glm::mat4 rotation_mom = glm::mat4(1.0f);
		glm::mat4 scale = glm::mat4(1.0f);

		Cube();

		Cube(const char* filename, int cnt);

		~Cube();

		void DrawShape();

		void Rotate_Self(GLfloat angle, glm::vec3 temp);

		void Rotate_World(GLfloat angle, glm::vec3 temp);

		void Rotate_Mom(GLfloat angle, glm::vec3 temp);
		
		void Transform_Self(glm::vec3);

		void Transform_World(glm::vec3);

		void Transform_change(glm::vec3);

		void Scale(glm::vec3);

		glm::vec3 GetPosition() const;
	};
}