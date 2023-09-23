//
// Created by Philip on 9/18/2023.
//

#include "ShaderOgl.h"

namespace EngiGraph {
    ShaderOGL::ShaderOGL(const std::string &vertex_path, const std::string &fragment_path) {
        //todo geometry shaders
        //todo warnings
        //todo shader constants

        std::string vertex_shader_text, fragment_shader_text;
        std::ifstream vertex_file, fragment_file;
        vertex_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fragment_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            vertex_file.open(vertex_path);
            fragment_file.open(fragment_path);
            std::stringstream vertex_stream, fragment_stream;
            vertex_stream << vertex_file.rdbuf();
            fragment_stream << fragment_file.rdbuf();
            vertex_file.close();
            fragment_file.close();
            vertex_shader_text = vertex_stream.str();
            fragment_shader_text = fragment_stream.str();
        }
        catch (std::ifstream::failure& error)
        {
            throw RuntimeException(std::string ("Error reading shaders:  ") + error.what());
        }

        const char* vertex_shader_text_ptr = vertex_shader_text.c_str();
        const char * fragment_shader_text_ptr = fragment_shader_text.c_str();

        khronos_uint32_t vertex_id, fragment_id;

        int success;
        char info_log[1024];

        //Compile vertex shader
        vertex_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_id, 1, &vertex_shader_text_ptr, nullptr);
        glCompileShader(vertex_id);
        glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(vertex_id, 1024, nullptr, info_log);
            throw RuntimeException("Error compiling vertex shader : " + vertex_path + " : " + info_log);
        }

        //Compile fragment shader
        fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_id, 1, &fragment_shader_text_ptr, nullptr);
        glCompileShader(fragment_id);
        glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(fragment_id, 1024, nullptr, info_log);
            throw RuntimeException("Error compiling fragment shader : " + fragment_path + " : " + info_log);
        }

        //Link shader program
        program_id = glCreateProgram();
        glAttachShader(program_id, vertex_id);
        glAttachShader(program_id, fragment_id);
        glLinkProgram(program_id);
        glGetProgramiv(program_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program_id, 1024, nullptr, info_log);
            throw RuntimeException("Error compiling shader program : " + fragment_path + " & " + vertex_path + " : " + info_log);
        }
        //clean up
        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);
    }

    ShaderOGL::~ShaderOGL() {
        glDeleteProgram(program_id);
    }

    void ShaderOGL::use() const {
        glUseProgram(program_id);
    }

    void ShaderOGL::setUniform(const char *name, const Eigen::Matrix4f &value) const {
        glUniformMatrix4fv(glGetUniformLocation(program_id, name), 1 , GL_FALSE, value.data());
    }

    void ShaderOGL::setUniform(const char *name, const Eigen::Matrix3f &value) const {
        glUniformMatrix3fv(glGetUniformLocation(program_id, name), 1 , GL_FALSE, value.data());
    }

    void ShaderOGL::setUniform(const char *name, const Eigen::Vector2f &value) const {
        glUniform2fv(glGetUniformLocation(program_id, name), 1, value.data());
    }

    void ShaderOGL::setUniform(const char *name, float value) const {
        glUniform1f(glGetUniformLocation(program_id, name), value);
    }

    void ShaderOGL::setUniform(const char *name, int value) const {
        glUniform1i(glGetUniformLocation(program_id, name), value);
    }

    void ShaderOGL::setUniform(const char *name, bool value) const {
        glUniform1i(glGetUniformLocation(program_id, name), (int)value);
    }

    void ShaderOGL::setUniform(const char *name, const Eigen::Vector3f &value) const {
        glUniform3fv(glGetUniformLocation(program_id, name), 1, value.data());
    }

    void ShaderOGL::setUniform(const char *name, const Eigen::Vector4f &value) const {
        glUniform4fv(glGetUniformLocation(program_id, name), 1, value.data());
    }

} // EngiGraph