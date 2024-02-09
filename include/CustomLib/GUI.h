#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


class GUI{
    public:
        void gui_init(GLFWwindow* window);
        ~GUI();
        void BeginFrame();
        void EndFrame();
};
