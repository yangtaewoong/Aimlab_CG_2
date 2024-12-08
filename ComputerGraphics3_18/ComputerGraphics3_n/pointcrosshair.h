#pragma once
#include <iostream>
#include <gl/glew.h> 
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

class PointCrosshair {
private:
    GLuint vao, vbo;         // VAO와 VBO
    glm::vec3 color;         // 조준선 색상
    GLfloat size;            // 조준선 크기

    // 유니폼 변수 위치
    GLint projectionLoc;              // 투영 행렬 위치
    GLint viewLoc;                    // 뷰 행렬 위치
    GLint modelLoc;                   // 모델 변환 행렬 위치
    GLint colorLoc;                   // 색상 유니폼 위치

public:
    PointCrosshair(); // 기본 생성자
    PointCrosshair(glm::vec3 color, GLfloat size);

    void Init(GLuint shaderProgramID);
    void SetColor(glm::vec3 newColor);
    void SetSize(GLfloat newSize);
    void Draw(GLuint shaderProgramID, int windowWidth, int windowHeight);
};
