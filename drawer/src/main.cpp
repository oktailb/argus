#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <windows.h>
#include "window.h"
#include "configuration.h"
#include "desktop.h"

int main(int argc, char *argv[])
{
    std::map<std::string, std::string> configuration = readConfiguration("config.ini");

    bool virtualDesktop = (configuration["General/virtualDesktop"] == "true");

    Window w(configuration);

//    if (virtualDesktop)
//        createDedicatedDesktop("Argus Desktop", w.getGlThread());
    w.exec();

//    if (virtualDesktop)
//        restoreOriginalDesktop();

}
