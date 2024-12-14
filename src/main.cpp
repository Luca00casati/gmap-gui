#include <iostream>
using namespace std;

#include <nfd.h>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "../vendor/imgui_impl_glfw.h"
#include "../vendor/imgui_impl_opengl3.h"
#include "../vendor/imgui_impl_opengl3_loader.h"

#define GLSL_VERSION "#version 330"

nfdchar_t* chosefile() {
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

    if (result == NFD_OKAY) {
        //puts("Success!");
        //puts(outPath);
        return outPath;
    } else if (result == NFD_CANCEL) {
        //puts("User pressed cancel.");
        return NULL;
    } else {
        printf("Error: %s\n", NFD_GetError());
    }
    return NULL;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);

    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Hello, ImGUI!",
                                          nullptr, nullptr);
    if (window == nullptr)
    {
        cout << "Could not create GLFW window" << endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.IniFilename = nullptr;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    nfdchar_t *file = nullptr;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::ShowDemoWindow(nullptr);

        ImGui::Begin("Window");
        {
            ImGui::Text("Hello, Dear ImGUI!");
            if (ImGui::Button("file"))
            {
                if (file)
                {
                    free(file);
                    file = nullptr;
                }
                file = chosefile();
            }

            if (file == nullptr)
            {
                ImGui::Text("No file chosen");
            }
            else
            {
                ImGui::Text("Chosen file: %s", file);
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}