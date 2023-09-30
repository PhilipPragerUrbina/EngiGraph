#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include "Rendering/OpenGL/PipeLines/SurfaceNormalPipeLineOgl.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "Rendering/OpenGL/PipeLines/SurfaceNormalPipeLineOgl.h"
#include "FileIO/ObjLoader.h"
#include "Rendering/OpenGL/Loaders/MeshLoaderOgl.h"
#include "FileIO/ImageIo.h"
#include "Rendering/OpenGL/Loaders/TextureLoaderOgl.h"
#include "Rendering/OpenGL/PipeLines/DeferredPipelineOgl.h"
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

    try {
        EngiGraph::initOpenGl((GLADloadproc) glfwGetProcAddress);
        EngiGraph::Camera camera{EngiGraph::AngleDegrees<float>(90.0f), 1.0f, 0.001, 1000, {0, -1, 0}};
        camera.setPosition({5.0, 5.0, 5.0});
        camera.setLookTarget({0.0f, 0.0f, 0.0f});
        auto pipeline = EngiGraph::DeferredPipelineOgl(500, 500, camera);

        auto meshes = EngiGraph::loadOBJ("meshes/mori_knob.obj");
        auto mesh = EngiGraph::loadMeshOgl(meshes[0]);
        auto mesh2 = EngiGraph::loadMeshOgl(meshes[1]);
        auto mesh3 = EngiGraph::loadMeshOgl(meshes[4]);
        auto albedo = EngiGraph::loadTextureOgl(EngiGraph::readImage("textures/test_orange.png"));
        auto albedo2 = EngiGraph::loadTextureOgl(EngiGraph::readImage("textures/test_blue.png"));


        // Our state
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        Eigen::Vector3f cam_pos = {5.0f, 5.0f, 5.0f};
        Eigen::Vector3f cam_look = {0, 0, 0};

        EngiGraph::DeferredPipelineOgl::PointLight basic_light{{0,0,0},{1,1,1}};
        pipeline.point_lights.push_back(basic_light);

        int width = 500;
        int height = 500;
        int last_width = 500;
        int last_height = 500;
        // Main loop
        while (!glfwWindowShouldClose(window)) {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();
            if (width != last_width || height != last_height) {
                last_width = width;
                last_height = height;
                pipeline.resize(width, height);
            }
            pipeline.camera.setPosition(cam_pos);
            pipeline.camera.setLookTarget(cam_look);
            pipeline.submitDrawCall({mesh, albedo, Eigen::Matrix4f::Identity()});
            pipeline.submitDrawCall({mesh2, albedo, Eigen::Matrix4f::Identity()});
            pipeline.submitDrawCall({mesh3, albedo2, Eigen::Matrix4f::Identity()});

            pipeline.render();

            auto frame_buffer = pipeline.getMainFramebuffer();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (ImGui::CollapsingHeader("Lights")) {
                ImGui::Indent();
                int i = 0;
                for ( auto& light : pipeline.point_lights) {
                    ImGui::PushID(i);
                    i++;
                    if (ImGui::CollapsingHeader("Point light")) {
                        ImGui::DragFloat3("Point", light.position.data());
                        ImGui::DragFloat("Brightness", &light.brightness);
                        ImGui::DragFloat("Constant attenuation", &light.constant_attenuation);
                        ImGui::DragFloat("Linear attenuation", &light.linear_attenuation);
                        ImGui::DragFloat("Quadratic attenuation", &light.quadratic_attenuation);
                        ImGui::ColorEdit3("Luminance", light.color.data());
                    }
                    ImGui::PopID();
                }
                ImGui::Unindent();
                if(ImGui::Button("Add Light")){
                    pipeline.point_lights.push_back(basic_light);
                }
            }


            if (ImGui::CollapsingHeader("Camera")) {
                ImGui::DragFloat3("Position", cam_pos.data());
                ImGui::DragFloat3("Target", cam_look.data());
            }
            if (ImGui::CollapsingHeader("World")) {
                ImGui::ColorEdit3("Ambient", pipeline.ambient_color.data());
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
                ImVec2 wsize = {(float) frame_buffer.width, (float) frame_buffer.height};
                // Because I use the texture from OpenGL, I need to invert the V from the UV.
                ImGui::Image((ImTextureID) frame_buffer.color_texture, wsize, ImVec2(0, 0), ImVec2(1, 1));
                ImGui::EndChild();
            }
            ImGui::End();

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                         clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    } catch (const EngiGraph::RuntimeException& e){
        while (!glfwWindowShouldClose(window)) {

            glfwPollEvents();
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Error");
            {
                ImGui::Text("%s", e.what());

            }
            ImGui::End();

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.0f,0.0f,0.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }
    }


    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}