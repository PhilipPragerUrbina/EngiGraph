#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "FileIO/ObjLoader.h"
#include "Rendering/OpenGL/Loaders/MeshLoaderOgl.h"
#include "FileIO/ImageIo.h"
#include "Rendering/OpenGL/Loaders/TextureLoaderOgl.h"

#include "Rendering/OpenGL/PipeLines/DeferredPipelineOgl.h"
#include "src/Geometry/MeshConversions.h"
#include "src/Physics/Collisions/LinearPointCcd.h"

#include <GLFW/glfw3.h>

/**
 * Callback if error occurs in glfw window.
 */
static void glfwErrorCallBack(int error, const char* description)
{
    throw EngiGraph::RuntimeException(description);
}

//Minimal main for testing.
//todo ImGui application class
int main(int, char**)
{
    glfwSetErrorCallback(glfwErrorCallBack);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    //todo window system
    GLFWwindow* window = glfwCreateWindow(1280, 720, "EngiGraph", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //Error can now be caught and displayed in a user-friendly gui.
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

        auto albedo = EngiGraph::loadTextureOgl(EngiGraph::loadImage("textures/test_orange.png"));
        auto albedo2 = EngiGraph::loadTextureOgl(EngiGraph::loadImage("textures/test_blue.png"));

        auto mesh_teapot = EngiGraph::loadOBJ("meshes/unit_sphere.obj")[0];
        auto mesh_teapot_visual = EngiGraph::loadMeshOgl(mesh_teapot);
        auto mesh_physics = EngiGraph::stripVisualMesh(mesh_teapot);


        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        Eigen::Vector3f cam_pos = {5.0f, 5.0f, 5.0f};
        Eigen::Vector3f cam_look = {0, 0, 0};

        Eigen::Vector3f teapot_a_position_initial = {2.0,2.0,2.0};
        Eigen::Vector3f teapot_a_position_final = {1.0,1.0,1.0};
        Eigen::Vector3f teapot_b_position = {1.5,1.5,1.5};


        EngiGraph::DeferredPipelineOgl::PointLight basic_light{{0,0,0},{1,1,1}};
        pipeline.point_lights.push_back(basic_light);

        int width = 500;
        int height = 500;
        int last_width = 500;
        int last_height = 500;

        while (!glfwWindowShouldClose(window)) {

            Eigen::Transform<double,3,Eigen::Affine> transform_a_initial =  Eigen::Transform<double,3,Eigen::Affine>::Identity();
            transform_a_initial.translate(teapot_a_position_initial.cast<double>());
            Eigen::Transform<double,3,Eigen::Affine> transform_a_final =  Eigen::Transform<double,3,Eigen::Affine>::Identity();
            transform_a_final.translate(teapot_a_position_final.cast<double>());
            Eigen::Transform<double,3,Eigen::Affine> transform_b =  Eigen::Transform<double,3,Eigen::Affine>::Identity();
            transform_b.translate(teapot_b_position.cast<double>());

            auto hit = EngiGraph::linearCCD(mesh_physics,mesh_physics,transform_a_initial.matrix(),transform_b.matrix(),transform_a_final.matrix(),transform_b.matrix());
            Eigen::Vector3f teapot_a_position = teapot_a_position_final;
            if(hit.has_value()){
                double time = hit->w();
                teapot_a_position = teapot_a_position_initial + (teapot_a_position_final - teapot_a_position_initial) *  time;

            }


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

            Eigen::Transform<float,3,Eigen::Affine> transform =  Eigen::Transform<float,3,Eigen::Affine>::Identity();
            pipeline.submitDrawCall({mesh_teapot_visual, albedo, transform.translate(teapot_a_position_initial).matrix()});
            transform =  Eigen::Transform<float,3,Eigen::Affine>::Identity();
            pipeline.submitDrawCall({mesh_teapot_visual, albedo, transform.translate(teapot_a_position_final).matrix()});
            transform =  Eigen::Transform<float,3,Eigen::Affine>::Identity();
            pipeline.submitDrawCall({mesh_teapot_visual, albedo, transform.translate(teapot_a_position).matrix()});
            transform =  Eigen::Transform<float,3,Eigen::Affine>::Identity();
            pipeline.submitDrawCall({mesh_teapot_visual, albedo2, transform.translate(teapot_b_position).matrix()});

            pipeline.render();

            auto frame_buffer = pipeline.getMainFramebuffer();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (ImGui::CollapsingHeader("CCD")) {
                ImGui::DragFloat3("A initial", teapot_a_position_initial.data());
                ImGui::DragFloat3("A final", teapot_a_position_final.data());
                ImGui::DragFloat3("B position", teapot_b_position.data());
            }

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

            //Render OpenGL pipeline to sub window
            ImGui::Begin("GameWindow");
            {
                ImGui::BeginChild("GameRender");
                width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
                height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
                ImVec2 wsize = {(float) frame_buffer.width, (float) frame_buffer.height};
                ImGui::Image((ImTextureID) frame_buffer.color_texture, wsize, ImVec2(0, 0), ImVec2(1, 1));
                ImGui::EndChild();
            }
            ImGui::End();

            //Careful, OpenGL settings set in pipelines may carry over, make sure to reset everything at the end of a pipeline.
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }
    } catch (const EngiGraph::RuntimeException& e){
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Error");
            {
                ImGui::Text("%s", e.what());

            }
            ImGui::End();
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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}