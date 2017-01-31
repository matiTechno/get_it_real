#include <app.hpp>
#include <stdexcept>
#include <iostream>

int main()
{
    try
    {
        App app;
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
