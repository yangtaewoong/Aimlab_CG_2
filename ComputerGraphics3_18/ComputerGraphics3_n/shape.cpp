#pragma once
#include "shape.h"

#define SCREENWIDTH 1980
#define SCREENHEIGHT 1080

using namespace shape;

void Shape::SetIsActive(bool is)
{
	isActive = is;
}

void Shape::SetSpeed(GLfloat s_speed)
{
	speed = s_speed;
}

void Shape::SetDic(glm::mat3 dic)
{
	direcTionV3 = dic;
}

Camera::Camera()
{
	position = glm::vec3(0.0f, 0.0f, 1.0f); // 카메라 위치
	front = glm::vec3(0.0f, 0.0f, -1.0f);   // 카메라가 바라보는 방향
	up = glm::vec3(0.0f, 1.0f, 0.0f);       // 월드 업 벡터
	fov = 108.0f;
	sensitivity = 0.255f;        // 마우스 감도
	yaw = -90.0f; // 카메라의 Y축 회전 (수평) // 기본적으로 -Z축을 바라보도록 초기화 (-90도)
	pitch = 0.0f; // 카메라의 X축 회전 (수직) // 위아래 각도는 초기에는 수평 상태
	isMouseLocked = true;		// 마우스 고정 여부
	firstMouse = true;          // 마우스 초기 이동 감지
	ignoreMouseEvent = false; // 마우스 중앙 고정 이벤트 무시 플래그
}

void Camera::calculateCameraFront()
{
	glm::vec3 temp;

	// Yaw와 Pitch를 사용해 방향 벡터 계산
	temp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	temp.y = sin(glm::radians(pitch)); // pitch는 위아래 각도
	temp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)); // yaw는 좌우 회전

	// 앞 방향 벡터를 정규화
	front = glm::normalize(temp);
	std::cout << "front : " << front.x << ", " << front.y << ", " << front.z << std::endl;

	// Right 벡터 계산 (카메라의 오른쪽 방향)
	glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	// Up 벡터 계산 (카메라의 위 방향)
	up = glm::normalize(glm::cross(right, front));
}

void Camera::lockMouse() {
	if (isMouseLocked) {
		ignoreMouseEvent = true;
	}

	glutWarpPointer(SCREENWIDTH / 2, SCREENHEIGHT / 2); // 커서를 화면 중앙으로 이동
}

void Camera::setIsMouseLocked(bool temp) {
	isMouseLocked = temp;
}

void Camera::mouseCallback(GLfloat xPos, GLfloat yPos)
{
	if (isMouseLocked) {
		glutSetCursor(GLUT_CURSOR_NONE);

		// 화면 중앙을 기준으로 이동량 계산
		float xoffset = xPos - (SCREENWIDTH / 2);
		float yoffset = (SCREENHEIGHT / 2) - yPos; // 화면의 Y축은 아래로 증가

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		// Yaw는 좌우 회전, Pitch는 위/아래 회전
		yaw += xoffset;  // yaw: 좌우 회전 (수평 회전)
		pitch += yoffset; // pitch: 위/아래 회전 (수직 회전)

		std::cout << yaw << ", " << pitch << std::endl;

		// Pitch 제한 (위아래 회전 제한)
		if (pitch > 80.0f)
			pitch = 80.0f;
		if (pitch < -80.0f)
			pitch = -80.0f;
		// 방향 벡터 갱신
		calculateCameraFront();

		// 마우스를 다시 화면 중앙으로 고정
		lockMouse(); // lockMouse()는 glutWarpPointer 호출
	}
}