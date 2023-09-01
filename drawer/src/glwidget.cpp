#include "GL/glew.h"
#include "glwidget.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include "shm.h"
#include "resources.h"

GLWidget::GLWidget(std::map<std::string, std::string> configuration)
    : configuration(configuration)
{
#ifdef WIN32
    virtualDesktop = (configuration["General/virtualDesktop"] == "true");
#elif __linux__
    capturer = new input(configuration);
    width = capturer->getWidth();
    height = capturer->getHeight();
#endif
    videoSync = (configuration["General/videoSync"] == "true");
    stats = (configuration["General/stats"] == "true");
    fps = (stoi(configuration["General/fps"]));

    prefix = configuration["General/Prefix"];
    child = configuration["General/Child"];
    title = configuration["General/title"];

    std::string out0 = prefix + " Argus SharedMemory";

    selectedPointX = 1;
    selectedPointY = 1;
    step = 1;
    editMode = false;

    Zlevel = 1;
    std::cerr << configuration["General/PillowRec"] << std::endl;
    std::cerr << configuration["General/QuadRec"] << std::endl;
    recursionLevel = std::stoi(configuration["General/PillowRec"]);
    quadLevel = std::stoi(configuration["General/QuadRec"]);
    t_Point N  = {std::stod(configuration["Geometry/Nx"]) , std::stod(configuration["Geometry/Ny"])};
    t_Point S  = {std::stod(configuration["Geometry/Sx"]) , std::stod(configuration["Geometry/Sy"])};
    t_Point E  = {std::stod(configuration["Geometry/Ex"]) , std::stod(configuration["Geometry/Ey"])};
    t_Point W  = {std::stod(configuration["Geometry/Wx"]) , std::stod(configuration["Geometry/Wy"])};
    t_Point SE = {std::stod(configuration["Geometry/SEx"]), std::stod(configuration["Geometry/SEy"])};
    t_Point NE = {std::stod(configuration["Geometry/NEx"]), std::stod(configuration["Geometry/NEy"])};
    t_Point NW = {std::stod(configuration["Geometry/NWx"]), std::stod(configuration["Geometry/NWy"])};
    t_Point SW = {std::stod(configuration["Geometry/SWx"]), std::stod(configuration["Geometry/SWy"])};
    t_Point C  = {std::stod(configuration["Geometry/Cx"]) , std::stod(configuration["Geometry/Cy"])};
    GLfloat Na  = std::stod(configuration["Blending/Na"]);
    GLfloat Sa  = std::stod(configuration["Blending/Sa"]);
    GLfloat Ea  = std::stod(configuration["Blending/Ea"]);
    GLfloat Wa  = std::stod(configuration["Blending/Wa"]);
    GLfloat SEa = std::stod(configuration["Blending/SEa"]);
    GLfloat NEa = std::stod(configuration["Blending/NEa"]);
    GLfloat NWa = std::stod(configuration["Blending/NWa"]);
    GLfloat SWa = std::stod(configuration["Blending/SWa"]);
    GLfloat Ca  = std::stod(configuration["Blending/Ca"]);

    GLfloat r  = std::stod(configuration["Color/r"]);
    GLfloat g  = std::stod(configuration["Color/g"]);
    GLfloat b  = std::stod(configuration["Color/b"]);

#ifdef WIN32
    header = (t_argusExchange*) getSHM(out0.c_str(), sizeof(*header));
    width = header->width;
    height = header->height;
#endif

    windowWidth = width;
    windowHeight = height;

    t_Point zero = {0.0, 0.0};
    if (SE == zero
        && SW == zero
        && NE == zero
        && NW == zero)
    {
        NW = {0.0, 0.0};
        NE = {width, 0.0};
        SW = {0.0, height};
        SE = {width, height};
        N = (NW + NE) / 2.0;
        S = (SW + SE) / 2.0;
        E = (NE + SE) / 2.0;
        W = (NW + SW) / 2.0;
        C = (N + S + E + W) / 4.0;
    }

    pillowModel = {
        {
            {NW, N, NE},
            {W , C, E },
            {SW, S, SE},
        },
        {
            {{0.0, 0.0       }, {1.0 / 2.0f, 0.0       }, {1.0, 0.0       } },
            {{0.0, 1.0 / 2.0f}, {1.0 / 2.0f, 1.0 / 2.0f}, {1.0, 1.0 / 2.0f} },
            {{0.0, 1.0       }, {1.0 / 2.0f, 1.0       }, {1.0, 1.0       } },
        },
        {
            {NWa, Na, NEa},
            {Wa , Ca, Ea },
            {SWa, Sa, SEa},
        },
        r,g,b,
        1.0, 1.0
    };
    smoothLen =  std::stof(configuration["General/SmoothLen"]);
    ramp = smoothLen;
    aramp = 1.0f - smoothLen;

    crossSize = 42;

#ifdef WIN32
    int size = 2 * header->size + sizeof(*header);
    shm = (t_argusExchange *)getSHM(out0.c_str(), size);
#endif
    calcPillow(pillowModel, recursionLevel, textureCurrent, Zlevel);
}

GLWidget::~GLWidget()
{
#ifdef __linux__
    delete capturer;
#endif
}

void GLWidget::initializeGL()
{
    glewInit();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &textureCapture);
    glBindTexture(GL_TEXTURE_2D, textureCapture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &textureHSV);
    glBindTexture(GL_TEXTURE_2D, textureHSV);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    clearColorR = 0.0f;
    clearColorG = 0.0f;
    clearColorB = 0.0f;
    clearColorA = 1.0f;
    glClearColor(clearColorR, clearColorG, clearColorB, clearColorA);

    char * data = ((char*)shm) + sizeof(*header);

    glListIndexPicture = glGenLists (1);
    glListIndexGrid = glGenLists (1);

    updateTextureFromSharedMemory(data);
    loadHSVTexture();
    textureCurrent = textureCapture;
    calcPillow(pillowModel, recursionLevel, textureCurrent, Zlevel);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1);
}

void GLWidget::updateTextureFromSharedMemory(char *data) {
    glBindTexture(GL_TEXTURE_2D, textureCapture);
#ifdef WIN32
    width = header->width;
    height = header->height;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
#elif __linux__
    capturer->shoot();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, capturer->getXimg()->data);
#endif
    GLERR;
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLWidget::loadHSVTexture() {
    glBindTexture(GL_TEXTURE_2D, textureHSV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 240, 0, GL_BGRA, GL_UNSIGNED_BYTE, hsv_rgba);
    GLERR;
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLWidget::paintGL()
{
#ifdef WIN32
    char * data;
    if (!header->firstBufferInWrite)
        data = (char*)((char*)shm + sizeof(*header));
    else
        data = (char*)((char*)shm + sizeof(*header) + header->size);
    if (textureCurrent == textureCapture)
        updateTextureFromSharedMemory(data);
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, textureCurrent);
#ifdef __linux__
    updateTextureFromSharedMemory(NULL);
#endif
    if (editMode)
    {
        calcPillow(pillowModel, recursionLevel, textureCapture, Zlevel);
        calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1);
        drawPillow();
        drawPillowFdf();
        drawEditMode(pillowModel, recursionLevel, textureCurrent, Zlevel + 1);
    }
    else
        drawPillow();
}
