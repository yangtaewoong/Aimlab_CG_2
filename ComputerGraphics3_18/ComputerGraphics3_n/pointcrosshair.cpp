#include "PointCrosshair.h"

PointCrosshair::PointCrosshair() : color(glm::vec3(1.0f, 0.0f, 0.0f)), size(0.02f) {}

PointCrosshair::PointCrosshair(glm::vec3 color, GLfloat size) : color(color), size(size) {}

void PointCrosshair::Init(GLuint shaderProgramID) {
    // 점의 위치 데이터 (중앙)
    float pointVertex[] = {
        0.0f, 0.0f, 0.0f
    };

    // 점의 색상 데이터
    float pointColor[] = {
        color.r, color.g, color.b, 1.0f
    };

    // VAO, VBO 생성
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    // 위치 데이터
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertex) + sizeof(pointColor), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointVertex), pointVertex);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointVertex), sizeof(pointColor), pointColor);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)sizeof(pointVertex));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PointCrosshair::SetColor(glm::vec3 newColor) {
    color = newColor;

    float pointColor[] = { color.r, color.g, color.b, 1.0f };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 3, sizeof(pointColor), pointColor);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PointCrosshair::SetSize(GLfloat newSize) {
    size = newSize; // 점의 크기는 이후 렌더링 시 `glPointSize`로 설정
}

void PointCrosshair::Draw(GLuint shaderProgramID, int windowWidth, int windowHeight) {
    glUseProgram(shaderProgramID);

    // 비율 계산
    float aspectRatio = (float)windowWidth / (float)windowHeight;

    // 행렬 설정
    glm::mat4 projection = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");
    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "modelTransform");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // 점 크기 설정
    glPointSize(size);

    // 점 그리기
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
}
