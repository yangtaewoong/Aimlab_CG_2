#pragma once
#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

class EmptyCrosshair {
private:
    GLuint vao, vbo[2];           // VAO와 VBO
    glm::vec3 color;           // 조준선 색상
    GLfloat size;              // 조준선 크기
    GLfloat gap;               // 가운데 비어 있는 간격

public:
    EmptyCrosshair();                                    // 기본 생성자
    EmptyCrosshair(glm::vec3 color, GLfloat size, GLfloat gap); // 색상, 크기, 간격 설정

    void Init(GLuint shaderProgramID);                  // VAO, VBO 초기화
    void SetColor(glm::vec3 newColor);                  // 색상 변경
    void SetSize(GLfloat newSize);                      // 크기 변경
    void Draw(GLuint shaderProgramID, int windowWidth, int windowHeight);  // 조준선 그리기
};