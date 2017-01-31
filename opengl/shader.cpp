#include <opengl/shader.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

GLuint Shader::boundID = 0;

void Shader::bind() const
{
    if(*ID != boundID)
    {
        glUseProgram(*ID);
        boundID = *ID;
    }
}

GLint Shader::getUniLoc(const std::string& uniformName) const
{

    GLint loc;
    try
    {
        loc = uniformLocations.at(uniformName);
    }
    catch(const std::exception&)
    {
        throw std::runtime_error("[Shader (ID=" + std::to_string(*ID) + ") error: no active uniform \'" + uniformName + "\']");
    }
    return loc;
}

Shader::Shader(const std::string& name)
{
    auto vertexID = compile(loadSource(name + ".vs"), GL_VERTEX_SHADER);
    auto fragmentID = compile(loadSource(name + ".fs"), GL_FRAGMENT_SHADER);

    bool vertexErr = getError(false, *vertexID, GL_COMPILE_STATUS, "vertex shader error log");
    bool fragmentErr = getError(false, *fragmentID, GL_COMPILE_STATUS, "fragment shader error log");

    if(vertexErr || fragmentErr)
        throw std::runtime_error("[shader compilation error: " + name + ']');

    auto deleter = [](GLuint* ID)
    {
        glDeleteProgram(*ID);
        delete ID;
    };
    ID = std::unique_ptr<GLuint, decltype(deleter)>(new GLuint, deleter);
    *ID = glCreateProgram();

    glAttachShader(*ID, *vertexID);
    glAttachShader(*ID, *fragmentID);
    glLinkProgram(*ID);

    if(getError(true, *ID, GL_LINK_STATUS, "program error log"))
        throw std::runtime_error("[program linking error: " + name + ']');

    glDetachShader(*ID, *vertexID);
    glDetachShader(*ID, *fragmentID);
    loadUniformLocations();
}

Shader::Shader(const std::string& vs_source, const std::string& fs_source)
{
    auto vertexID = compile(vs_source, GL_VERTEX_SHADER);
    auto fragmentID = compile(fs_source, GL_FRAGMENT_SHADER);

    bool vertexErr = getError(false, *vertexID, GL_COMPILE_STATUS, "vertex shader error log");
    bool fragmentErr = getError(false, *fragmentID, GL_COMPILE_STATUS, "fragment shader error log");

    if(vertexErr || fragmentErr)
        throw std::runtime_error("[shader compilation error]");

    auto deleter = [](GLuint* ID)
    {
        glDeleteProgram(*ID);
        delete ID;
    };
    ID = std::unique_ptr<GLuint, decltype(deleter)>(new GLuint, deleter);
    *ID = glCreateProgram();

    glAttachShader(*ID, *vertexID);
    glAttachShader(*ID, *fragmentID);
    glLinkProgram(*ID);

    if(getError(true, *ID, GL_LINK_STATUS, "program error log"))
        throw std::runtime_error("[program linking error]");

    glDetachShader(*ID, *vertexID);
    glDetachShader(*ID, *fragmentID);
    loadUniformLocations();
}

std::unique_ptr<GLuint, std::function<void(GLuint*)>> Shader::compile(const std::string& source,
                                                                      GLenum shaderType) const
{
                                                      auto deleter = [](GLuint* ID)
{
    glDeleteShader(*ID);
    delete ID;
};

std::unique_ptr<GLuint, decltype(deleter)> shaderID(new GLuint, deleter);
*shaderID = glCreateShader(shaderType);

const GLchar* sourceGL = source.c_str();
glShaderSource(*shaderID, 1, &sourceGL, nullptr);
glCompileShader(*shaderID);
return std::move(shaderID);
}

std::string Shader::loadSource(const std::string& filename) const
{
    std::ifstream ifs(filename);
    if(!ifs)
        throw std::runtime_error("[could not open file: " + filename + ']');
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

bool Shader::getError(bool isProgram, GLuint ID, GLenum flag, const std::string& message) const
{
    GLint success;
    if(isProgram)
        glGetProgramiv(ID, flag, &success);
    else
        glGetShaderiv(ID, flag, &success);

    if(success == GL_FALSE)
    {
        GLint length;
        if(isProgram)
            glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &length);
        else
            glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &length);

        std::vector<GLchar> infoLog(static_cast<std::size_t>(length));
        if(isProgram)
            glGetProgramInfoLog(ID, length, nullptr, infoLog.data());
        else
            glGetShaderInfoLog(ID, length, nullptr, infoLog.data());

        std::cout << '[' + message << std::endl;
        std::cout.write(infoLog.data(), static_cast<long>(infoLog.size()))
                << ']' << std::endl;
        return true;
    }
    return false;
}

void Shader::loadUniformLocations()
{
    GLint numUniforms;
    glGetProgramiv(*ID, GL_ACTIVE_UNIFORMS, &numUniforms);
    std::vector<char> uniformName(256);

    for(GLuint i = 0; i < static_cast<GLuint>(numUniforms); ++i)
    {
        GLint dum1;
        GLenum dum2;
        glGetActiveUniform(*ID, i, static_cast<GLsizei>(uniformName.size()),
                           nullptr, &dum1, &dum2, uniformName.data());
        GLint location = glGetUniformLocation(*ID, uniformName.data());

        uniformLocations[uniformName.data()] = location;
    }
}
