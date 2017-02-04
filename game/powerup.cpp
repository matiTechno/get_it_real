#include <game/powerup.hpp>
#include <algorithm>

void PowerUp_System::render(Renderer_2D& rendr)
{
    for(auto& powerup: powerups)
    {
        rendr.render(powerup.getSprite());
    }
}

void PowerUp_System::update(float dt)
{
    for(auto& powerup: powerups)
    {
        powerup.update(dt);
    }
    powerups.erase(std::remove_if(powerups.begin(), powerups.end(), [](PowerUp& pw){
                       if(pw.isDead)
                       return true;
                       return false;
                   }), powerups.end());
}
