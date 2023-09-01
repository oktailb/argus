#include "argus.h"
#include "configuration.h"
#include "input.hpp"
#include "subprocessrunner.h"

int main(int argc, char *argv[])
{
    usage(argc, argv);
    std::map<std::string, std::string> configuration = readConfiguration(argv[1]);

    if (configuration.find("General/Child") != configuration.end())
    {
        input * capt = new input(argv[1]);

        SubProcessRunner *subProcessRunner;
        std::string desktop = "";

        subProcessRunner = new SubProcessRunner(configuration["General/Child"], desktop, argc, argv);
        subProcessRunner->runSubProcess();

        while (subProcessRunner->active()) {
            capt->shoot();
        }
        delete subProcessRunner;
        delete capt;
    }
}

