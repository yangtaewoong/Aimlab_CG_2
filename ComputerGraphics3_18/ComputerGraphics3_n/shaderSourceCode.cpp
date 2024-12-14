#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <iostream>
#include <print>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shaderSourceCode.h"

#define SCREENWIDTH 1980
#define SCREENHEIGHT 1080
#define MAX_LINE_LENGTH 1000

using namespace shape;

char* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;

	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;

	// Seek to the end of the file and get the length
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);

	// Handle empty or invalid file
	if (length < 0) {
		fclose(fptr);
		return NULL; // Return NULL for negative length
	}

	// Allocate buffer for the entire file plus a null terminator
	buf = (char*)malloc(length + 1);
	if (!buf) {
		fclose(fptr);
		return NULL; // Handle memory allocation failure
	}

	// Go back to the beginning of the file
	fseek(fptr, 0, SEEK_SET);

	// Read the contents of the file into the buffer
	size_t result = fread(buf, 1, length, fptr);
	if (result != length) {
		free(buf); // Free allocated buffer
		fclose(fptr); // Close the file
		return NULL; // Return NULL on read error
	}

	fclose(fptr); // Close the file
	buf[length] = '\0'; // Null terminator
	return buf; // Return the buffer
}

void free_model(shape::Model* model) {
	free(model->vertices);
	free(model->normals);
	free(model->faces);
	model->vertices = NULL;
	model->normals = NULL;
	model->faces = NULL;
	model->vertex_count = 0;
	model->normal_count = 0;
	model->face_count = 0;
}


void read_obj_file(const char* filename, shape::Model* model) {
	FILE* file = fopen(filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << filename << " 로딩 성공" << std::endl;
	}

	char line[MAX_LINE_LENGTH];
	model->vertex_count = 0;
	model->normal_count = 0;
	model->face_count = 0;

	// 1st pass: count vertices, normals, and faces
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ')
			model->vertex_count++;
		else if (line[0] == 'v' && line[1] == 'n')
			model->normal_count++;
		else if (line[0] == 'f' && line[1] == ' ')
			model->face_count++;
	}

	// Allocate memory
	model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));
	model->normals = (Vertex*)malloc(model->normal_count * sizeof(Vertex));
	model->faces = (Face*)malloc(model->face_count * 2 * sizeof(Face)); // Account for potential quadrilaterals

	if (!model->vertices || !model->normals || !model->faces) {
		perror("Error allocating memory");
		fclose(file);
		exit(EXIT_FAILURE);
	}

	// 2nd pass: parse vertices, normals, and faces
	fseek(file, 0, SEEK_SET);
	size_t vertex_index = 0, normal_index = 0, face_index = 0;

	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			sscanf(line + 2, "%f %f %f", &model->vertices[vertex_index].x,
				&model->vertices[vertex_index].y,
				&model->vertices[vertex_index].z);
			vertex_index++;
		}
		else if (line[0] == 'v' && line[1] == 'n') {
			sscanf(line + 3, "%f %f %f", &model->normals[normal_index].x,
				&model->normals[normal_index].y,
				&model->normals[normal_index].z);
			normal_index++;
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			unsigned int v[4], n[4];
			int matches = sscanf(line + 2, "%u//%u %u//%u %u//%u %u//%u",
				&v[0], &n[0], &v[1], &n[1],
				&v[2], &n[2], &v[3], &n[3]);

			if (matches == 6) { // Triangle
				model->faces[face_index].v1 = v[0] - 1;
				model->faces[face_index].v2 = v[1] - 1;
				model->faces[face_index].v3 = v[2] - 1;
				face_index++;
			}
			else if (matches == 8) { // Quadrilateral
				// Split into two triangles
				model->faces[face_index].v1 = v[0] - 1;
				model->faces[face_index].v2 = v[1] - 1;
				model->faces[face_index].v3 = v[2] - 1;
				face_index++;

				model->faces[face_index].v1 = v[0] - 1;
				model->faces[face_index].v2 = v[2] - 1;
				model->faces[face_index].v3 = v[3] - 1;
				face_index++;
			}
		}
	}

	fclose(file);
}


