#include <game/en_text.hpp>

void Text_Entity::update(float dt)
{
    text.position += velocity * dt;
    life -= dt;
    custom_update(dt);
}

void Min_hp_t::custom_update(float)
{
}

Combo_t::Combo_t(const glm::vec2& coll_pos, Font& font, std::size_t count):
    Text_Entity(font)
{
    text.setText(std::to_string(count) + " combo");
    text.setScale(0.8f);
    text.position = coll_pos - text.getSize() / 2.f;
    life = 1.f;
    text.bloom = true;
    text.color = glm::vec4(152.f, 255.f, 152.f, 0.3f);
    velocity = glm::vec2(0.f, -40.f);
}
