#include "cube.h"
#include "shaderSourceCode.h"

using namespace shape;

Cube::Cube()
{
	read_obj_file("cube.txt", &model); // cube.obj를 읽어온다

    if (model.face_count > 0) {
        color = new glm::vec4[model.vertex_count];

        for (int i = 0; i < model.vertex_count; i++) {
            color[i] = glm::vec4(1.0f);
        }
    }

    for (int i = 0; i < model.vertex_count; i++) {
        color[i] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    transform_self = glm::mat4(1.0f);
    transform_world = glm::mat4(1.0f);
    transform_change = glm::mat4(1.0f);
    rotation_self = glm::mat4(1.0f);
    rotation_world = glm::mat4(1.0f);
    rotation_mom = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);

    faceVertexCnt = 6;
}

Cube::Cube(const char* filename, int cnt)
{
    read_obj_file(filename, &model); // cube.obj를 읽어온다

    if (model.vertex_count > 0) {
        color = new glm::vec4[model.vertex_count];

        for (int i = 0; i < model.vertex_count; i++) {
            color[i] = glm::vec4(1.0f);
        }
    }

    for (int i = 0; i < model.vertex_count; i++) {
        color[i] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    transform_self = glm::mat4(1.0f);
    transform_world = glm::mat4(1.0f);
    transform_change = glm::mat4(1.0f);
    rotation_self = glm::mat4(1.0f);
    rotation_world = glm::mat4(1.0f);
    rotation_mom = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);

    faceVertexCnt = cnt;
}

Cube::~Cube()
{

}

void Cube::DrawShape()
{
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < model.face_count; ++i) {
        Face face = model.faces[i];

        // Draw each vertex of the face with its corresponding normal
       // glNormal3f(model.normals[face.n1].x, model.normals[face.n1].y, model.normals[face.n1].z);
        glVertex3f(model.vertices[face.v1].x, model.vertices[face.v1].y, model.vertices[face.v1].z);

       // glNormal3f(model.normals[face.n2].x, model.normals[face.n2].y, model.normals[face.n2].z);
        glVertex3f(model.vertices[face.v2].x, model.vertices[face.v2].y, model.vertices[face.v2].z);

       // glNormal3f(model.normals[face.n3].x, model.normals[face.n3].y, model.normals[face.n3].z);
        glVertex3f(model.vertices[face.v3].x, model.vertices[face.v3].y, model.vertices[face.v3].z);
    }
    glEnd();
}

// glm::mat4 glm::rotate(glm::mat4 const& m, float angle, glm::vec3 const& axis);
// glm::mat4 glm::scale(glm::mat4 const& m, glm::vec3 const& factors);
// glm::mat4 glm::translate(glm::mat4 const& m, glm::vec3 const& translation);

void Cube::Rotate_Self(GLfloat angle, glm::vec3 temp)
{
    if (model.vertex_count > 0) {
         rotation_self = glm::rotate(rotation_self, glm::radians(angle), temp);
    }
}

void Cube::Rotate_World(GLfloat angle, glm::vec3 temp)
{
    if (model.vertex_count > 0) {
        rotation_world = glm::rotate(rotation_world, glm::radians(angle), temp);
    }
}

void Cube::Rotate_Mom(GLfloat angle, glm::vec3 temp)
{
    if (model.vertex_count > 0) {
        rotation_mom = glm::rotate(rotation_mom, glm::radians(angle), temp);
    }
}

void Cube::Transform_Self(glm::vec3 temp)
{
    if (model.vertex_count > 0) {
        glm::mat4 transMatrix = glm::mat4(1.0f);
        transMatrix = glm::translate(transMatrix, temp);
        transform_self = transMatrix * transform_self;
    }
}

void Cube::Transform_World(glm::vec3 temp)
{
    if (model.vertex_count > 0) {
        glm::mat4 transMatrix = glm::mat4(1.0f);
        transMatrix = glm::translate(transMatrix, temp);
        transform_world = transMatrix * transform_world;
    }
}

void Cube::Transform_change(glm::vec3 temp)
{
    if (model.vertex_count > 0) {
        glm::mat4 transMatrix = glm::mat4(1.0f);
        transMatrix = glm::translate(transMatrix, temp);
        transform_change = transMatrix * transform_change;
    }
}

void Cube::Scale(glm::vec3 temp)
{
    if (model.vertex_count > 0) {
        glm::mat4 scaleMatrix = glm::mat4(1.0f);
        scale = glm::scale(scaleMatrix, temp);
        scale = scaleMatrix * scale;
    }
}