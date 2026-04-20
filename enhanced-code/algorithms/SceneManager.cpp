///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ================
// Manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//  Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
//
//  Enhanced for CS 499 Milestone Three by Jeffrey Adjei:
//  - Refactored repeated render logic into structured scene object rendering
//  - Introduced vector-based scene organization for scalability
//  - Reduced duplicated transformation and draw code
//  - Improved maintainability and algorithmic clarity
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>

namespace
{
    const char* g_ModelName = "model";
    const char* g_ColorValueName = "objectColor";
    const char* g_TextureValueName = "objectTexture";
    const char* g_UseTextureName = "bUseTexture";
    const char* g_UseLightingName = "bUseLighting";

    /*
     * Scene object definition used to reduce repeated code and organize
     * renderable objects using a data structure.
     */
    struct SceneObject
    {
        std::string meshType;
        glm::vec3 scaleXYZ;
        float xRotation;
        float yRotation;
        float zRotation;
        glm::vec3 positionXYZ;
        glm::vec4 colorValue;
    };
}

/***********************************************************
 *  SceneManager Constructor
 ***********************************************************/
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
    m_pShaderManager = pShaderManager;
    m_basicMeshes = new ShapeMeshes();
    m_loadedTextures = 0;
}

/***********************************************************
 *  SceneManager Destructor
 ***********************************************************/
SceneManager::~SceneManager()
{
    DestroyGLTextures();

    m_pShaderManager = nullptr;

    delete m_basicMeshes;
    m_basicMeshes = nullptr;
}

/***********************************************************
 *  CreateGLTexture
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
    int width = 0;
    int height = 0;
    int colorChannels = 0;
    GLuint textureID = 0;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load(filename, &width, &height, &colorChannels, 0);

    if (image)
    {
        std::cout << "Loaded image: " << filename
                  << ", width: " << width
                  << ", height: " << height
                  << ", channels: " << colorChannels << std::endl;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (colorChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        }
        else if (colorChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }
        else
        {
            std::cout << "Unsupported channel count: " << colorChannels << std::endl;
            stbi_image_free(image);
            glBindTexture(GL_TEXTURE_2D, 0);
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_textureIDs[m_loadedTextures].ID = textureID;
        m_textureIDs[m_loadedTextures].tag = tag;
        m_loadedTextures++;

        return true;
    }

    std::cout << "Failed to load image: " << filename << std::endl;
    return false;
}

/***********************************************************
 *  BindGLTextures
 ***********************************************************/
void SceneManager::BindGLTextures()
{
    for (int i = 0; i < m_loadedTextures; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
    }
}

/***********************************************************
 *  DestroyGLTextures
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
    for (int i = 0; i < m_loadedTextures; ++i)
    {
        glDeleteTextures(1, &m_textureIDs[i].ID);
    }

    m_loadedTextures = 0;
}

/***********************************************************
 *  FindTextureID
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
    for (int i = 0; i < m_loadedTextures; ++i)
    {
        if (m_textureIDs[i].tag == tag)
        {
            return m_textureIDs[i].ID;
        }
    }

    return -1;
}

/***********************************************************
 *  FindTextureSlot
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
    for (int i = 0; i < m_loadedTextures; ++i)
    {
        if (m_textureIDs[i].tag == tag)
        {
            return i;
        }
    }

    return -1;
}

/***********************************************************
 *  FindMaterial
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
    for (const auto& mat : m_objectMaterials)
    {
        if (mat.tag == tag)
        {
            material = mat;
            return true;
        }
    }

    return false;
}

/***********************************************************
 *  SetTransformations
 ***********************************************************/
void SceneManager::SetTransformations(
    glm::vec3 scaleXYZ,
    float XrotationDegrees,
    float YrotationDegrees,
    float ZrotationDegrees,
    glm::vec3 positionXYZ)
{
    glm::mat4 modelView = glm::translate(positionXYZ)
        * glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1, 0, 0))
        * glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0, 1, 0))
        * glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0, 0, 1))
        * glm::scale(scaleXYZ);

    if (m_pShaderManager)
    {
        m_pShaderManager->setMat4Value(g_ModelName, modelView);
    }
}

/***********************************************************
 *  SetShaderColor
 ***********************************************************/
void SceneManager::SetShaderColor(float r, float g, float b, float a)
{
    if (m_pShaderManager)
    {
        m_pShaderManager->setIntValue(g_UseTextureName, false);
        m_pShaderManager->setVec4Value(g_ColorValueName, glm::vec4(r, g, b, a));
    }
}

