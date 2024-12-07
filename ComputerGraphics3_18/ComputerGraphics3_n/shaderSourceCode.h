#pragma once
#include "shape.h"
#include "DefaultShape.h"
#include "cube.h"

// 세이더 함수
char* filetobuf(const char* file);
void free_model(shape::Model* model);
void read_obj_file(const char* filename, shape::Model* model);
void make_vertexShaders(GLuint& vertexShader);
void make_fragmentShaders(GLuint& fragmentShader);
void make_shaderProgram(GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderID);

// 버퍼 함수
void InitBuffer(GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape);
void InitBuffer(GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape);

// 카메라 설정 함수
void SetCamera(shape::Camera camera, GLuint& shaderID, bool isOrtho);

// Draw 함수
void DrawShape(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape, bool isOrtho);
void DrawShape(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, bool isOrtho);

// Draw 특수 함수
void DrawShape_Face(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, int faceNum, bool isOrtho);
void DrawShape_Line(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, bool isOrtho);
void DrawShape_Face_Line(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, int faceNum, bool isOrtho);
void DrawShape_NoneCamera(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape, bool isOrtho);