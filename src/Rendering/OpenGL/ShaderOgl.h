//
// Created by Philip on 9/18/2023.
//

#pragma once

#include "glad/glad.h"
#include <string>
#include <fstream>
#include <sstream>
#include "./src/Exceptions/RuntimeException.h"
#include <Eigen>

namespace EngiGraph {

    /**
     * OpenGL shader program wrapper.
     */
    class ShaderOGL {
    private:
        khronos_uint32_t program_id;
    public:

        /**
         * Create an OpenGL shader program.
         * @throws RuntimeException Error loading or compiling shader.
         * @param vertex_path Path of vertex shader.
         * @param fragment_path Path of fragment shader.
         * @see https://learnopengl.com/Getting-started/Shaders
         */
        ShaderOGL(const std::string& vertex_path, const std::string& fragment_path);

        /**
         * Get the shader program id.
         * @warning Shaders will be deleted once this object goes out of scope.
         */
        [[nodiscard]] khronos_uint32_t getProgramId() const {
            return program_id;
        }

        /**
         * Set the shader program as the active shader program.
         * @details Calls glUseProgram.
         */
        void use() const;

        //todo double uniforms(don't overload, add d postfix instead)

        /**
         * Set a boolean uniform.
         * @param name Name of uniform.
         * @param value Value of uniform.
         */
        void setUniform(const char* name, bool value) const;

        /**
         * Set a signed integer uniform.
         * @param name Name of uniform.
         * @param value Value of uniform.
         */
        void setUniform(const char* name, int value) const;

        /**
         * Set a float uniform.
         * @param name Name of uniform.
         * @param value Value of uniform.
         */
        void setUniform(const char* name, float value) const;

        /**
         * Set a vector 2 float uniform.
         * @param name Name of uniform.
         * @param value Value of uniform.
         */
        void setUniform(const char* name, const Eigen::Vector2f& value) const;

        /**
         * Set a vector 3 float uniform.
         * @param name Name of uniform.
         * @param value Value of uniform.
         */
        void setUniform(const char* name, const Eigen::Vector3f& value) const;

        /**
        * Set a vector 4 float uniform.
        * @param name Name of uniform.
        * @param value Value of uniform.
        */
        void setUniform(const char* name, const Eigen::Vector4f& value) const;

        /**
        * Set a 3x3 matrix float uniform.
        * @param name Name of uniform.
        * @param value Value of uniform.
        */
        void setUniform(const char* name, const Eigen::Matrix3f& value) const;

        /**
        * Set a 4x4 matrix float uniform.
        * @param name Name of uniform.
        * @param value Value of uniform.
        */
        void setUniform(const char* name, const Eigen::Matrix4f& value) const;

        /**
         * Delete shader program.
         */
        ~ShaderOGL();

    };

} // EngiGraph
