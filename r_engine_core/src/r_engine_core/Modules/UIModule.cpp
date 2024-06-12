#include <r_engine_core/Modules/UIModule.hpp>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>


namespace r_engine
{
    // Инициализация при создании окна
    void UIModule::on_window_create(GLFWwindow* pWindow)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        // Флаг для докинга окон
        io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
        // Флаг вьюпортов
        io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_ViewportsEnable;

        // Определение контекста IMGUI в контексте OpenGL
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(pWindow, true);
    }

    // Завершение работы IMGUI
    void UIModule::on_window_close()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    // Начало отрисовки
    void UIModule::on_ui_draw_begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    // Завершение отрисовки
    void UIModule::on_ui_draw_end()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
}