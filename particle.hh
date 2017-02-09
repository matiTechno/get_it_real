#ifndef PARTICLE_HH
#define PARTICLE_HH

#include <random>
#include <opengl/buffers.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <functional>
class Texture;

struct Particle
{
    Particle(const glm::vec2& position, float size, const glm::vec4& color, float life,
             const glm::vec2& velocity, const glm::vec2& acceleration);

    void update(float dt);

    glm::vec2 position;
    float size;
    glm::vec4 color;
    float life;
    float maxLife;
    glm::vec2 velocity;
    glm::vec2 acceleration;
};

struct PData
{
    bool bloom;
    glm::vec2 sizeRange;
    glm::vec4 lowColor;
    glm::vec4 highColor;
    glm::vec2 lifeRange;
    Texture* texture;
    glm::vec4 velocityRange;
    glm::vec4 accelerationRange;
    std::function<void(float dt, Particle&)> customUpdate;
};

struct Instance
{
    glm::vec4 color;
    // translation + scale (same for x and y)
    glm::vec3 model;
};

class ParticleGenerator
{
public:
    friend class Renderer_2D;

    ParticleGenerator(float spawnTime, const glm::vec2& position, float life, const glm::vec4 spawnRange,
                      bool isSpawnCircle, GLenum sFactor, GLenum dFactor, const PData& pData,
                      std::function<void(float)> customUpdate = {}, std::size_t numParticles = 0);

    // use one time per frame
    void update(float dt);
    bool isActive() const;
    const glm::vec2& getPosition() const;
    void setPosition(const glm::vec2& position);
    const PData& getPdata() const;
    PData& getPdata();

    // beautiful hack :)
    float spawnTime;

private:
    glm::vec2 position;
    float accumulator;
    // <= -1000 means it's always active
    float life;
    // left top corner relative to generator position
    // + spawn size
    glm::vec4 spawnRange;
    GLenum sFactor;
    GLenum dFactor;
    int lastActive;
    std::size_t maxParticles;
    VAO vao;
    BO vboStatic;
    BO vboDynamic;
    std::vector<Particle> pPool;
    std::vector<Instance> vboData;
    std::mt19937 rng;
    PData pData;
    bool isSpawnCircle;

    std::function<void(float)> customUpdate;
    std::size_t spawnParticle();
    void killParticle(Particle& particle);
    void updateVboInstance(std::size_t i);
};

#endif // PARTICLE_HH
