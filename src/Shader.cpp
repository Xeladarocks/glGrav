#include "Shader.h"

const std::string vShaderString = R"(
    #version 450 core

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aVertexColor;

    out vec3 vColor;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vColor = aVertexColor;
        //vColor = vec3(255, 255, 0);
    }
)";

const std::string fShaderString = R"(
    #version 450 core

    in vec3 vColor;

    out vec4 FragColor;

    void main() {
        FragColor = vec4(vColor, 1.0f);
    }
)";

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // get root of source directory
        std::string dir_path = ".\\";
        const size_t last_slash_idx = dir_path.rfind('\\');
        if (std::string::npos != last_slash_idx) {
            dir_path = dir_path.substr(0, last_slash_idx);
        }
        //std::cout << dir_path + "\\" + vertexPath << "\n";
        // open files
        vShaderFile.open(dir_path + "\\" + vertexPath);
        fShaderFile.open(dir_path + "\\" + fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "WARNING::SHADER::FILE_NOT_SUCCESFULLY_READ::FALLBACK_TO_STRING" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;

        vertexCode = vShaderString;
        fragmentCode = fShaderString;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, glm::mat3 value) const {
    glUniformMatrix4fv(
        glGetUniformLocation(ID, name.c_str()), // location
        1, // # of matrices
        GL_FALSE, // transpose
        glm::value_ptr(value)
    );
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, glm::mat4 value) const {
    glUniformMatrix4fv(
        glGetUniformLocation(ID, name.c_str()), // location
        1, // # of matrices
        GL_FALSE, // transpose
        &value[0][0]
    );
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    if (success) {
        std::cout << "Shader of type " + type + " compiled successfully." << "\n";
    }
}