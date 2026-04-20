// ShapeMeshes.cpp
#include "ShapeMeshes.h"
#include <cmath>
#include <iostream>

const float PI = 3.14159265359f;

// --- CUBE ---
void DrawCubeMesh() {
    glutSolidCube(1.0);
}

// --- CONE ---
void DrawConeMesh(int slices) {
    glutSolidCone(0.5, 1.0, slices, slices);
}

// --- CYLINDER ---
void DrawCylinderMesh(int slices) {
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.5, 0.5, 1.0, slices, slices);
    gluDisk(quad, 0.0, 0.5, slices, 1);     // bottom
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.0f);
    gluDisk(quad, 0.0, 0.5, slices, 1);     // top
    glPopMatrix();
    gluDeleteQuadric(quad);
}

// --- PLANE ---
void DrawPlaneMesh() {
    glBegin(GL_QUADS);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f( 0.5f, 0.0f, -0.5f);
    glVertex3f( 0.5f, 0.0f,  0.5f);
    glVertex3f(-0.5f, 0.0f,  0.5f);
    glEnd();
}

// --- PRISM ---
void DrawPrismMesh() {
    // Simple triangular prism
    glBegin(GL_TRIANGLES); // two bases
    glVertex3f( 0.0f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f( 0.5f, 0.0f, 0.5f);

    glVertex3f( 0.0f, 0.5f, 0.0f);
    glVertex3f( 0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glEnd();

    glBegin(GL_QUADS); // sides
    glVertex3f(-0.5f, 0.0f,  0.5f);
    glVertex3f( 0.5f, 0.0f,  0.5f);
    glVertex3f( 0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glEnd();
}

// --- PYRAMID ---
void DrawPyramidMesh() {
    glBegin(GL_TRIANGLES);
    // Front
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f( 0.5f, 0.0f, 0.5f);
    // Right
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glVertex3f( 0.5f, 0.0f, 0.5f);
    glVertex3f( 0.5f, 0.0f, -0.5f);
    // Back
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glVertex3f( 0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    // Left
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glEnd();

    glBegin(GL_QUADS); // base
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f( 0.5f, 0.0f, -0.5f);
    glVertex3f( 0.5f, 0.0f,  0.5f);
    glVertex3f(-0.5f, 0.0f,  0.5f);
    glEnd();
}

// --- SPHERE ---
void DrawSphereMesh(int stacks, int slices) {
    glutSolidSphere(0.5f, slices, stacks);
}

// --- TAPERED CYLINDER ---
void DrawTaperedCylinderMesh(int slices) {
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.5, 0.2, 1.0, slices, slices);
    gluDeleteQuadric(quad);
}

// --- TORUS ---
void DrawTorusMesh(int numMajor, int numMinor) {
    glutSolidTorus(0.2, 0.5, numMinor, numMajor);
}

// --- OPTIONAL UTILITY FUNCTIONS ---

void CreateMeshVAO(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices, GLuint& vao, GLuint& vbo, GLuint& ebo) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Assume layout: position only (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void DrawMesh(GLuint vao, GLsizei indexCount) {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void DeleteMesh(GLuint& vao, GLuint& vbo, GLuint& ebo) {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    vao = vbo = ebo = 0;
}