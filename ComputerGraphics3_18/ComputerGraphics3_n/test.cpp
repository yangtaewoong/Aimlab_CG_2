#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f); // 카메라 위치
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);   // 카메라가 바라보는 방향
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);       // 월드 업 벡터
GLfloat cameraYaw = -90.0f; // 카메라의 Y축 회전 (수평)
GLfloat cameraPitch = 0.0f; // 카메라의 X축 회전 (수직)
GLfloat lastX = 450.f, lastY = 450.f;  // 마우스 초기 위치 (윈도우 중심값으로 설정)
GLfloat sensitivity = 0.1f;        // 마우스 감도
GLfloat yaw = -90.0f; // 기본적으로 -Z축을 바라보도록 초기화 (-90도)
GLfloat pitch = 0.0f; // 위아래 각도는 초기에는 수평 상태
bool isMouseLocked = true; // 마우스 고정 여부
bool firstMouse = true;          // 마우스 초기 이동 감지
bool ignoreMouseEvent = false; // 마우스 중앙 고정 이벤트 무시 플래그