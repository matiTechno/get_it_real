#ifndef INPUT_HPP
#define INPUT_HPP

#include <unordered_map>

template<typename T, typename Hash>
class Input
{
public:
    void begin_new_frame();

    void pressEvent(T code);

    void releaseEvent(T code);

    bool wasPressed(T code) const;

    bool isPressed(T code) const;

    bool wasReleased(T code) const;

    const std::unordered_map<T, bool, Hash>& get_map_wasPressed() const;

    const std::unordered_map<T, bool, Hash>& get_map_isPressed() const;

    const std::unordered_map<T, bool, Hash>& get_map_wasReleased() const;

private:
    std::unordered_map<T, bool, Hash> map_wasPressed;
    std::unordered_map<T, bool, Hash> map_isPressed;
    std::unordered_map<T, bool, Hash> map_wasReleased;
};

template<typename T, typename Hash>
void Input<T, Hash>::begin_new_frame()
{
    map_wasPressed.clear();
    map_wasReleased.clear();
}

template<typename T, typename Hash>
void Input<T, Hash>::pressEvent(T code)
{
    map_isPressed[code] = true;
    map_wasPressed[code] = true;
}

template<typename T, typename Hash>
void Input<T, Hash>::releaseEvent(T code)
{
    map_wasReleased[code] = true;
    map_isPressed.erase(code);
}

template<typename T, typename Hash>
bool Input<T, Hash>::wasPressed(T code) const
{
    auto i = map_wasPressed.find(code);
    if(i == map_wasPressed.end())
        return false;
    return true;
}

template<typename T, typename Hash>
bool Input<T, Hash>::isPressed(T code) const
{
    auto i = map_isPressed.find(code);
    if(i == map_isPressed.end())
        return false;
    return true;
}

template<typename T, typename Hash>
bool Input<T, Hash>::wasReleased(T code) const
{
    auto i = map_wasReleased.find(code);
    if(i == map_wasReleased.end())
        return false;
    return true;
}

template<typename T, typename Hash>
const std::unordered_map<T, bool, Hash>& Input<T, Hash>::get_map_wasPressed() const
{
    return map_wasPressed;
}

template<typename T, typename Hash>
const std::unordered_map<T, bool, Hash>& Input<T, Hash>::get_map_isPressed() const
{
    return map_isPressed;
}

template<typename T, typename Hash>
const std::unordered_map<T, bool, Hash>& Input<T, Hash>::get_map_wasReleased() const
{
    return map_wasReleased;
}

#endif // INPUT_HPP