void make_vertexShaders(GLuint& vertexShader) // 버텍스 세이더 객체)
{
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
	vertexSource = filetobuf("vertex.glsl");

	// 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, NULL);

	// 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);

	// 에러체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_vertexShaders_noneLight(GLuint& vertexShader) // 버텍스 세이더 객체)
{
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
	vertexSource = filetobuf("vertex_nonelight.glsl");

	// 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, NULL);

	// 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);

	// 에러체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders(GLuint& fragmentShader)
{
	GLchar* fragmentSource;

	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기

	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기

	// 프레그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, NULL);

	// 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);

	// 에러체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders_noneLight(GLuint& fragmentShader)
{
	GLchar* fragmentSource;

	fragmentSource = filetobuf("fragment_nonelight.glsl"); // 프래그세이더 읽어오기

	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기

	// 프레그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, NULL);

	// 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);

	// 에러체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_shaderProgram(GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderID)
{
	make_vertexShaders(vertexShader);
	make_fragmentShaders(fragmentShader);

	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기

	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기

	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기

	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
	}

	glUseProgram(shaderID);
	//--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
}

void make_shaderProgram_noneLight(GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderID)
{
	make_vertexShaders_noneLight(vertexShader);
	make_fragmentShaders_noneLight(fragmentShader);

	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기

	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기

	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기

	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
	}

	glUseProgram(shaderID);
	//--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
}

void InitBuffer(GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape)
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기

	glBindVertexArray(vao); //--- VAO를 바인드하기

	glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 10, defaultShape.vertex, GL_STATIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);
	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 10, defaultShape.color, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightPosLocation, 0.0f, 0.0f, 0.0f);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, 0.0f, 0.0f, 0.0f);
	unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(objColorLocation, defaultShape.color->x, defaultShape.color->y, defaultShape.color->z);
}

void InitBuffer_noneLight(GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape)
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기

	glBindVertexArray(vao); //--- VAO를 바인드하기

	glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 10, defaultShape.vertex, GL_STATIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);
	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 10, defaultShape.color, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);
}

void InitBuffer(GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape) {
	glGenVertexArrays(1, &vao); // VAO 생성
	glBindVertexArray(vao);     // VAO 바인드

	glGenBuffers(2, vbo); // 두 개의 VBO 생성
	glGenBuffers(1, ebo); // 하나의 EBO 생성

	// EBO에 face 데이터를 복사
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Face) * defaultShape.model.face_count, defaultShape.model.faces, GL_STATIC_DRAW);

	// VBO에 vertex 데이터를 복사
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * defaultShape.model.vertex_count, defaultShape.model.vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0); // Attribute 0 활성화

	// VBO에 normal 데이터를 복사
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * defaultShape.model.normal_count, defaultShape.model.normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1); // Attribute 1 활성화

	// Uniform 설정
	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos");
	glUniform3f(lightPosLocation, 10.0f, 10.0f, 5.0f);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor");
	glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
	unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(objColorLocation, defaultShape.color->x, defaultShape.color->y, defaultShape.color->z);
}


void InitBuffer_noneLight(GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape)
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기

	glBindVertexArray(vao); //--- VAO를 바인드하기

	glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
	glGenBuffers(1, ebo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Face) * defaultShape.model.face_count, defaultShape.model.faces, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * defaultShape.model.vertex_count, defaultShape.model.vertices, GL_STATIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);
	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * defaultShape.model.vertex_count, defaultShape.color, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);
}

void SetCamera(shape::Camera camera, GLuint& shaderID, bool isOrtho)
{
	// 뷰 및 프로젝션 초기화
	glm::mat4 view = glm::lookAt(
		camera.position, // 카메라 위치 (x, y, z)
		camera.position + camera.front, // 카메라가 바라보는 점 (x, y, z)
		camera.up // 월드 업 벡터 (여기서는 y축)
	);

	glm::mat4 projection;

	if (isOrtho) {
		projection = glm::ortho(
			-1.0f, 1.0f, // 왼쪽과 오른쪽 평면
			-1.0f, 1.0f, // 아래와 위 평면
			0.1f, 100.0f // 가까운 클립 평면과 먼 클립 평면
		);
	}
	else {
		projection = glm::perspective(
			glm::radians(camera.fov), // 시야각 (FOV)
			(float)SCREENWIDTH / (float)SCREENHEIGHT, // 종횡비
			0.001f, // 가까운 클립 평면
			100.0f // 먼 클립 평면
		);
	}

	GLint viewLoc = glGetUniformLocation(shaderID, "view");
	GLint projectionLoc = glGetUniformLocation(shaderID, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void DrawShape(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape, bool isOrtho)
{
	InitBuffer_noneLight(shaderID, vao, vbo, defaultShape);
	SetCamera(camera, shaderID, isOrtho);

	unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glm::mat4 temp = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(temp)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao);

	switch (defaultShape.cnt) {
	case 1:
		glDrawArrays(GL_POINTS, 0, 1);
		break;
	case 2:
		glDrawArrays(GL_LINES, 0, 2);
		break;
	case 3:
		glDrawArrays(GL_TRIANGLES, 0, 3);
		break;
	case 4:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		break;
	case 5:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
		break;
	case 6:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
		break;
	case 7:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 7);
		break;
	case 8:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
		break;
	case 9:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 9);
		break;
	case 10:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 10);
		break;
	}
}

