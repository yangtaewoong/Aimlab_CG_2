#include "emptycrosshair.h"

// 기본 생성자
EmptyCrosshair::EmptyCrosshair()
    : color(glm::vec3(1.0f, 1.0f, 1.0f)), size(0.1f), gap(0.02f), vao(0), vbo(0) {}

// 사용자 정의 생성자
EmptyCrosshair::EmptyCrosshair(glm::vec3 color, GLfloat size, GLfloat gap)
    : color(color), size(size), gap(gap), vao(0), vbo(0) {}

// 초기화 함수
void EmptyCrosshair::Init(GLuint shaderProgramID) {
    // 선으로 이루어진 십자형 조준선의 좌표
    GLfloat crosshairVertices[] = {
        // 수평선 (왼쪽)
        -size - gap, 0.0f, 0.0f,
        -gap,        0.0f, 0.0f,
        // 수평선 (오른쪽)
         gap,        0.0f, 0.0f,
         size + gap, 0.0f, 0.0f,
         // 수직선 (아래쪽)
           0.0f, -size - gap, 0.0f,
           0.0f, -gap,        0.0f,
           // 수직선 (위쪽)
             0.0f,  gap,        0.0f,
             0.0f,  size + gap, 0.0f
    };

    // 각 정점의 색상 (예: 검정색 RGBA)
    GLfloat crosshairColors[] = {
        0.0f, 0.0f, 0.0f, 1.0f, // 왼쪽 선
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f, // 오른쪽 선
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f, // 아래쪽 선
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f, // 위쪽 선
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(2, vbo);

    glBindVertexArray(vao);

    // 위치 데이터
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 색상 데이터
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairColors), crosshairColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


// 색상 변경
void EmptyCrosshair::SetColor(glm::vec3 newColor) {
    color = newColor;
}

// 크기 변경
void EmptyCrosshair::SetSize(GLfloat newSize) {
    size = newSize;
}

// 그리기 함수
void EmptyCrosshair::Draw(GLuint shaderProgramID, int windowWidth, int windowHeight) {
    glUseProgram(shaderProgramID);

    // 화면 비율 계산
    float aspectRatio = (float)windowWidth / (float)windowHeight;

    // 행렬 설정
    glm::mat4 projection = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");
    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "modelTransform");
    GLuint colorLoc = glGetUniformLocation(shaderProgramID, "color");

    // 행렬 전달
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    

    // 조준선 그리기
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 8); 
    glBindVertexArray(0);
}
