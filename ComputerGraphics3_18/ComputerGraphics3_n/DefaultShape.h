#pragma once
#include "shape.h"

namespace shape
{
	class DefaultShape : public Shape
	{
	public:
		int cnt{};
		int showcnt{};
		glm::vec3 center{};
		glm::vec3 vertex[10]{}; // 버텍스 버퍼
		int index[30]{}; // 인덱스 버퍼
		glm::vec4 color[10]{};
		bool isChnageShape{};
		bool isClicked{};

		DefaultShape();

		void SetVertex(glm::vec3 index, int cnt);

		void SetColor(glm::vec4 index, int cnt);
	};
}