void DrawShape(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, bool isOrtho) {
	SetCamera(camera, shaderID, isOrtho);
	InitBuffer(shaderID, vao, vbo, ebo, defaultShape);

	// 뷰 및 프로젝션 설정
	unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");
	glm::mat4 temp = defaultShape.transform_change * defaultShape.rotation_mom *
		defaultShape.rotation_world * defaultShape.transform_world *
		defaultShape.rotation_self * defaultShape.transform_self * defaultShape.scale;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(temp));

	glBindVertexArray(vao);

	// 각 face를 삼각형 두 개로 나눴으므로 총 인덱스 개수는 face_count * 6
	int totalTriangles = 2 * defaultShape.model.face_count; // 삼각형 개수
	glDrawElements(GL_TRIANGLES, 3 * totalTriangles, GL_UNSIGNED_INT, 0);
}


void DrawShape_Face(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, int faceNum, bool isOrtho)
{
	SetCamera(camera, shaderID, isOrtho);
	InitBuffer(shaderID, vao, vbo, ebo, defaultShape);
	// 뷰 및 프로젝션 초기화

	unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glm::mat4 temp = defaultShape.transform_change * defaultShape.rotation_mom * defaultShape.rotation_world * defaultShape.transform_world * defaultShape.rotation_self * defaultShape.transform_self * defaultShape.scale;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(temp)); //--- modelTransform 변수에 변환 값 적용하기

	int faceIndex = faceNum; // 0부터 시작하여, 두 번째 면을 선택
	int offset = faceIndex * defaultShape.faceVertexCnt * sizeof(GLuint); // 시작 인덱스의 바이트 오프셋 계산

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, defaultShape.faceVertexCnt, GL_UNSIGNED_INT, (void*)offset);
}

void DrawShape_Line(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, bool isOrtho)
{
	SetCamera(camera, shaderID, isOrtho);

	InitBuffer(shaderID, vao, vbo, ebo, defaultShape);
	// 뷰 및 프로젝션 초기화

	unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glm::mat4 temp = defaultShape.transform_change * defaultShape.rotation_mom * defaultShape.rotation_world * 
		defaultShape.transform_world * defaultShape.rotation_self * defaultShape.transform_self * defaultShape.scale;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(temp)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao);
	glDrawElements(GL_LINE_STRIP, 3 * defaultShape.model.face_count, GL_UNSIGNED_INT, 0);
}

void DrawShape_Face_Line(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, GLuint* ebo, shape::Cube defaultShape, int faceNum, bool isOrtho)
{
	SetCamera(camera, shaderID, isOrtho);

	InitBuffer(shaderID, vao, vbo, ebo, defaultShape);
	// 뷰 및 프로젝션 초기화

	unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glm::mat4 temp = defaultShape.transform_change * defaultShape.rotation_mom * defaultShape.rotation_world * 
		defaultShape.transform_world * defaultShape.rotation_self * defaultShape.transform_self * defaultShape.scale;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(temp)); //--- modelTransform 변수에 변환 값 적용하기

	int faceIndex = faceNum; // 0부터 시작하여, 두 번째 면을 선택
	int offset = faceIndex * defaultShape.faceVertexCnt * sizeof(GLuint); // 시작 인덱스의 바이트 오프셋 계산

	glBindVertexArray(vao);
	glDrawElements(GL_LINE_STRIP, defaultShape.faceVertexCnt, GL_UNSIGNED_INT, (void*)offset);
}

void DrawShape_NoneCamera(shape::Camera camera, GLuint& shaderID, GLuint& vao, GLuint* vbo, shape::DefaultShape defaultShape, bool isOrtho)
{
	InitBuffer(shaderID, vao, vbo, defaultShape);

	unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glm::mat4 temp = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(temp)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao);

	switch (defaultShape.cnt) {
	case 1:
		glDrawArrays(GL_POINTS, 0, 1);
		break;
	case 2:
		glDrawArrays(GL_LINES, 0, 2);
		break;
	case 3:
		glDrawArrays(GL_TRIANGLES, 0, 3);
		break;
	case 4:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		break;
	case 5:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
		break;
	case 6:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
		break;
	case 7:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 7);
		break;
	case 8:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
		break;
	case 9:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 9);
		break;
	case 10:
		glDrawArrays(GL_TRIANGLE_FAN, 0, 10);
		break;
	}
}