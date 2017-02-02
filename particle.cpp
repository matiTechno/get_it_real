#include <particle.hh>
#include <glm/gtc/matrix_transform.hpp>

Particle::Particle(const glm::vec2& position, float size, const glm::vec4& color, float life,
                   const glm::vec2& velocity, const glm::vec2& acceleration):
    position(position),
    size(size),
    color(color),
    life(life),
    maxLife(life),
    velocity(velocity),
    acceleration(acceleration)
{}

void Particle::update(float dt)
{
    position = position + velocity * dt + 0.5f * acceleration * glm::pow(dt, 2.f);
    velocity = velocity + acceleration * dt;
}

ParticleGenerator::ParticleGenerator(float spawnTime, const glm::vec2& position, float life, const glm::vec4 spawnRange,
                                     bool isSpawnCirlce, GLenum sFactor, GLenum dFactor, const PData& pData,
                                     std::function<void(float)> customUpdate, std::size_t numParticles):
    spawnTime(spawnTime),
    position(position),
    accumulator(0.f),
    life(life),
    spawnRange(spawnRange),
    sFactor(sFactor),
    dFactor(dFactor),
    lastActive(-1),
    maxParticles(static_cast<std::size_t>(1.f/spawnTime * pData.lifeRange.y + 1.f)),
    pData(pData),
    isSpawnCircle(isSpawnCirlce),
    customUpdate(customUpdate)
{
    std::random_device rd;
    rng.seed(rd());

    if(numParticles > maxParticles)
        maxParticles = numParticles;

    pPool.reserve(maxParticles);
    vboData.reserve(maxParticles);

    vao.bind();

    GLfloat vertices[] =
    {
        // pos....texCoord
        0.f, 0.f, 0.f, 0.f,
        1.f, 0.f, 1.f, 0.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        0.f, 1.f, 0.f, 1.f,
        0.f, 0.f, 0.f, 0.f
    };

    vboStatic.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    vboDynamic.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(maxParticles * sizeof(Instance)),
                 nullptr, GL_DYNAMIC_DRAW);

    // color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // model matrix
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Instance), reinterpret_cast<const void*>(sizeof(glm::vec4)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
}

std::size_t ParticleGenerator::spawnParticle()
{
    // size
    std::uniform_real_distribution<float> uni(pData.sizeRange.x, pData.sizeRange.y);
    float pSize = uni(rng);
    // position
    uni = std::uniform_real_distribution<float>(0.f, 1.f);
    glm::vec2 pPosition;
    pPosition.x = uni(rng) * spawnRange.z;
    if(isSpawnCircle)
    {
        assert(int(spawnRange.z) == int(spawnRange.w));
        float yRange = glm::sqrt(glm::pow(spawnRange.z / 2.f, 2.f) - glm::pow(pPosition.x - spawnRange.z / 2.f, 2.f));
        uni = std::uniform_real_distribution<float>(-yRange, yRange);
        pPosition.y = uni(rng) + spawnRange.z / 2.f;
    }
    else
        pPosition.y = uni(rng) * spawnRange.w;

    pPosition.x += position.x + spawnRange.x - pSize / 2.f;
    pPosition.y += position.y + spawnRange.y - pSize / 2.f;
    // color
    glm::vec4 pColor;
    uni = std::uniform_real_distribution<float>(pData.lowColor.r, pData.highColor.r);
    pColor.r = uni(rng);
    uni = std::uniform_real_distribution<float>(pData.lowColor.g, pData.highColor.g);
    pColor.g = uni(rng);
    uni = std::uniform_real_distribution<float>(pData.lowColor.b, pData.highColor.b);
    pColor.b = uni(rng);
    uni = std::uniform_real_distribution<float>(pData.lowColor.a, pData.highColor.a);
    pColor.a = uni(rng);
    // life
    uni = std::uniform_real_distribution<float>(pData.lifeRange.x, pData.lifeRange.y);
    float pLife = uni(rng);
    // velocity
    glm::vec2 pVelocity;
    uni = std::uniform_real_distribution<float>(pData.velocityRange.x, pData.velocityRange.z);
    pVelocity.x = uni(rng);
    uni = std::uniform_real_distribution<float>(pData.velocityRange.y, pData.velocityRange.w);
    pVelocity.y = uni(rng);
    // acceleration
    glm::vec2 pAcceleration;
    uni = std::uniform_real_distribution<float>(pData.accelerationRange.x, pData.accelerationRange.z);
    pAcceleration.x = uni(rng);
    uni = std::uniform_real_distribution<float>(pData.accelerationRange.y, pData.accelerationRange.w);
    pAcceleration.y = uni(rng);

    if(static_cast<std::size_t>(lastActive + 1) == pPool.size())
    {
        assert(static_cast<std::size_t>(lastActive + 1 + 1) <= maxParticles);
        pPool.emplace_back(pPosition, pSize, pColor, pLife, pVelocity, pAcceleration);
        vboData.emplace_back();
    }
    else
    {
        pPool[static_cast<std::size_t>(lastActive + 1)] = {pPosition, pSize, pColor, pLife, pVelocity, pAcceleration};
    }
    ++lastActive;

    return static_cast<std::size_t>(lastActive);
}

void ParticleGenerator::killParticle(Particle& particle)
{
    std::swap(particle, pPool[static_cast<std::size_t>(lastActive)]);
    --lastActive;
}

void ParticleGenerator::updateVboInstance(std::size_t i)
{
    vboData[i].color.r = pPool[i].color.r / 255.f;
    vboData[i].color.g = pPool[i].color.g / 255.f;
    vboData[i].color.b = pPool[i].color.b / 255.f;
    vboData[i].color.a = pPool[i].color.a;

    vboData[i].model = glm::vec3(pPool[i].position, pPool[i].size);
}

void ParticleGenerator::update(float dt)
{
    for(std::size_t i = 0; i < static_cast<std::size_t>(lastActive + 1); ++i)
    {
        pPool[i].life -= dt;
        if(pPool[i].life <= 0.f)
            killParticle(pPool[i]);
        else
        {
            pPool[i].update(dt);
            if(pData.customUpdate)
                pData.customUpdate(dt, pPool[i]);
            updateVboInstance(i);
        }
    }

    if(customUpdate)
        customUpdate(dt);
    accumulator += dt;

    if(life > 0.f)
        life -= dt;

    while(accumulator >= spawnTime && (life <= -1000.f || life > 0.f))
    {
        updateVboInstance(spawnParticle());
        accumulator -= spawnTime;
    }
}

bool ParticleGenerator::isActive() const
{
    return (life <= -1000.f || (life < 0.f && lastActive != -1) || life > 0.f);
}

const glm::vec2& ParticleGenerator::getPosition() const
{
    return position;
}

void ParticleGenerator::setPosition(const glm::vec2& position)
{
    this->position = position;
}

const PData& ParticleGenerator::getPdata() const
{
    return pData;
}

PData& ParticleGenerator::getPdata()
{
    return const_cast<PData&>(
                static_cast<const ParticleGenerator&>(*this).getPdata()
                );
}
