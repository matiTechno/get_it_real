#ifndef ANIMENTITY_HPP
#define ANIMENTITY_HPP

#include <game/entity.hpp>
#include <game/animation.hh>

class AnimEntity: public Entity
{
public:
    AnimEntity(const glm::vec2& pos, Texture& tex, Animation& anim):
        Entity(),
        anim(anim)
    {
        // so the midddle of entity will be in pos (not the left corner)
        this->texture = &tex;
        position = pos;
        anim.set(*this);
    }

    Animation anim;

    void update(float dt) override
    {
        anim.update(dt, *this);
        update_after_anim(dt);
    }

private:
    virtual void update_after_anim(float dt);
};
#endif // ANIMENTITY_HPP
