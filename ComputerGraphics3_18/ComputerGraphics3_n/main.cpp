#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <iostream>
#include <vector>
#include "random.h"
#include "shaderSourceCode.h"
#include "crosshair.h"	//--- 크로스헤어 헤더 추가
#include "pointcrosshair.h"
#include "emptycrosshair.h"

#define SCREENWIDTH 1920
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
shape::Camera camera;
bool setTimer = true;
bool isLeftButtonClick;
bool isRightButtonClick;
GLuint vao, vbo[2], ebo;

// 사용자 정의 변수 ( 필수아님 )
shape::DefaultShape line_x{};
shape::DefaultShape line_y{};
shape::DefaultShape line_z{};
shape::DefaultShape line_camera{};
std::vector<shape::Cube*> cubes;
bool isDepth = true;
bool isOrtho = false;
bool isSolid{};

//--- 크로스헤어 객체 선언
Crosshair crosshair;	
PointCrosshair pointCrosshair(glm::vec3(0.0f, 1.0f, 0.0f), 5.0f); // 녹색, 크기 5.0
EmptyCrosshair emptyCrosshair(glm::vec3(0.0f, 1.0f, 0.0f), 0.02f, 0.005f);
int crosshairType = 1;

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

	// 세이더 읽어와서 세이더 프로그램 만들기
	make_shaderProgram(vertexShader, fragmentShader, shaderProgramID);
	glPointSize(1.f);
	glLineWidth(5.f);

	// 조준선 초기화
	crosshair = Crosshair(glm::vec3(1.0f, 1.0f, 1.0f), 0.3f); 
	crosshair.Init(shaderProgramID); 
	emptyCrosshair.Init(shaderProgramID);
	//-------------
	pointCrosshair.Init(shaderProgramID);

	camera.lockMouse();

	line_x.SetVertex(glm::vec3(1.0f, 0.0f, 0.0f), 0);
	line_x.SetVertex(glm::vec3(-1.0f, 0.0f, 0.0f), 1);
	line_y.SetVertex(glm::vec3(0.0f, 1.0f, 0.0f), 0);
	line_y.SetVertex(glm::vec3(0.0f, -1.0f, 0.0f), 1);
	line_z.SetVertex(glm::vec3(0.0f, 0.0f, 1.0f), 0);
	line_z.SetVertex(glm::vec3(0.0f, 0.0f, -1.0f), 1);
	line_camera.SetVertex(camera.front - camera.position, 0);
	line_camera.SetVertex(camera.position, 1);
	line_x.SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), 0);
	line_x.SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), 1);
	line_y.SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), 0);
	line_y.SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), 1);
	line_z.SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 0.5f), 0);
	line_z.SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 0.5f), 1);
	line_camera.SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0);
	line_camera.SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1);
	line_x.cnt = 2;
	line_y.cnt = 2;
	line_z.cnt = 2;

	for (int i{}; i < 10; ++i) {
		for (int j{}; j < 10; ++j) {
			shape::Cube* cube = new shape::Cube{ "sphere.txt", 0 };
			cube->Transform_World(glm::vec3(-1.0f + (i * 0.2f), -1.0f + (j * 0.2f), 0.0f));
			cube->Scale(glm::vec3(0.02f, 0.02f, 0.02));
			cubes.push_back(cube);
		}
	}

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
	glUseProgram(shaderProgramID);

	// 여기부터------------------------------------------------

	// x y z 축
	DrawShape(camera, shaderProgramID, vao, vbo, line_x, isOrtho);
	DrawShape(camera, shaderProgramID, vao, vbo, line_y, isOrtho);
	DrawShape(camera, shaderProgramID, vao, vbo, line_z, isOrtho);
	DrawShape_NoneCamera(camera, shaderProgramID, vao, vbo, line_camera, isOrtho);

	for(shape::Cube* cube : cubes)
		DrawShape(camera, shaderProgramID, vao, vbo, &ebo, *cube, isOrtho);

	// 현재 선택된 조준선 렌더링
	if (crosshairType == 1) {
		crosshair.Draw(shaderProgramID, SCREENWIDTH, SCREENHEIGHT); // 기본 십자 조준선
	}
	else if (crosshairType == 2) {
		pointCrosshair.Draw(shaderProgramID, SCREENWIDTH, SCREENHEIGHT); // 점 조준선
	}
	else if (crosshairType == 3) {
		emptyCrosshair.Draw(shaderProgramID, SCREENWIDTH, SCREENHEIGHT);

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
	line_camera.SetVertex(glm::normalize(camera.front - camera.position), 0);
	line_camera.SetVertex(camera.position, 1);

	glutPostRedisplay();
}

void Motion(int x, int y)
{
	GLfloat xPos = (2.0f * x) / SCREENWIDTH - 1.0f;	
	GLfloat yPos = 1.0f - (2.0f * y) / SCREENHEIGHT;

	camera.mouseCallback(x, y);	
	line_camera.SetVertex(glm::normalize(camera.front - camera.position), 0);
	line_camera.SetVertex(camera.position, 1);

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