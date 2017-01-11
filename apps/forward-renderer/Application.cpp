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

        glm::mat4 projection = glm::perspective(
            45.0f,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
            (float)m_nWindowWidth / (float)m_nWindowHeight, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
            0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
            100.0f       // Far clipping plane. Keep as little as possible.
        );

        m_viewController.update((float)seconds/50.0f);
        glm::mat4 view = m_viewController.getViewMatrix();
        /* rotate code */
        m_cubeModel = glm::rotate(m_cubeModel, (float)seconds/20.0f, glm::vec3(1.0, 0.0, 0.0));

        m_mvMatrix = view * m_cubeModel;
        m_mvpMatrix = projection * view * m_cubeModel;
        m_normalMatrix = glm::transpose(glm::inverse(m_mvMatrix));

        glUniformMatrix4fv(m_uMVPMatrix, 1, false, glm::value_ptr(m_mvpMatrix));
        glUniformMatrix4fv(m_uMVMatrix, 1, false, glm::value_ptr(m_mvMatrix));
        glUniformMatrix4fv(m_uNormalMatrix, 1, false, glm::value_ptr(m_normalMatrix));

        // Put here rendering code
        glBindVertexArray(m_cubeVAO);

        glDrawElements(GL_TRIANGLES, m_cube.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);

        m_mvMatrix = view * m_sphereModel;
        m_mvpMatrix = projection * view * m_sphereModel;
        m_normalMatrix = glm::transpose(glm::inverse(m_mvMatrix));

        glUniformMatrix4fv(m_uMVPMatrix, 1, false, glm::value_ptr(m_mvpMatrix));
        glUniformMatrix4fv(m_uMVMatrix, 1, false, glm::value_ptr(m_mvMatrix));
        glUniformMatrix4fv(m_uNormalMatrix, 1, false, glm::value_ptr(m_normalMatrix));

        glBindVertexArray(m_sphereVAO);

        glDrawElements(GL_TRIANGLES, m_sphere.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);

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
    m_sphere = glmlv::makeSphere(10);

    // Doing the coolest cube VBO
    glGenBuffers(1, &m_cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_cube.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_cube.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Doing the coolest cube IBO
    glGenBuffers(1, &m_cubeIBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeIBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_cube.indexBuffer.size() * sizeof(uint32_t), m_cube.indexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Doing the coolest cube VAO
    glGenVertexArrays(1, &m_cubeVAO);

        // Doing the coolest cube VBO
    glGenBuffers(1, &m_sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_sphere.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_sphere.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Doing the coolest cube IBO
    glGenBuffers(1, &m_sphereIBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphereIBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_sphere.indexBuffer.size() * sizeof(uint32_t), m_sphere.indexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Doing the coolest cube VAO
    glGenVertexArrays(1, &m_sphereVAO);

    // Here we load and compile shaders from the library
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "cube.vs.glsl", m_ShadersRootPath / m_AppName / "cube.fs.glsl" });

    // Here we use glGetAttribLocation(program, attribname) to obtain attrib locations; We could also directly use locations if they are set in the vertex shader (cf. triangle app)
    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordAttrLocation  = 2;


    /* BINDING THE CUBE */
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

        /* BINDING THE SPHERE */
    glBindVertexArray(m_sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));
    glEnableVertexAttribArray(normalAttrLocation);
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));
    glEnableVertexAttribArray(texCoordAttrLocation);
    glVertexAttribPointer(texCoordAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereIBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    m_program.use();

     //setting uniforms
    m_uMVPMatrix = m_program.getUniformLocation("uMVPMatrix");
    m_uMVMatrix = m_program.getUniformLocation("uMVMatrix");
    m_uNormalMatrix = m_program.getUniformLocation("uNormalMatrix");


    m_cubeModel = glm::mat4(1.0f);
    m_sphereModel = glm::mat4(1.0f);
    m_sphereModel = glm::translate(m_sphereModel, glm::vec3(0.0, 1.0, 0.0));


    /* CAMERA */
    m_viewController = glmlv::ViewController(m_GLFWHandle.window(), 1.0);
    m_viewController.setViewMatrix(glm::lookAt(
            glm::vec3(4,3,3),
            glm::vec3(0,0,0), 
            glm::vec3(0,1,0)  
        ));        


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

    if (m_sphereVBO) {
        glDeleteBuffers(1, &m_cubeVBO);
    }

    if (m_sphereVAO) {
        glDeleteBuffers(1, &m_cubeVAO);
    }

    if (m_sphereIBO) {
        glDeleteBuffers(1, &m_cubeIBO);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
