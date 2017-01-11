#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/ViewController.hpp>


class Application
{
public:
    Application(int argc, char** argv);

    ~Application();

    int run();
private:
    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;

    glmlv::ViewController m_viewController = 0;

    GLuint m_cubeVBO = 0;
    GLuint m_cubeIBO = 0;
    GLuint m_cubeVAO = 0;

    GLuint m_sphereVBO = 0;
    GLuint m_sphereIBO = 0;
    GLuint m_sphereVAO = 0;

    glmlv::SimpleGeometry m_cube;
    glmlv::SimpleGeometry m_sphere;

    glm::mat4 m_mvMatrix;
    glm::mat4 m_mvpMatrix;
    glm::mat4 m_normalMatrix;

    GLint m_uMVPMatrix;
    GLint m_uMVMatrix;
    GLint m_uNormalMatrix;

    GLint m_uDirectionalLightDir;
    GLint m_uDirectionalLightIntensity;

    GLint m_uPointLightPos;
    GLint m_uPointLightIntensity;
    GLint m_uKd;

    glm::mat4 m_cubeModel;
    glm::mat4 m_sphereModel;
    glm::vec3 m_cubeColor;
    glm::vec3 m_sphereColor;

    glm::vec4 m_directionalLightDir;
    glm::vec4 m_pointLightPos;
    glm::vec3 m_directionalLightIntensity;
    glm::vec3 m_pointLightIntensity;

    glmlv::GLProgram m_program;
};