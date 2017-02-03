#include <game/en_text.hpp>

void Text_Entity::update(float dt)
{
    text.position += velocity * dt;
    life -= dt;
    custom_update(dt);
}

void Min_hp_t::custom_update(float dt)
{
    text.setScale(text.getScale() + 0.2f * dt);
}
