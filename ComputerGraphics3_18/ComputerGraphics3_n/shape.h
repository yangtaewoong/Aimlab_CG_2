#pragma once
#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

namespace shape
{
	class Shape;

	class Shape
	{
	public:
		int cnt{};
		GLfloat speed{};
		bool isActive{};
		glm::mat3 direcTionV3{};

		virtual void SetSpeed(GLfloat s_speed);

		virtual void SetDic(glm::mat3 dic);

		virtual void SetIsActive(bool is);

		virtual void MoveShape() {};
	};

	typedef struct {
		float x, y, z;
	} Vertex;

	typedef struct {
		float u, v;
	} Texture;


	typedef struct {
		unsigned int v1, v2, v3, v4;  // Vertex indices
		unsigned int t1, t2, t3, t4;
		unsigned int n1, n2, n3, n4;  // Normal indices
	} Face;

	typedef struct {
		Vertex* vertices;
		Vertex* normals;
		Texture* textures;
		Face* faces;
		size_t vertex_count;
		size_t textures_count;
		size_t normal_count;
		size_t face_count;
	} Model;

	class Camera 
	{
	public:
		glm::vec3 position{}; // 카메라 위치
		glm::vec3 front{};   // 카메라가 바라보는 방향
		glm::vec3 up{};       // 월드 업 벡터
		GLfloat fov{};		// 카메라 시야각
		GLfloat lastX{};	// 마우스 초기 위치 (윈도우 중심값으로 설정)
		GLfloat lastY{};  // 마우스 초기 위치 (윈도우 중심값으로 설정)
		GLfloat sensitivity{};        // 마우스 감도
		GLfloat yaw{}; // 카메라의 Y축 회전 (수평) // 기본적으로 -Z축을 바라보도록 초기화 (-90도)
		GLfloat pitch{}; // 카메라의 X축 회전 (수직) // 위아래 각도는 초기에는 수평 상태

		bool isMouseLocked{}; // 마우스 고정 여부
		bool firstMouse{};       // 마우스 초기 이동 감지
		bool ignoreMouseEvent{}; // 마우스 중앙 고정 이벤트 무시 플래그

		Camera();

		void calculateCameraFront();

		void lockMouse();

		void setIsMouseLocked(bool);

		void mouseCallback(GLfloat, GLfloat);

		glm::mat4 GetViewMatrix() const; // 카메라의 뷰 행렬 계산
		glm::mat4 GetProjectionMatrix(float aspectRatio, float nearPlane, float farPlane) const; // 투영 행렬 계산

	};
}