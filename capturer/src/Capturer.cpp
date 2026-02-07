#include "argus.h"
#include "argusConfig.h"
#include "input.hpp"
#include "subprocessrunner.h"

int main(int argc, char *argv[])
{
    usage(argc, argv);
    argus::ArgusConfig config;
    if (!argus::loadConfig(argv[1], config).ok)
        config = argus::ArgusConfig();

    if (!config.general.child.empty())
    {
        input * capt = new input(argv[1]);

        SubProcessRunner *subProcessRunner;
        std::string desktop = "";

        subProcessRunner = new SubProcessRunner(config.general.child, desktop, argc, argv);
        subProcessRunner->runSubProcess();

        while (subProcessRunner->active()) {
            capt->shoot();
        }
        delete subProcessRunner;
        delete capt;
    }
}

