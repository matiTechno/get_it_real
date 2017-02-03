#ifndef EN_TEXT_HPP
#define EN_TEXT_HPP

#include <glm/vec2.hpp>
#include <text.hpp>

class Text_Entity
{
public:
    Text_Entity(Font& font):
        text(font)
    {}

    virtual ~Text_Entity() = default;
    glm::vec2 velocity;
    float life;
    virtual void update(float dt);
    Text text;

private:
    virtual void custom_update(float dt)
    {(void)dt;}
};

class Min_hp_t: public Text_Entity
{
public:
    Min_hp_t(Font& font, glm::vec2& proj_size):
        Text_Entity(font)
    {
        text.setText("-1 hp");
        text.position = proj_size / 2.f - text.getSize() / 2.f;
        life = 3.f;
        text.bloom = true;
        velocity = glm::vec2(0.f, -30.f);
    }
private:
    void custom_update(float) override;
};

#endif // EN_TEXT_HPP
