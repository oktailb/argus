#include "GL/glew.h"
#include "glwidget.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

#include "configuration.h"
#include "shm.h"

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
//            {{0.0, 0.0          }, {width / 2.0f, 0.0          }, {width, 0.0          } },
//            {{0.0, height / 2.0f}, {width / 2.0f, height / 2.0f}, {width, height / 2.0f} },
//            {{0.0, height       }, {width / 2.0f, height       }, {width, height       } },
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
    int size = header->size + sizeof(*header);
    shm = (t_argusExchange *)getSHM(out0.c_str(), size);
#endif
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
}

GLWidget::~GLWidget()
{
#ifdef __linux__
    delete capturer;
#endif
}

#define GLERR {GLenum error = glGetError(); \
if (error != GL_NO_ERROR) { \
    std::cerr << "OpenGL error: " << error << " in file " << __FILE__ << " on line " << __LINE__ << ": "  << std::endl; \
} \
} \


void GLWidget::initializeGL()
{
    glewInit();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
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

    updateTextureFromSharedMemory();
}

void GLWidget::updateTextureFromSharedMemory() {
    glBindTexture(GL_TEXTURE_2D, texture);
#ifdef WIN32
    header = (t_argusExchange *)shm;
    width = header->width;
    height = header->height;
    while (header->inWrite);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)shm + sizeof(*header));
#elif __linux__
    capturer->shoot();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, capturer->getXimg()->data);
#endif
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
    updateTextureFromSharedMemory();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);

    drawPillow();
    if (editMode)
    {
        drawPillowFdf();
        drawEditMode(pillowModel, recursionLevel, texture, Zlevel + 1, true, true);
    }
}

