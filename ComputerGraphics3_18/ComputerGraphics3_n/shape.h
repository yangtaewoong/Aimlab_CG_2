#pragma once
#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
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
		glm::vec3 position{}; // ī�޶� ��ġ
		glm::vec3 front{};   // ī�޶� �ٶ󺸴� ����
		glm::vec3 up{};       // ���� �� ����
		GLfloat fov{};		// ī�޶� �þ߰�
		GLfloat lastX{};	// ���콺 �ʱ� ��ġ (������ �߽ɰ����� ����)
		GLfloat lastY{};  // ���콺 �ʱ� ��ġ (������ �߽ɰ����� ����)
		GLfloat sensitivity{};        // ���콺 ����
		GLfloat yaw{}; // ī�޶��� Y�� ȸ�� (����) // �⺻������ -Z���� �ٶ󺸵��� �ʱ�ȭ (-90��)
		GLfloat pitch{}; // ī�޶��� X�� ȸ�� (����) // ���Ʒ� ������ �ʱ⿡�� ���� ����

		bool isMouseLocked{}; // ���콺 ���� ����
		bool firstMouse{};       // ���콺 �ʱ� �̵� ����
		bool ignoreMouseEvent{}; // ���콺 �߾� ���� �̺�Ʈ ���� �÷���

		Camera();

		void calculateCameraFront();

		void lockMouse();

		void setIsMouseLocked(bool);

		void mouseCallback(GLfloat, GLfloat);

		glm::mat4 GetViewMatrix() const; // ī�޶��� �� ��� ���
		glm::mat4 GetProjectionMatrix(float aspectRatio, float nearPlane, float farPlane) const; // ���� ��� ���

	};
}