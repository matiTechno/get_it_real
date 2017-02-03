#ifndef EN_TEXT_HPP
#define EN_TEXT_HPP

#include <glm/vec2.hpp>
#include <text.hpp>

class Text_Entity
{
public:
    Text_Entity(Font& font):
        text(font),
        velocity(0.f, 0.f)
    {}

    virtual ~Text_Entity() = default;
    float life;
    virtual void update(float dt);
    Text text;
    glm::vec2 velocity;

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
        text.color.a = 0.3f;
        velocity = glm::vec2(0.f, -30.f);
    }
private:
    void custom_update(float) override;
};

class Combo_t: public Text_Entity
{
public:
    Combo_t(const glm::vec2& coll_pos, Font& font, std::size_t count);
};

#endif // EN_TEXT_HPP
