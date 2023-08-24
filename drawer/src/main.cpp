#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>

#ifdef WIN32
#include <windows.h>
#include "desktop.h"
#elif __linux__
#endif

#include "window.h"
#include "configuration.h"
#include "webServer.h"
#include "resources.h"

class cssRenderer : public webRenderer {
public:
    cssRenderer(std::string mimeType, void * data) : webRenderer(mimeType) {}
    virtual void render(const std::string url) { setData(style_css, style_css_size); }
};

class ajaxLibRenderer : public webRenderer {
public:
    ajaxLibRenderer(std::string mimeType, void * data) : webRenderer(mimeType) {}
    virtual void render(const std::string url) { setData(ajaxlib_js, ajaxlib_js_size); }
};

class indexRenderer : public webRenderer {
public:
    indexRenderer(std::string mimeType, void * data) : webRenderer(mimeType) {}
    virtual void render(const std::string url) { setData(index_html, index_html_size); }
};

class setVarRenderer : public webRenderer {
public:
    setVarRenderer(std::string mimeType, void * data) : webRenderer(mimeType) {}
    virtual void render(const std::string url) {
        std::ostringstream text;

        std::string target = getVar("target");
        text << target;
        setData(text.str().c_str(), text.str().size());
    }
};

class getVarRenderer : public webRenderer {
public:
    getVarRenderer(std::string mimeType, void * data) : webRenderer(mimeType) {}
    virtual void render(const std::string url) { setData(index_html, index_html_size); }
};

#ifdef WIN32
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#elif __linux__
int main(int argc, char *argv[])
#endif
{
    std::cerr << "TEST" << std::endl;

    std::map<std::string, std::string> configuration = readConfiguration("config.ini");

    bool virtualDesktop = (configuration["General/virtualDesktop"] == "true");

    webServer *srv = new webServer(configuration["General/webServerInterface"], std::stoi(configuration["General/webServerPort"]));
    ArgusWindow w(configuration);

    srv->addResource("/ajaxlib.js"  , new ajaxLibRenderer(std::string("text/javascript"), srv));
    srv->addResource("/style.css"   , new cssRenderer(std::string("text/css")           , srv));
    srv->addResource("/"            , new indexRenderer(std::string("text/html")        , srv));
    srv->addResource("/index.html"  , new indexRenderer(std::string("text/html")        , srv));
    srv->addResource("/setVar"      , new setVarRenderer(std::string("text/html")       , &w));
    srv->addResource("/getVar"      , new getVarRenderer(std::string("text/html")       , &w));

    srv->start();

//    if (virtualDesktop)
//        createDedicatedDesktop("Argus Desktop", w.getGlThread());
    w.exec();

//    if (virtualDesktop)
//        restoreOriginalDesktop();

}
