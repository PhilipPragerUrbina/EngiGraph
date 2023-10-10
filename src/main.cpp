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
#include "src/Physics/TOISolver/ToiSolver.h"

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

        auto albedo_orange = EngiGraph::loadTextureOgl(EngiGraph::loadImage("textures/test_orange.png"));
        auto albedo_blue = EngiGraph::loadTextureOgl(EngiGraph::loadImage("textures/test_blue.png"));

        auto mesh_sphere = EngiGraph::loadOBJ("meshes/unit_sphere.obj")[0];
        auto mesh_cube = EngiGraph::loadOBJ("test_files/cube.obj")[0];
        auto mesh_cube_visual = EngiGraph::loadMeshOgl(mesh_cube);
        auto mesh_cube_physics = std::make_shared<EngiGraph::Mesh>(EngiGraph::stripVisualMesh(mesh_cube));
        auto mesh_sphere_visual = EngiGraph::loadMeshOgl(mesh_sphere);
        auto mesh_sphere_physics = std::make_shared<EngiGraph::Mesh>(EngiGraph::stripVisualMesh(mesh_sphere));

        EngiGraph::TOISolver solver;



        solver.bodies.push_back(EngiGraph::TOISolver::RigidBody{
                {0,2,0},Eigen::Quaterniond::Identity(),{0,0,0},{0,0,0},Eigen::Matrix4d::Identity(), Eigen::Matrix4d::Identity(),mesh_sphere_physics,mesh_sphere_visual,albedo_blue,1.0f,Eigen::Matrix3d::Identity(),{0,0,0}
        });

        solver.bodies.push_back(EngiGraph::TOISolver::RigidBody{
                {0,-2,0},Eigen::Quaterniond::Identity(),{0,0,0},{0,0,0},Eigen::Matrix4d::Identity(), Eigen::Matrix4d::Identity(),mesh_cube_physics,mesh_cube_visual,albedo_orange,1.0f,Eigen::Matrix3d::Identity(),{0,0,0},
                false
        });




        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        Eigen::Vector3f cam_pos = {5.0f, 5.0f, 5.0f};
        Eigen::Vector3f cam_look = {0, 0, 0};

        EngiGraph::DeferredPipelineOgl::PointLight basic_light{{0,0,0},{1,1,1}};
        pipeline.ambient_color = {1.0,1.0,1.0};
        pipeline.point_lights.push_back(basic_light);

        int width = 500;
        int height = 500;
        int last_width = 500;
        int last_height = 500;

        std::vector<EngiGraph::TOISolver::RigidBody> last_rigidbody_data;

        float delta_time = 0.01;

        bool step = false;

        solver.step(delta_time);

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();


            if (width != last_width || height != last_height) {
                last_width = width;
                last_height = height;
                pipeline.resize(width, height);
            }


            for (const auto& body : solver.bodies) {
                pipeline.submitDrawCall(EngiGraph::DeferredPipelineOgl::DrawCall{body.render_mesh,body.render_texture,body.initial_transform.cast<float>()});
            }
            pipeline.render();

            auto frame_buffer = pipeline.getMainFramebuffer();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if(ImGui::CollapsingHeader("Physics")){
                ImGui::DragFloat("delta time", &delta_time,0.001f,0.0001f);
                if(ImGui::Button("Step")){
                    solver.step(delta_time);
                }
                if(step){
                    if(ImGui::Button("Stop")){
                        solver.bodies = last_rigidbody_data;
                        solver.step(delta_time);
                        step = false;
                    }
                }else{
                    if(ImGui::Button("Start")){
                        last_rigidbody_data = solver.bodies;
                        step = true;
                    }
                }

                ImGui::Indent();
                int i = 0;
                for ( auto& body : solver.bodies) {
                    ImGui::PushID(i);
                    i++;
                    if (ImGui::CollapsingHeader("Body")) {
                        if(ImGui::DragScalarN("Position", ImGuiDataType_Double,body.position.data(),3)){
                            Eigen::Transform<double,3,Eigen::Affine> transform_initial = Eigen::Transform<double,3,Eigen::Affine>::Identity();
                            transform_initial.translate(body.position).rotate(body.rotation);
                            body.initial_transform = transform_initial.matrix();
                        }

                        ImGui::DragScalarN("Velocity", ImGuiDataType_Double,body.velocity.data(),3);
                        ImGui::DragScalarN("Angular Velocity", ImGuiDataType_Double,body.angular_velocity.data(),3);
                    }
                    ImGui::PopID();
                }
                ImGui::Unindent();
            }

            if(step){
                solver.step(delta_time);
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
                if(ImGui::DragFloat3("Position", cam_pos.data())){
                    pipeline.camera.setPosition(cam_pos);
                }
                if(ImGui::DragFloat3("Target", cam_look.data())){
                    pipeline.camera.setLookTarget(cam_look);
                }
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