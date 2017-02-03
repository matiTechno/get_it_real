#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
struct Font;

class Text
{
public:
    friend class Renderer_2D;

    Text(const Font& font);
    Text(const glm::vec4& color, bool bloom, const glm::vec2& position, const Font& font,
         float scale, const std::string& text);

    float getScale() const
    {
        return scale;
    }
    void setScale(float scale);
    void setText(const std::string& text);
    const std::string& getText() const;
    const glm::vec2& getSize() const;

    glm::vec4 color;
    bool bloom;
    glm::vec2 position;

private:
    const Font* font;
    float scale;
    std::string text;
    glm::vec2 size;

    void findSize();
};

#endif // TEXT_HPP
