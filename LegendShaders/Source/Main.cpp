#include <print>
#include <slang.h>

#include "Options.hpp"

int main(const int argc, char* argv[])
{
    Options options(argc, argv);
    options.Print();



    return 0;
}
