#include <game/animation.hh>
#include <fstream>
#include <sstream>

Animation::Animation(float scale, const std::vector<std::pair<glm::vec4, float>>& frames, bool loop,
                     Origin origin):
    frameTime(frames.front().second),
    scale(scale),
    frames(frames),
    loop(loop),
    state(State::active),
    accumulator(0.f),
    currentFrame(this->frames.cbegin()),
    origin(origin)
{}

Animation::Animation(const Animation& rhs):
    frameTime(rhs.frames.front().second),
    scale(rhs.scale),
    frames(rhs.frames),
    loop(rhs.loop),
    state(rhs.state),
    accumulator(rhs.accumulator),
    currentFrame(frames.cbegin() + (rhs.currentFrame - rhs.frames.cbegin())),
    origin(rhs.origin)
{}

Animation& Animation::operator=(const Animation& rhs)
{
    frameTime = rhs.frames.front().second;
    scale = rhs.scale;
    frames = rhs.frames;
    loop = rhs.loop;
    state = rhs.state;
    accumulator = rhs.accumulator;

    auto x = rhs.currentFrame - rhs.frames.cbegin();
    currentFrame = frames.cbegin() + x;

    origin = rhs.origin;

    return *this;
}

void Animation::update(float dt, Entity& entity)
{
    if(state == State::active)
    {
        accumulator += dt;
        while(accumulator >= frameTime)
        {
            accumulator -= frameTime;
            ++currentFrame;
            if(currentFrame == frames.end())
            {
                if(!loop)
                {
                    state = State::end;
                    return;
                }
                currentFrame = frames.begin();
                frameTime = currentFrame->second;
            }
            else
                frameTime = currentFrame->second;

            set(entity);
        }
    }
}

void Animation::set(Entity& entity) const
{
    glm::vec2 middle = entity.position + entity.size/2.f;
    glm::vec2 bottom = middle + glm::vec2(0.f, entity.size.y/2.f);
    entity.texCoords = currentFrame->first;
    glm::vec2 newSize = glm::vec2(currentFrame->first.z, currentFrame->first.w);
    entity.size = scale * newSize;

    if(origin == Origin::middle)
    {
        entity.position = middle - scale*newSize/2.f;
    }
    else if(origin == Origin::bottom)
    {
        entity.position = bottom - glm::vec2(scale*newSize.x/2.f, scale*newSize.y);
    }
}

// it's a nice hack
std::vector<glm::vec4> lTCimp(const std::string& filename)
{
    std::ifstream ifs(filename);
    if(!ifs)
        throw std::runtime_error("no file: " + filename);

    std::vector<glm::vec4> texCoordsVec;

    std::string line;
    while(std::getline(ifs, line))
    {
        if(line.find("spr name") != std::string::npos)
        {
            int it = 0;
            glm::vec4 texCoords;
            char dummy;
            std::istringstream iss(line);
            while(iss >> dummy)
                if(dummy == '"')
                {
                    switch(it)
                    {
                    case 2: iss >> texCoords.x;
                        break;
                    case 3: iss >> texCoords.y;
                        break;
                    case 4: iss >> texCoords.z;
                        break;
                    case 5: iss >> texCoords.w;
                        break;
                    default:;
                    }
                    ++it;
                    iss >> dummy;
                }
            texCoordsVec.push_back(texCoords);
        }
    }
    if(!texCoordsVec.size())
        throw std::runtime_error("no valid data: " + filename);
    return texCoordsVec;
}

std::vector<std::pair<glm::vec4, float>> loadTexCoords(const std::string& filename,
                                                       float frameTime)
{
    auto texCoordsVec = lTCimp(filename);
    std::vector<std::pair<glm::vec4, float>> frames;

    for(auto& texCs: texCoordsVec)
        frames.emplace_back(texCs, frameTime);

    return frames;
}

std::vector<std::pair<glm::vec4, float>> loadTexCoords(const std::string& filename,
                                                       std::vector<float> frameTimes)
{
    auto texCoordsVec = lTCimp(filename);
    if(texCoordsVec.size() != frameTimes.size())
        throw std::runtime_error("not valid frameTimes size: " + filename);

    std::vector<std::pair<glm::vec4, float>> frames;

    for(std::size_t i = 0; i < texCoordsVec.size(); ++i)
        frames.emplace_back(texCoordsVec[i], frameTimes[i]);

    return frames;
}
