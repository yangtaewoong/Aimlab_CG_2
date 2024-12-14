#pragma once
#include "crosshair.h"
#include "shaderSourceCode.h"

Crosshair::Crosshair() : color(glm::vec3(0.0f, 0.0f, 0.0f)), size(0.02f) {}

Crosshair::Crosshair(glm::vec3 color, GLfloat size) : color(color), size(size) {}

void Crosshair::Init(GLuint shaderProgramID) {
    // ũ�ν���� ���� ��ǥ (NDC ��ǥ��)
    float crosshairVertices[] = {
        -0.02f,  0.0f, 0.0f, // ���� ��
         0.02f,  0.0f, 0.0f, // ������ ��
         0.0f, -0.02f, 0.0f, // �Ʒ��� ��
         0.0f,  0.02f, 0.0f  // ���� ��
    };

    float crosshairColors[] = {
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f, 
    0.0f, 1.0f, 0.0f, 1.0f, 
    0.0f, 1.0f, 0.0f, 1.0f  
    };

    glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�

    glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�

    glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
    //--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    //--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    //--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //--- attribute �ε��� 0���� ��밡���ϰ� ��
    glEnableVertexAttribArray(0);
    //--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    //--- ���� colors���� ���ؽ� ������ �����Ѵ�.
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairColors), crosshairColors, GL_STATIC_DRAW);
    //--- ������ attribute �ε��� 1���� ����Ѵ�
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    //--- attribute �ε��� 1���� ��� �����ϰ� ��.
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgramID);
    int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
    glUniform3f(lightPosLocation, 0.0f, 0.0f, 0.0f);
    int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
    glUniform3f(lightColorLocation, 0.0f, 0.0f, 0.0f);
    unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
    glUniform3f(objColorLocation, color.x, color.y, color.z);
}

void Crosshair::SetColor(glm::vec3 newColor) {
    color = newColor;
}

void Crosshair::SetSize(GLfloat newSize) {
    size = newSize;

    // ������Ʈ�� ũ��� ���� �缳��
    vertices[0] = -size; vertices[1] = 0.0f; vertices[2] = 0.0f;
    vertices[3] = size; vertices[4] = 0.0f; vertices[5] = 0.0f;
    vertices[6] = 0.0f; vertices[7] = -size; vertices[8] = 0.0f;
    vertices[9] = 0.0f; vertices[10] = size; vertices[11] = 0.0f;

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Crosshair::Draw(GLuint shaderProgramID, int windowWidth, int windowHeight) {
    glUseProgram(shaderProgramID);

    // ���� ���
    float aspectRatio = (float)windowWidth / (float)windowHeight;

    // ��� ����
    glm::mat4 projection = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f); // �⺻ ���� ���
    glm::mat4 model = glm::mat4(1.0f); // �⺻ ���� ���

    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");
    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "modelTransform");

    // ���� ��ķ� ����
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // ũ�ν���� �׸���
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);
}


