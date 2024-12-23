#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <iostream>
#include <vector>
#include <cmath>
#include "random.h"
#include "shaderSourceCode.h"
#include "crosshair.h"	//--- 크로스헤어 헤더 추가
#include "pointcrosshair.h"
#include "emptycrosshair.h"

#define SCREENWIDTH 1980
#define SCREENHEIGHT 1080


// 사용자 정의 함수
GLvoid Reshape(int w, int h); //--- 콜백 함수: 다시 그리기 콜백 함수
GLvoid drawScene(GLvoid);
void Keyboard(unsigned char, int, int); // 키 입력 받는 함수
void SpecialKeyboard(int key, int x, int y);
void TimerFunction(int); // 타이머 함수
void Mouse(int, int, int, int);
void PassiveMotion(int x, int y);
void Motion(int x, int y);

// 사용자 정의 변수 ( 필수 )
GLuint shaderProgramID; // 세이더 프로그램 이름
GLuint vertexShader; // 버텍스 세이더 객체
GLuint fragmentShader; // 프래그먼트 세이더 객체
GLuint shaderProgramID_noneLight; // 세이더 프로그램 이름
GLuint vertexShader_noneLight; // 버텍스 세이더 객체
GLuint fragmentShader_noneLight; // 프래그먼트 세이더 객체
shape::Camera camera;
bool setTimer = true;
bool isLeftButtonClick;
bool isRightButtonClick;
GLuint vao, vbo[2], ebo;

// 사용자 정의 변수 ( 필수아님 )
shape::DefaultShape line_x{};
shape::DefaultShape line_y{};
shape::DefaultShape line_z{};
std::vector<shape::Cube*> cubes;
bool isDepth = true;
bool isOrtho = false;
bool isSolid{};

//--- 크로스헤어 객체 선언
Crosshair crosshair;	
PointCrosshair pointCrosshair(glm::vec3(0.0f, 1.0f, 0.0f), 5.0f); // 녹색, 크기 5.0
EmptyCrosshair emptyCrosshair(glm::vec3(0.0f, 1.0f, 0.0f), 0.02f, 0.005f);
int crosshairType = 1;

