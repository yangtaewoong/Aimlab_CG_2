#include "DefaultShape.h"

using namespace shape;

DefaultShape::DefaultShape()
{
	for (glm::vec3& vertex : vertex) {
		vertex = glm::vec3(1.0f);
	}
	
	for (glm::vec4& color : color) {
		color = glm::vec4(0.0f);
	}
}

void DefaultShape::SetVertex(glm::vec3 index, int cnt) {
	vertex[cnt] = index;
}

void DefaultShape::SetColor(glm::vec4 index, int cnt) {
	color[cnt] = index;
}