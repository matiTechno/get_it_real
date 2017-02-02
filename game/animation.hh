#ifndef ANIMATION_HH
#define ANIMATION_HH

#include <vector>
#include <string>
#include <glm/vec4.hpp>
#include <game/entity.hpp>

enum class State
{
    active,
    end
};

enum class Origin
{
    middle,
    bottom
};

class Animation
{
public:
    Animation(float scale, const std::vector<std::pair<glm::vec4, float>>& frames, bool loop,
              Origin origin);

    Animation(const Animation& rhs);
    Animation& operator=(const Animation& rhs);

    void update(float dt, Entity& entity);
    void set(Entity& entity) const;
    State getState() const;

private:
    float frameTime;
    float scale;
    std::vector<std::pair<glm::vec4, float>> frames;
    bool loop;
    State state;
    float accumulator;
    std::vector<std::pair<glm::vec4, float>>::const_iterator currentFrame;
    Origin origin;
};

inline State Animation::getState() const
{
    return state;
}

// works with darkFunction Editor *.sprite files
std::vector<std::pair<glm::vec4, float>> loadTexCoords(const std::string& filename,
                                                       float frameTime);
std::vector<std::pair<glm::vec4, float>> loadTexCoords(const std::string& filename,
                                                       std::vector<float> frameTimes);

#endif // ANIMATION_HH
