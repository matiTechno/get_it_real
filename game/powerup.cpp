#include <game/powerup.hpp>
#include <algorithm>

PowerUp::PowerUp(const glm::vec2& pos, Texture& tex, Animation& anim, PW_Type type):
    AnimEntity(pos, tex, anim),
    type(type)
{

}

void PowerUp_System::render(Renderer_2D& rendr)
{
    for(auto& powerup: powerups)
    {
        rendr.render(powerup->getSprite());
    }
}

void PowerUp_System::update(float dt)
{
    for(auto& powerup: powerups)
    {
        powerup->update(dt);
    }
    powerups.erase(std::remove_if(powerups.begin(), powerups.end(), [](std::unique_ptr<PowerUp>& pw){
                       if(pw->isDead)
                       return true;
                       return false;
                   }), powerups.end());
}

void PowerUp::update_after_anim(float dt)
{
    move(dt);
    if(immuneTime >= 0.f)
        immuneTime -= dt;
}