void GLWidget::resizeGL(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    int side = std::min(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

#ifdef __linux__
input *GLWidget::getCapturer() const
{
    return capturer;
}
#endif

void GLWidget::setStep(int newStep)
{
    step = newStep;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::setLastPos(int x, int y)
{
    lastPos.x = x;
    lastPos.y = y;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::toggleEditMode()
{
    editMode = !editMode;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::increaseAlpha()
{
    pillowModel.alpha[selectedPointX][selectedPointY] += 0.01;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::decreaseAlpha()
{
    pillowModel.alpha[selectedPointX][selectedPointY] -= 0.01;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::increaseSmoothLen()
{
    smoothLen += 0.01;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::decreaseSmoothLen()
{
    smoothLen -= 0.01;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::increasePillowRecursion()
{
    std::cerr << "recursionLevel++";

    recursionLevel++;
    if (recursionLevel >= 16)
        recursionLevel = 15;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::decreasePillowRecursion()
{
    std::cerr << "recursionLevel--";

    recursionLevel--;
    if (recursionLevel <= 1)
        recursionLevel = 1;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::increaseQuadRecursion()
{
    std::cerr << "recursionLevel++";

    quadLevel++;
    if (quadLevel >= 16)
        quadLevel = 15;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::decreaseQuadRecursion()
{
    std::cerr << "recursionLevel--";

    quadLevel--;
    if (quadLevel <= 1)
        quadLevel = 1;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::movePointTo(int x, int y)
{
    pillowModel.points[selectedPointX][selectedPointY].x = x;
    pillowModel.points[selectedPointX][selectedPointY].y = height - y;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::movePointUp()
{
    pillowModel.points[selectedPointX][selectedPointY].y += step;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::movePointDown()
{
    pillowModel.points[selectedPointX][selectedPointY].y -= step;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::movePointLeft()
{
    pillowModel.points[selectedPointX][selectedPointY].x -= step;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::movePointRight()
{
    pillowModel.points[selectedPointX][selectedPointY].x += step;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::adjustR(bool way)
{
    if (way)
        pillowModel.r += 0.01;
    else
        pillowModel.r -= 0.01;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::adjustG(bool way)
{
    if (way)
        pillowModel.g += 0.01;
    else
        pillowModel.g -= 0.01;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::adjustB(bool way)
{
    if (way)
        pillowModel.b += 0.01;
    else
        pillowModel.b -= 0.01;
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

void GLWidget::save(std::string conf)
{
    configuration["Geometry/Sx"]  = std::to_string(pillowModel.points[Up]    [Center].x);
    configuration["Geometry/Sy"]  = std::to_string(pillowModel.points[Up]    [Center].y);
    configuration["Geometry/Nx"]  = std::to_string(pillowModel.points[Down]  [Center].x);
    configuration["Geometry/Ny"]  = std::to_string(pillowModel.points[Down]  [Center].y);
    configuration["Geometry/Wx"]  = std::to_string(pillowModel.points[Middle][Left].x);
    configuration["Geometry/Wy"]  = std::to_string(pillowModel.points[Middle][Left].y);
    configuration["Geometry/Ex"]  = std::to_string(pillowModel.points[Middle][Right].x);
    configuration["Geometry/Ey"]  = std::to_string(pillowModel.points[Middle][Right].y);
    configuration["Geometry/SWx"] = std::to_string(pillowModel.points[Up]    [Left].x);
    configuration["Geometry/SWy"] = std::to_string(pillowModel.points[Up]    [Left].y);
    configuration["Geometry/NWx"] = std::to_string(pillowModel.points[Down]  [Left].x);
    configuration["Geometry/NWy"] = std::to_string(pillowModel.points[Down]  [Left].y);
    configuration["Geometry/SEx"] = std::to_string(pillowModel.points[Up]    [Right].x);
    configuration["Geometry/SEy"] = std::to_string(pillowModel.points[Up]    [Right].y);
    configuration["Geometry/NEx"] = std::to_string(pillowModel.points[Down]  [Right].x);
    configuration["Geometry/NEy"] = std::to_string(pillowModel.points[Down]  [Right].y);
    configuration["Geometry/Cx"]  = std::to_string(pillowModel.points[Middle][Center].x);
    configuration["Geometry/Cy"]  = std::to_string(pillowModel.points[Middle][Center].y);

    configuration["Blending/Na"]  = std::to_string(pillowModel.alpha[Down]  [Center]);
    configuration["Blending/Sa"]  = std::to_string(pillowModel.alpha[Up]    [Center]);
    configuration["Blending/Ea"]  = std::to_string(pillowModel.alpha[Middle][Right]);
    configuration["Blending/Wa"]  = std::to_string(pillowModel.alpha[Middle][Left]);
    configuration["Blending/NEa"]  = std::to_string(pillowModel.alpha[Down] [Right]);
    configuration["Blending/SEa"]  = std::to_string(pillowModel.alpha[Up]   [Right]);
    configuration["Blending/NWa"]  = std::to_string(pillowModel.alpha[Down] [Left]);
    configuration["Blending/SWa"]  = std::to_string(pillowModel.alpha[Up]   [Left]);
    configuration["Blending/Ca"]  = std::to_string(pillowModel.alpha[Middle][Center]);

    configuration["Color/r"]  = std::to_string(pillowModel.r);
    configuration["Color/g"]  = std::to_string(pillowModel.g);
    configuration["Color/b"]  = std::to_string(pillowModel.b);

    configuration["General/PillowRec"] = std::to_string(recursionLevel);
    configuration["General/Child"] = child;
    configuration["General/Prefix"] = prefix;
    configuration["General/QuadRec"] = std::to_string(quadLevel);
    configuration["General/SmoothLen"] = std::to_string(pillowModel.SmoothLen);
    configuration["General/title"] = title;
#ifdef WIN32
    configuration["General/virtualDesktop"] = virtualDesktop?"true":"false";
#endif
    configuration["General/fps"] = std::to_string(fps);
    configuration["General/videoSync"] = videoSync?"true":"false";

    saveConfiguration(configuration, "config.ini");
}

void GLWidget::selectPoint(int id)
{
    switch(id) {
    case 1:
    {
        selectedPointX = Left;
        selectedPointY = Down;
        break;
    }
    case 2:
    {
        selectedPointX = Left;
        selectedPointY = Middle;
        break;
    }
    case 3:
    {
        selectedPointX = Left;
        selectedPointY = Up;
        break;
    }
    case 4:
    {
        selectedPointX = Center;
        selectedPointY = Down;
        break;
    }
    case 5:
    {
        selectedPointX = Center;
        selectedPointY = Middle;
        break;
    }
    case 6:
    {
        selectedPointX = Center;
        selectedPointY = Up;
        break;
    }
    case 7:
    {
        selectedPointX = Right;
        selectedPointY = Down;
        break;
    }
    case 8:
    {
        selectedPointX = Right;
        selectedPointY = Middle;
        break;
    }
    case 9:
    {
        selectedPointX = Right;
        selectedPointY = Up;
        break;
    }
    }
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
}

