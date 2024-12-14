#pragma once
#include "shape.h"
#include "DefaultShape.h"
#include "cube.h"

// ���̴� �Լ�
char* filetobuf(const char* file);
void free_model(shape::Model* model);
void read_obj_file(const char* filename, shape::Model* model);
void make_vertexShaders(GLuint& vertexShader);
void make_vertexShaders_noneLight(GLuint& vertexShader);
void make_fragmentShaders(GLuint& fragmentShader);
void make_fragmentShaders_noneLight(GLuint& fragmentShader);
void make_shaderProgram(GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderID);
void make_shaderProgram_noneLight(GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderID);

// ���� �Լ�
void InitBuffer(GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape);
void InitBuffer(GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape);
void InitBuffer_noneLight(GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape);
void InitBuffer_noneLight(GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape);

// ī�޶� ���� �Լ�
void SetCamera(shape::Camera camera, GLuint& shaderID, bool isOrtho);

// Draw �Լ�
void DrawShape(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape, bool isOrtho);
void DrawShape(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, bool isOrtho);

// Draw Ư�� �Լ�
void DrawShape_Face(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, int faceNum, bool isOrtho);
void DrawShape_Line(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, bool isOrtho);
void DrawShape_Face_Line(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, int faceNum, bool isOrtho);
void DrawShape_NoneCamera(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape, bool isOrtho);