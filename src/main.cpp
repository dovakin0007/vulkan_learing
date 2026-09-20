
#include <first_app.h>
#include <stdexcept>
#include <cstdlib>
#include <iostream>

int main()
{

    try
    {
        the_engine::FirstApp app{};
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}