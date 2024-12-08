#pragma once
#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

class Crosshair {
private:
    GLuint vao, vbo[2];         // VAO와 VBO
    glm::vec3 color;         // 조준선 색상
    GLfloat size;            // 조준선 크기
    float vertices[12];

    // 유니폼 변수 위치
    GLint projectionLoc;              // 투영 행렬 위치
    GLint viewLoc;                    // 뷰 행렬 위치
    GLint modelLoc;                   // 모델 변환 행렬 위치
    GLint colorLoc;                   // 색상 유니폼 위치

public:
    Crosshair();                             // 기본 생성자
    Crosshair(glm::vec3 color, GLfloat size); // 색상과 크기를 설정하는 생성자

    void Init(GLuint shaderID);              // VAO/VBO 초기화
    void SetColor(glm::vec3 newColor);       // 색상 변경
    void SetSize(GLfloat newSize);           // 크기 변경
    void Draw(GLuint shaderProgramID, int windowWidth, int windowHeight);  // 조준선 그리기
};
