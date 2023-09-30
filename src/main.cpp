#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include "Rendering/OpenGL/PipeLines/SurfaceNormalPipeLineOgl.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "Rendering/OpenGL/PipeLines/SurfaceNormalPipeLineOgl.h"
#include "FileIO/ObjLoader.h"
#include "Rendering/OpenGL/Loaders/MeshLoaderOgl.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;


    // Decide GL+GLSL versions
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only


    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    EngiGraph::initOpenGl((GLADloadproc)glfwGetProcAddress);
    EngiGraph::Camera camera{EngiGraph::AngleDegrees<float>(90.0f),1.0f,0.001,1000,{0,-1,0}};
    camera.setPosition({5.0,5.0,5.0});
    camera.setLookTarget({0.0f,0.0f,0.0f});
    EngiGraph::SurfaceNormalPipeLineOgl pipeline = EngiGraph::SurfaceNormalPipeLineOgl(camera,500,500);
    auto mesh = EngiGraph::loadMeshOgl(EngiGraph::loadOBJ("meshes/utah_teapot.obj")[0]);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    Eigen::Vector3f cam_pos = {5.0f,5.0f,5.0f};
    Eigen::Vector3f cam_look = {0,0,0};

    int width = 500;
    int height = 500;
    int last_width = 500;
    int last_height = 500;
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if(width != last_width || height != last_height){
            last_width = width;
            last_height = height;
            pipeline.resize(width,height);
        }
        pipeline.main_camera.setPosition(cam_pos);
        pipeline.main_camera.setLookTarget(cam_look);
        pipeline.submitDrawCall(mesh,Eigen::Matrix4f::Identity());

        pipeline.render();

        auto frame_buffer = pipeline.getMainFramebuffer();

                // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if ( ImGui::CollapsingHeader("Camera") ) {
            ImGui::DragFloat3("Position",cam_pos.data());
            ImGui::DragFloat3("Target",cam_look.data());
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::Begin("GameWindow");
        {


            // Using a Child allow to fill all the space of the window.
            // It also alows customization
            ImGui::BeginChild("GameRender");

            width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
            height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

            // Get the size of the child (i.e. the whole draw size of the windows).
            ImVec2 wsize = {(float)frame_buffer.width,(float)frame_buffer.height};
            // Because I use the texture from OpenGL, I need to invert the V from the UV.
            ImGui::Image((ImTextureID)frame_buffer.color_texture, wsize, ImVec2(0, 0), ImVec2(1, 1));
            ImGui::EndChild();
        }
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    //todo catch errors and display them as popups. Also make sure to clean up after errors.

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}