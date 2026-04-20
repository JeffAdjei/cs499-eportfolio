// ShapeMeshes.h
#ifndef SHAPE_MESHES_H
#define SHAPE_MESHES_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

// Mesh generation functions for basic shapes
void DrawCubeMesh();
void DrawConeMesh(int slices = 32);
void DrawCylinderMesh(int slices = 32); // FIXED: Signature should match .cpp
void DrawPlaneMesh();
void DrawPrismMesh();
void DrawPyramidMesh();
void DrawSphereMesh(int stacks = 18, int slices = 36);
void DrawTaperedCylinderMesh(int slices = 32);
void DrawTorusMesh(int numMajor = 32, int numMinor = 32);

// Utility for drawing mesh with VAO/VBO
void CreateMeshVAO(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices, GLuint& vao, GLuint& vbo, GLuint& ebo);
void DrawMesh(GLuint vao, GLsizei indexCount);

// Cleanup utility
void DeleteMesh(GLuint& vao, GLuint& vbo, GLuint& ebo);

#endif