/***********************************************************
 *  SetShaderTexture
 ***********************************************************/
void SceneManager::SetShaderTexture(std::string textureTag)
{
    if (m_pShaderManager)
    {
        m_pShaderManager->setIntValue(g_UseTextureName, true);
        int textureSlot = FindTextureSlot(textureTag);
        m_pShaderManager->setSampler2DValue(g_TextureValueName, textureSlot);
    }
}

/***********************************************************
 *  SetTextureUVScale
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
    if (m_pShaderManager)
    {
        m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
    }
}

/***********************************************************
 *  SetShaderMaterial
 ***********************************************************/
void SceneManager::SetShaderMaterial(std::string materialTag)
{
    OBJECT_MATERIAL material;

    if (FindMaterial(materialTag, material) && m_pShaderManager)
    {
        m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
        m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
        m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
        m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
        m_pShaderManager->setFloatValue("material.shininess", material.shininess);
    }
}

/***********************************************************
 *  PrepareScene
 ***********************************************************/
void SceneManager::PrepareScene()
{
    m_basicMeshes->LoadPlaneMesh();
    m_basicMeshes->LoadBoxMesh();
    m_basicMeshes->LoadCylinderMesh();
}

/***********************************************************
 *  DrawSceneObject
 *  Helper to render one object from the scene collection.
 ***********************************************************/
void DrawSceneObject(SceneManager* sceneManager, ShapeMeshes* meshes, const SceneObject& object)
{
    sceneManager->SetTransformations(
        object.scaleXYZ,
        object.xRotation,
        object.yRotation,
        object.zRotation,
        object.positionXYZ
    );

    sceneManager->SetShaderColor(
        object.colorValue.r,
        object.colorValue.g,
        object.colorValue.b,
        object.colorValue.a
    );

    if (object.meshType == "plane")
    {
        meshes->DrawPlaneMesh();
    }
    else if (object.meshType == "box")
    {
        meshes->DrawBoxMesh();
    }
    else if (object.meshType == "cylinder")
    {
        meshes->DrawCylinderMesh();
    }
}

/***********************************************************
 *  RenderScene
 ***********************************************************/
void SceneManager::RenderScene()
{
    if (m_pShaderManager == nullptr || m_basicMeshes == nullptr)
    {
        return;
    }

    m_pShaderManager->setBoolValue(g_UseLightingName, true);
    m_pShaderManager->Use();

    // Viewer position
    m_pShaderManager->setVec3Value("viewPos", glm::vec3(0.0f, 5.0f, 15.0f));

    // Light 1
    m_pShaderManager->setVec3Value("lights[0].position", glm::vec3(3.0f, 3.0f, 3.0f));
    m_pShaderManager->setVec3Value("lights[0].color", glm::vec3(1.0f, 1.0f, 1.0f));
    m_pShaderManager->setFloatValue("lights[0].intensity", 1.0f);

    // Light 2
    m_pShaderManager->setVec3Value("lights[1].position", glm::vec3(-2.0f, 4.0f, -2.0f));
    m_pShaderManager->setVec3Value("lights[1].color", glm::vec3(0.0f, 0.5f, 1.0f));
    m_pShaderManager->setFloatValue("lights[1].intensity", 0.8f);

    /*
     * Data-structure enhancement:
     * Objects are stored in a vector and processed in a loop instead of
     * repeating nearly identical draw logic for every object.
     *
     * This makes the rendering logic easier to scale and reduces duplicated code.
     */
    std::vector<SceneObject> sceneObjects =
    {
        {
            "plane",
            glm::vec3(20.0f, 1.0f, 10.0f),
            0.0f, 0.0f, 0.0f,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
        },
        {
            "cylinder",
            glm::vec3(0.5f, 1.5f, 0.5f),
            0.0f, 0.0f, 0.0f,
            glm::vec3(0.0f, 1.5f, 0.0f),
            glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)
        },
        {
            "box",
            glm::vec3(6.0f, 3.0f, 0.5f),
            0.0f, 0.0f, 0.0f,
            glm::vec3(0.0f, 5.0f, 0.0f),
            glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)
        }
    };

    for (const SceneObject& object : sceneObjects)
    {
        DrawSceneObject(this, m_basicMeshes, object);
    }
}