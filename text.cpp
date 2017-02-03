#include <text.hpp>
#include <vector>
#include <algorithm>
#include <renderer_2d.hpp>

Text::Text(const Font& font):
    color(255.f, 255.f, 255.f, 1.f),
    bloom(false),
    font(&font),
    scale(1.f)
{}

Text::Text(const glm::vec4& color, bool bloom, const glm::vec2& position, const Font& font,
           float scale, const std::string& text):
    color(color),
    bloom(bloom),
    position(position),
    font(&font),
    scale(scale),
    text(text)
{
    findSize();
}

void Text::setScale(float scale)
{
    glm::vec2 middle = position + size / 2.f;
    this->scale = scale;
    findSize();
    position = middle - size / 2.f;
}

void Text::setText(const std::string& text)
{
    this->text = text;
    findSize();
}

const std::string& Text::getText() const
{
    return text;
}

const glm::vec2& Text::getSize() const
{
    return size;
}

void Text::findSize()
{
    std::size_t n_new_line = static_cast<std::size_t>(std::count(text.begin(), text.end(), '\n'));
    std::size_t current_newLine = 0;
    std::vector<float> lines_widths(1, 0.f);
    float text_height = font->maxBearingY * scale;
    std::size_t it = 0;
    float below_origin = 0.f;
    for(auto c: text)
    {
        if(c == '\n')
        {
            lines_widths.push_back(0.f);
            it += 1;
            text_height += (font->newLineSpace) * scale;
            ++current_newLine;
        }
        else
        {
            lines_widths[it] += font->characters.at(c).advanceX * scale;

            if(current_newLine == n_new_line)
            {
                auto val = (font->maxBearingY - font->characters.at(c).bearing.y) * scale;
                if(val > below_origin)
                    below_origin = val;
            }
        }
    }
    auto it2 = std::max_element(std::begin(lines_widths), std::end(lines_widths));
    size = glm::vec2(*it2, text_height + below_origin);
}