bool isObjectInCenter(const glm::vec3& objectPosition, const shape::Camera& camera,
	float aspectRatio,float angleThreshold = 0.01f, float screenDistanceThreshold = 0.04f) {
	// 카메라 위치에서 도형으로 향하는 벡터
	glm::vec3 toObject = glm::normalize(objectPosition - camera.position);
	// 카메라 시선 벡터와의 각도 확인 (dot product 사용)
	float dotProduct = glm::dot(toObject, camera.front);

	if (dotProduct < (1.0f - angleThreshold)) {
		return false; // 각도가 일정 범위 이상이면 중앙에 있지 않음
	}

	glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::vec4 clipSpacePos = projection * view * glm::vec4(objectPosition, 1.0f);
	glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w; // NDC 좌표 계산
	glm::vec2 screenPos = glm::vec2(ndcPos.x, ndcPos.y); // 화면 중심으로부터의 상대 거리 (-1 ~ 1 범위)

	// 화면 중심으로부터의 거리 확인
	return glm::length(screenPos) <= screenDistanceThreshold;
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	// 상태 관리
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// 세이더 읽어와서 세이더 프로그램 만들기
	make_shaderProgram(vertexShader, fragmentShader, shaderProgramID);
	make_shaderProgram_noneLight(vertexShader_noneLight, fragmentShader_noneLight, shaderProgramID_noneLight);
	glPointSize(1.f);
	glLineWidth(5.f);

	// 조준선 초기화
	crosshair = Crosshair(glm::vec3(1.0f, 1.0f, 1.0f), 0.3f); 
	crosshair.Init(shaderProgramID_noneLight);
	emptyCrosshair.Init(shaderProgramID_noneLight);
	pointCrosshair.Init(shaderProgramID_noneLight);

	camera.lockMouse();

	line_x.SetVertex(glm::vec3(1.0f, 0.0f, 0.0f), 0);
	line_x.SetVertex(glm::vec3(-1.0f, 0.0f, 0.0f), 1);
	line_y.SetVertex(glm::vec3(0.0f, 1.0f, 0.0f), 0);
	line_y.SetVertex(glm::vec3(0.0f, -1.0f, 0.0f), 1);
	line_z.SetVertex(glm::vec3(0.0f, 0.0f, 1.0f), 0);
	line_z.SetVertex(glm::vec3(0.0f, 0.0f, -1.0f), 1);
	line_x.SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), 0);
	line_x.SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), 1);
	line_y.SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), 0);
	line_y.SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), 1);
	line_z.SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 0.5f), 0);
	line_z.SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 0.5f), 1);
	line_x.cnt = 2;
	line_y.cnt = 2;
	line_z.cnt = 2;

	//for (int i{}; i < 11; ++i) {
	//	for (int j{}; j < 11; ++j) {
	//		shape::Cube* cube = new shape::Cube{ "sphere.txt", 0 };
	//		cube->Transform_World(glm::vec3(-1.0f + (i * 0.2f), -1.0f + (j * 0.2f), 0.6f));
	//		cube->Scale(glm::vec3(0.02f, 0.02f, 0.02f));
	//		cubes.push_back(cube);
	//	}
	//}

	shape::Cube* cube = new shape::Cube{ "sphere.txt", 0 };
	cube->model.face_count *= 2;
	cube->Transform_World(glm::vec3(0.0f, 0.0f, 0.6f));
	cube->Scale(glm::vec3(0.2f, 0.2f, 0.2f));
	cubes.push_back(cube);

	glutTimerFunc(60, TimerFunction, 1); // 타이머함수 재 설정

	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutMouseFunc(Mouse);
	glutPassiveMotionFunc(PassiveMotion);
	glutMotionFunc(Motion);
	glClearColor(1.f, 1.f, 1.f, 1.f); // 바탕색을 ‘backGround’로 지정
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(1.f, 1.f, 1.f, 1.f); // 바탕색을 ‘backGround’로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.

	// 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID_noneLight);

	// 여기부터------------------------------------------------

	// x y z 축
	DrawShape(camera, shaderProgramID_noneLight, vao, vbo, line_x, isOrtho);
	DrawShape(camera, shaderProgramID_noneLight, vao, vbo, line_y, isOrtho);
	DrawShape(camera, shaderProgramID_noneLight, vao, vbo, line_z, isOrtho);

		// 현재 선택된 조준선 렌더링
	if (crosshairType == 1) {
		crosshair.Draw(shaderProgramID_noneLight, SCREENWIDTH, SCREENHEIGHT); // 기본 십자 조준선
	}
	else if (crosshairType == 2) {
		pointCrosshair.Draw(shaderProgramID_noneLight, SCREENWIDTH, SCREENHEIGHT); // 점 조준선
	}
	else if (crosshairType == 3) {
		emptyCrosshair.Draw(shaderProgramID_noneLight, SCREENWIDTH, SCREENHEIGHT);
	}

	glUseProgram(shaderProgramID);

	// 도형들 그리기 및 중앙 검사
	bool objectInCenter = false;
	float asepectRatio = 1980.0f / 1080.0f;
	for (shape::Cube* cube : cubes) {
		DrawShape(camera, shaderProgramID, vao, vbo, &ebo, *cube, isOrtho);
		// 도형이 화면 중앙에 있는지 검사
		if (isObjectInCenter(cube->GetPosition(), camera, asepectRatio)) {
			objectInCenter = true;
			std::cout << "도형이 인식됨" << std::endl;
		}
	}

	// 여기까지-----------------------------------------------

	glutSwapBuffers(); // 화면에 출력하기
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1': // 기본 십자 조준선
		crosshairType = 1;
		break;
	case '2': // 점 조준선
		crosshairType = 2;
		break;
	case '3': // 빈십자 조준선
		crosshairType = 3;
		break;
	case 'i':
		camera.setIsMouseLocked(false);
		break;
	case 'I':
		camera.setIsMouseLocked(true);
		break;
	case 'a':
		camera.position.x -= 0.1f;
		camera.front.x -= 0.1f;
		camera.up.x -= 0.1f;
		break;
	case 'd':
		camera.position.x += 0.1f;
		camera.front.x += 0.1f;
		camera.up.x += 0.1f;
		break;
	case 'w':
		camera.position.z += 0.1f;
		camera.front.z += 0.1f;
		camera.up.z += 0.1f;
		break;
	case 's':
		camera.position.z -= 0.1f;
		camera.front.z -= 0.1f;
		camera.up.z -= 0.1f;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}

	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {
	switch (key) {
	}
	glutPostRedisplay();
}


void Mouse(int button, int state, int x, int y) // 마우스 좌표는 왼쪽 위가 0,0 오른쪽 아래가 800,800 (창 크기)
{
	GLfloat xPos = (2.0f * x) / SCREENWIDTH - 1.0f;
	GLfloat yPos = 1.0f - (2.0f * y) / SCREENHEIGHT;

	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			isLeftButtonClick = true;
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			isRightButtonClick = true;
		}
	}
	else if (state == GLUT_UP)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			isLeftButtonClick = false;
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			isRightButtonClick = false;
		}
	}

	glutPostRedisplay();
}

void PassiveMotion(int x, int y)
{
	GLfloat xPos = (2.0f * x) / SCREENWIDTH - 1.0f;
	GLfloat yPos = 1.0f - (2.0f * y) / SCREENHEIGHT;

	camera.mouseCallback(x, y);

	glutPostRedisplay();
}

void Motion(int x, int y)
{
	GLfloat xPos = (2.0f * x) / SCREENWIDTH - 1.0f;	
	GLfloat yPos = 1.0f - (2.0f * y) / SCREENHEIGHT;

	camera.mouseCallback(x, y);	

	if (isLeftButtonClick == true)
	{
	}
	else if (isRightButtonClick == true)
	{
	}

	glutPostRedisplay();
}

void TimerFunction(int value)
{
	if (setTimer)
	{
		switch (value) {
		case 1:
			break;
		}
		glutPostRedisplay(); // 화면 재 출력
		glutTimerFunc(60, TimerFunction, 1); // 타이머함수 재 설정
	}
}