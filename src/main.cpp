#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Skybox.h"
#include "Shader.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0, 0.0, -0.25));
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;
bool first_mouse = true;

float delta_time = 0.0f;
float last_frame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // build and compile our shader program
    // ------------------------------------
    Shader shader(RESOURCES_PATH "shaders/vertex.vs", 
                  RESOURCES_PATH "shaders/fragment.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

    VertexArray VAO;
    VertexBuffer VBO;
    VAO.bind();
    VBO.set_data(vertices, sizeof(vertices));
    VBO.set_attributes(0, 3, 3, 0); // position

    Shader skybox_shader(RESOURCES_PATH "shaders/skybox.vs", RESOURCES_PATH "shaders/skybox.fs");
    // SkyboxFacePaths paths = {
    //     RESOURCES_PATH "skyboxes/PoodsCalmNebula/PositiveX.png",
    //     RESOURCES_PATH "skyboxes/PoodsCalmNebula/NegativeX.png",
    //     RESOURCES_PATH "skyboxes/PoodsCalmNebula/PositiveY.png",
    //     RESOURCES_PATH "skyboxes/PoodsCalmNebula/NegativeY.png",
    //     RESOURCES_PATH "skyboxes/PoodsCalmNebula/PositiveZ.png",
    //     RESOURCES_PATH "skyboxes/PoodsCalmNebula/NegativeZ.png",
    // };
    SkyboxFacePaths paths = {
        RESOURCES_PATH "skyboxes/ocean/right.jpg",
        RESOURCES_PATH "skyboxes/ocean/left.jpg",
        RESOURCES_PATH "skyboxes/ocean/top.jpg",
        RESOURCES_PATH "skyboxes/ocean/bottom.jpg",
        RESOURCES_PATH "skyboxes/ocean/front.jpg",
        RESOURCES_PATH "skyboxes/ocean/back.jpg",
    };

    Skybox skybox(paths, &skybox_shader);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(RESOURCES_PATH "fonts/JetBrainsMono-Regular.ttf", 24.0);

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // input
        // -----
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::ShowDemoWindow();

        {
            static float f = 0.0;
            static int   counter = 0;

            ImGui::Begin("New Window");

            ImGui::Text("FPS: 0%.1f", 1.0f / delta_time);

            ImGui::Text("This is a line of text.");

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);

            if (ImGui::Button("Button")) {
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::End();
        }

        // render
        // ------
        ImGui::Render();
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // draw our first triangle
        // glUseProgram(shaderProgram);
        shader.use();
        // skybox_shader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.get_zoom()), 
                                                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 
                                                0.1f, 100.0f);
        shader.set_mat4("projection", projection);
        // skybox_shader.set_mat4("projection", projection);

        glm::mat4 view = camera.get_view_matrix();
        shader.set_mat4("view", view);
        // skybox_shader.set_mat4("view", glm::mat4(glm::mat3(camera.get_view_matrix())));

        glm::mat4 model(1.0f);
        shader.set_mat4("model", model);

        skybox_shader.use();
        skybox_shader.set_mat4("projection", projection);
        skybox_shader.set_mat4("view", glm::mat4(glm::mat3(camera.get_view_matrix())));
        skybox.draw();

        // skybox.draw();

        shader.use();

        // glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        VAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.process_keyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.process_keyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.process_keyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.process_keyboard(RIGHT, delta_time);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (first_mouse)
    {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

    last_x = xpos;
    last_y = ypos;

    camera.process_mouse(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.process_scroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static bool cursor_enabled = true;
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, cursor_enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        cursor_enabled = !cursor_enabled;
    }

    static bool wireframe_enabled = true;
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, wireframe_enabled ? GL_LINE : GL_FILL);
        wireframe_enabled = !wireframe_enabled;
    }
}