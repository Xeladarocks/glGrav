#pragma once
#ifndef MAIN_SHADER_H
#define MAIN_SHADER_H


#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use() { glUseProgram(ID); }
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat3(const std::string& name, glm::mat3 value) const;
    void setMat4(const std::string& name, glm::mat4 value) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};


#endif //MAIN_SHADER_H
