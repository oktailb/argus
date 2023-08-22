#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <windows.h>
#include "window.h"
#include "configuration.h"
#include "desktop.h"
#include "webServer.h"


class cssRenderer : public webRenderer
{
public:
    cssRenderer(std::string mimeType, void * data)
        :
          webRenderer(mimeType)
    {
    }
    virtual void render(const std::string url);
};

void cssRenderer::render(const std::string url)
{
    LOAD_RESOURCE(libs_processingRemoteControl_res_style_css, webResource text);
    setData(text.data(), text.size());
}

class ajaxLibRenderer : public webRenderer
{
public:
    ajaxLibRenderer(std::string mimeType, void * data)
        :
          webRenderer(mimeType)
    {
    }
    virtual void render(const std::string url);
};

void ajaxLibRenderer::render(const std::string url)
{
    LOAD_RESOURCE(libs_processingRemoteControl_res_ajaxlib_js, webResource text);
    setData(text.data(), text.size());
}


int main(int argc, char *argv[])
{
    std::map<std::string, std::string> configuration = readConfiguration("config.ini");

    bool virtualDesktop = (configuration["General/virtualDesktop"] == "true");

    webServer *srv = new webServer(configuration["General/webServerInterface"], std::stoi(configuration["General/webServerPort"]));
    srv->addResource("/ajaxlib.js"  , new ajaxLibRenderer(std::string("text/javascript"), srv));
    srv->addResource("/style.css"   , new cssRenderer(std::string("text/css")           , srv));
//    srv->addResource("/"            , new remoteControlIndexLibRenderer(std::string("text/html")        , NULL      , this));
//    srv->addResource("/index.html"  , new remoteControlIndexLibRenderer(std::string("text/html")        , NULL      , this));
//    srv->addResource("/setVar"      , new editFormRenderer(std::string("text/html")                     , modelCopy , this));
//    srv->addResource("/getVar"      , new remoteControlVariablesRenderer(std::string("text/html")       , modelCopy , this));

    srv->start();
    Window w(configuration);

//    if (virtualDesktop)
//        createDedicatedDesktop("Argus Desktop", w.getGlThread());
    w.exec();

//    if (virtualDesktop)
//        restoreOriginalDesktop();

}
