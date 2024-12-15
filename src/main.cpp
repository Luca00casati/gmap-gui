#include <iostream>
using namespace std;

//#include <nfd.h>
#include "include/common.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "../vendor/imgui_impl_glfw.h"
#include "../vendor/imgui_impl_opengl3.h"
#include "../vendor/imgui_impl_opengl3_loader.h"

#define GLSL_VERSION "#version 330"

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

    nfdchar_t *Strings_File = nullptr;
    string Strings_Text = "";
    bool Strings_Loaded = false;
    int Strings_MinLength = 5;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //#define DEMO

        #ifdef DEMO
        ImGui::ShowDemoWindow(nullptr);
        #endif

        #ifndef DEMO
        ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Strings");
        {            
            if (ImGui::InputInt("MinLength", &Strings_MinLength)){
                Strings_Loaded = false;
                if (Strings_MinLength < 0) {
                    Strings_MinLength = 0;
                }
                if (Strings_MinLength > 100) {
                    Strings_MinLength = 100;
                }
            }
            if (ImGui::Button("open file"))
            {   
                if (Strings_File)
                {
                    free(Strings_File);
                    Strings_File = nullptr;
                    Strings_Loaded = false;
                }
                Strings_File = chosefileload();
            }

            if (Strings_File == nullptr)
            {
                ImGui::Text("No file chosen");
            }
            else
            {
                ImGui::Text("Chosen file: %s", Strings_File);
                ImGui::Separator();
                if (!Strings_Loaded){
                    Strings_Text = extractStrings(Strings_File, Strings_MinLength);
                }
                ImGui::TextUnformatted(Strings_Text.c_str());
                Strings_Loaded = true;

            }
        }
        ImGui::End();
        #endif

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