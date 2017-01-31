#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glad/glad.h>
#include <memory>
#include <functional>
#include <unordered_map>

// supported filenames: *.vs *.fs
class Shader
{
public:
    // filename without extension
    Shader(const std::string& name);
    Shader(const std::string& vs_source, const std::string& fs_source);

    void bind() const;
    GLint getUniLoc(const std::string& uniformName) const;

private:
    static GLuint boundID;
    std::unique_ptr<GLuint, std::function<void(GLuint*)>> ID;
    std::unordered_map<std::string, GLint> uniformLocations;

    std::string loadSource(const std::string& filename) const;
    bool getError(bool isProgram, GLuint ID, GLenum flag, const std::string& message) const;
    void loadUniformLocations();
    std::unique_ptr<GLuint, std::function<void(GLuint*)>> compile(const std::string& source,
                                                                  GLenum shaderType) const;
};

#endif // SHADER_HPP
