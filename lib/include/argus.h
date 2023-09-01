#pragma once
#include <string>
#include <iostream>

void usage(int argc, char **argv)
{
    if (argc == 1)
    {
        std::cerr << argv[0] << " config_file.ini" << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::string version()
{
    return (VERSION);
}
