#include "Application.hpp"

#include <iostream>
#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>

#include <glm/gtc/type_ptr.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        //clean gl depth buffer byte

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Put here rendering code
        // Put here rendering code
        glBindVertexArray(m_cubeVAO);

        glDrawElements(GL_TRIANGLES, m_cube.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        //
        //
        //

        // GUI code:
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }
            ImGui::End();
        }

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        ImGui::Render();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            //viewController.update(float(ellapsedTime))
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" }

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file
    

    m_cube = glmlv::makeCube();

    // Doing the coolest VBO
    glGenBuffers(1, &m_cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_cube.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_cube.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Doing the coolest IBO
    glGenBuffers(1, &m_cubeIBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeIBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_cube.indexBuffer.size() * sizeof(uint32_t), m_cube.indexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Doing the coolest VAO
    glGenVertexArrays(1, &m_cubeVAO);

    // Here we load and compile shaders from the library
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "cube.vs.glsl", m_ShadersRootPath / m_AppName / "cube.fs.glsl" });

    // Here we use glGetAttribLocation(program, attribname) to obtain attrib locations; We could also directly use locations if they are set in the vertex shader (cf. triangle app)
    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordAttrLocation  = 2;


    glBindVertexArray(m_cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));
    glEnableVertexAttribArray(normalAttrLocation);
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));
    glEnableVertexAttribArray(texCoordAttrLocation);
    glVertexAttribPointer(texCoordAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    m_program.use();

     //setting uniforms
    auto uMVPMatrix = m_program.getUniformLocation("uMVPMatrix");
    auto uMVMatrix = m_program.getUniformLocation("uMVMatrix");
    auto uNormalMatrix = m_program.getUniformLocation("uNormalMatrix");

    glm::mat4 projection = glm::perspective(
    45.0f,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
    4.0f / 3.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
    0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
    100.0f       // Far clipping plane. Keep as little as possible.
    );

    glm::mat4 view = glm::lookAt(
        glm::vec3(4,3,3),
        glm::vec3(0,0,0), 
        glm::vec3(0,1,0)  
    );

    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 mvMatrix = view * model;
    glm::mat4 mvpMatrix = projection * view * model;
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(mvMatrix));


    glUniformMatrix4fv(uMVPMatrix, 1, false, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(uMVMatrix, 1, false, glm::value_ptr(mvMatrix));
    glUniformMatrix4fv(uNormalMatrix, 1, false, glm::value_ptr(normalMatrix));

    glEnable(GL_DEPTH_TEST);

}

Application::~Application()
{
    if (m_cubeVBO) {
        glDeleteBuffers(1, &m_cubeVBO);
    }

    if (m_cubeVAO) {
        glDeleteBuffers(1, &m_cubeVAO);
    }

    if (m_cubeIBO) {
        glDeleteBuffers(1, &m_cubeIBO);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
