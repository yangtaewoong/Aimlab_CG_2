#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include <iostream>
#include <vector>
#include <cmath>
#include "random.h"
#include "shaderSourceCode.h"
#include "crosshair.h"	//--- ũ�ν���� ��� �߰�
#include "pointcrosshair.h"
#include "emptycrosshair.h"

#define SCREENWIDTH 1980
#define SCREENHEIGHT 1080


// ����� ���� �Լ�
GLvoid Reshape(int w, int h); //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
GLvoid drawScene(GLvoid);
void Keyboard(unsigned char, int, int); // Ű �Է� �޴� �Լ�
void SpecialKeyboard(int key, int x, int y);
void TimerFunction(int); // Ÿ�̸� �Լ�
void Mouse(int, int, int, int);
void PassiveMotion(int x, int y);
void Motion(int x, int y);

// ����� ���� ���� ( �ʼ� )
GLuint shaderProgramID; // ���̴� ���α׷� �̸�
GLuint vertexShader; // ���ؽ� ���̴� ��ü
GLuint fragmentShader; // �����׸�Ʈ ���̴� ��ü
GLuint shaderProgramID_noneLight; // ���̴� ���α׷� �̸�
GLuint vertexShader_noneLight; // ���ؽ� ���̴� ��ü
GLuint fragmentShader_noneLight; // �����׸�Ʈ ���̴� ��ü
shape::Camera camera;
bool setTimer = true;
bool isLeftButtonClick;
bool isRightButtonClick;
GLuint vao, vbo[2], ebo;

// ����� ���� ���� ( �ʼ��ƴ� )
shape::DefaultShape line_x{};
shape::DefaultShape line_y{};
shape::DefaultShape line_z{};
std::vector<shape::Cube*> cubes;
bool isDepth = true;
bool isOrtho = false;
bool isSolid{};

//--- ũ�ν���� ��ü ����
Crosshair crosshair;	
PointCrosshair pointCrosshair(glm::vec3(0.0f, 1.0f, 0.0f), 5.0f); // ���, ũ�� 5.0
EmptyCrosshair emptyCrosshair(glm::vec3(0.0f, 1.0f, 0.0f), 0.02f, 0.005f);
int crosshairType = 1;

bool isObjectInCenter(const glm::vec3& objectPosition, const shape::Camera& camera,
	float aspectRatio,float angleThreshold = 0.01f, float screenDistanceThreshold = 0.04f) {
	// ī�޶� ��ġ���� �������� ���ϴ� ����
	glm::vec3 toObject = glm::normalize(objectPosition - camera.position);
	// ī�޶� �ü� ���Ϳ��� ���� Ȯ�� (dot product ���)
	float dotProduct = glm::dot(toObject, camera.front);

	if (dotProduct < (1.0f - angleThreshold)) {
		return false; // ������ ���� ���� �̻��̸� �߾ӿ� ���� ����
	}

	glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::vec4 clipSpacePos = projection * view * glm::vec4(objectPosition, 1.0f);
	glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w; // NDC ��ǥ ���
	glm::vec2 screenPos = glm::vec2(ndcPos.x, ndcPos.y); // ȭ�� �߽����κ����� ��� �Ÿ� (-1 ~ 1 ����)

	// ȭ�� �߽����κ����� �Ÿ� Ȯ��
	return glm::length(screenPos) <= screenDistanceThreshold;
}

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT); // �������� ũ�� ����
	glutCreateWindow("Example1"); // ������ ���� (������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	// ���� ����
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// ���̴� �о�ͼ� ���̴� ���α׷� �����
	make_shaderProgram(vertexShader, fragmentShader, shaderProgramID);
	make_shaderProgram_noneLight(vertexShader_noneLight, fragmentShader_noneLight, shaderProgramID_noneLight);
	glPointSize(1.f);
	glLineWidth(5.f);

	// ���ؼ� �ʱ�ȭ
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

	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸��Լ� �� ����

	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutMouseFunc(Mouse);
	glutPassiveMotionFunc(PassiveMotion);
	glutMotionFunc(Motion);
	glClearColor(1.f, 1.f, 1.f, 1.f); // �������� ��backGround���� ����
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	glClearColor(1.f, 1.f, 1.f, 1.f); // �������� ��backGround���� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.

	// ������ ���������ο� ���̴� �ҷ�����
	glUseProgram(shaderProgramID_noneLight);

	// �������------------------------------------------------

	// x y z ��
	DrawShape(camera, shaderProgramID_noneLight, vao, vbo, line_x, isOrtho);
	DrawShape(camera, shaderProgramID_noneLight, vao, vbo, line_y, isOrtho);
	DrawShape(camera, shaderProgramID_noneLight, vao, vbo, line_z, isOrtho);

		// ���� ���õ� ���ؼ� ������
	if (crosshairType == 1) {
		crosshair.Draw(shaderProgramID_noneLight, SCREENWIDTH, SCREENHEIGHT); // �⺻ ���� ���ؼ�
	}
	else if (crosshairType == 2) {
		pointCrosshair.Draw(shaderProgramID_noneLight, SCREENWIDTH, SCREENHEIGHT); // �� ���ؼ�
	}
	else if (crosshairType == 3) {
		emptyCrosshair.Draw(shaderProgramID_noneLight, SCREENWIDTH, SCREENHEIGHT);
	}

	glUseProgram(shaderProgramID);

	// ������ �׸��� �� �߾� �˻�
	bool objectInCenter = false;
	float asepectRatio = 1980.0f / 1080.0f;
	for (shape::Cube* cube : cubes) {
		DrawShape(camera, shaderProgramID, vao, vbo, &ebo, *cube, isOrtho);
		// ������ ȭ�� �߾ӿ� �ִ��� �˻�
		if (isObjectInCenter(cube->GetPosition(), camera, asepectRatio)) {
			objectInCenter = true;
			std::cout << "������ �νĵ�" << std::endl;
		}
	}

	// �������-----------------------------------------------

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1': // �⺻ ���� ���ؼ�
		crosshairType = 1;
		break;
	case '2': // �� ���ؼ�
		crosshairType = 2;
		break;
	case '3': // ����� ���ؼ�
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


void Mouse(int button, int state, int x, int y) // ���콺 ��ǥ�� ���� ���� 0,0 ������ �Ʒ��� 800,800 (â ũ��)
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
		glutPostRedisplay(); // ȭ�� �� ���
		glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸��Լ� �� ����
	}
}