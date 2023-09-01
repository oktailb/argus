#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>

#include "webServer.h"


#include "window.h"
#include "configuration.h"
#include "resources.h"
#include "argus.h"

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

int main(int argc, char *argv[])
{
    usage(argc, argv);

    std::map<std::string, std::string> configuration = readConfiguration(argv[1]);

    bool virtualDesktop = (configuration["General/virtualDesktop"] == "true");

    webServer *srv = new webServer(configuration["General/webServerInterface"], std::stoi(configuration["General/webServerPort"]));
    ArgusWindow w(argv[1]);

    srv->addResource("/ajaxlib.js"  , new ajaxLibRenderer(std::string("text/javascript"), srv));
    srv->addResource("/style.css"   , new cssRenderer(std::string("text/css")           , srv));
    srv->addResource("/"            , new indexRenderer(std::string("text/html")        , srv));
    srv->addResource("/index.html"  , new indexRenderer(std::string("text/html")        , srv));
    srv->addResource("/setVar"      , new setVarRenderer(std::string("text/html")       , &w));
    srv->addResource("/getVar"      , new getVarRenderer(std::string("text/html")       , &w));

    srv->start();

    w.exec();

    return (EXIT_SUCCESS);
}

