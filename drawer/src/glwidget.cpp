#include "GL/glew.h"
#include "glwidget.h"
#include "types.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

#include "configuration.h"
#include "shm.h"

GLWidget::GLWidget(std::map<std::string, std::string> configuration)
    : configuration(configuration)
{
    virtualDesktop = (configuration["General/virtualDesktop"] == "true");

    prefix = configuration["General/Prefix"];
    child = configuration["General/Child"];
    title = configuration["General/title"];

    std::string out0 = prefix + " Argus SharedMemory";

    width = 1920;
    height = 1080;

    selectedPointX = 1;
    selectedPointY = 1;
    step = 1;

    windowWidth = width;
    windowHeight = height;

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

    header = (t_argusExchange*) getSHM(out0.c_str(), sizeof(*header));
    width = header->width;
    height = header->height;

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

    int size = header->size + sizeof(*header);
    shm = (t_argusExchange *)getSHM(out0.c_str(), size);
}

GLWidget::~GLWidget()
{
}

#define GLERR {GLenum error = glGetError(); \
if (error != GL_NO_ERROR) { \
    std::cerr << "OpenGL error: " << error << " on line " << __LINE__ << ": "  << std::endl; \
}\
}\

void GLWidget::initializeGL()
{
    glewInit();
    //initializeOpenGLFunctions();
    // Activation des fonctionnalités OpenGL nécessaires pour le rendu 2D et les textures
    glMatrixMode(GL_PROJECTION);
    GLERR
    glLoadIdentity();
    GLERR
    glOrtho(0, width, 0, height, -10.0, 10.0); // Projection orthographique 2D
    GLERR
    glMatrixMode(GL_MODELVIEW);
    GLERR
    glLoadIdentity();
    GLERR

    glEnable(GL_TEXTURE_2D);
    GLERR
    glGenTextures(1, &texture);
    GLERR
    glBindTexture(GL_TEXTURE_2D, texture);
    GLERR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GLERR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLERR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GLERR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLERR
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLERR
    glEnable( GL_BLEND );
    GLERR

    clearColorR = 0.0f;
    clearColorG = 0.0f;
    clearColorB = 0.0f;
    clearColorA = 1.0f;
    glClearColor(clearColorR, clearColorG, clearColorB, clearColorA);
    GLERR

    // Assurez-vous que la texture est mise à jour avant le rendu
    //updateTextureFromSharedMemory();
}

void GLWidget::updateTextureFromSharedMemory() {
    glBindTexture(GL_TEXTURE_2D, texture);
    header = (t_argusExchange *)shm;
    width = header->width;
    height = header->height;
    while (header->inWrite);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, header->width, header->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (char*)shm + sizeof(*header));
    GLERR;
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void GLWidget::paintGL()
{
    updateTextureFromSharedMemory();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);

    drawPillow(pillowModel, recursionLevel, texture, Zlevel, editMode, 1);
    if (editMode)
        drawEditMode(pillowModel, recursionLevel, texture, Zlevel + 1, editMode, 1);
}

void GLWidget::resizeGL(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    int side = std::min(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}

void GLWidget::setStep(int newStep)
{
    step = newStep;
}

void GLWidget::setLastPos(int x, int y)
{
    lastPos.x = x;
    lastPos.y = y;
}

void GLWidget::toggleEditMode()
{
    editMode = !editMode;
}

void GLWidget::increaseAlpha()
{
    pillowModel.alpha[selectedPointX][selectedPointY] += 0.01;
}

void GLWidget::decreaseAlpha()
{
    pillowModel.alpha[selectedPointX][selectedPointY] -= 0.01;
}

void GLWidget::increaseSmoothLen()
{
    smoothLen += 0.01;
}

void GLWidget::decreaseSmoothLen()
{
    smoothLen -= 0.01;
}

void GLWidget::increasePillowRecursion()
{
    std::cerr << "recursionLevel++";

    recursionLevel++;
    if (recursionLevel >= 16)
        recursionLevel = 15;
}

void GLWidget::decreasePillowRecursion()
{
    std::cerr << "recursionLevel--";

    recursionLevel--;
    if (recursionLevel <= 1)
        recursionLevel = 1;
}

void GLWidget::increaseQuadRecursion()
{
    std::cerr << "recursionLevel++";

    quadLevel++;
    if (quadLevel >= 16)
        quadLevel = 15;
}

void GLWidget::decreaseQuadRecursion()
{
    std::cerr << "recursionLevel--";

    quadLevel--;
    if (quadLevel <= 1)
        quadLevel = 1;
}

void GLWidget::movePointTo(int x, int y)
{
    pillowModel.points[selectedPointX][selectedPointY].x = x;
    pillowModel.points[selectedPointX][selectedPointY].y = height - y;
}

void GLWidget::movePointUp()
{
    pillowModel.points[selectedPointX][selectedPointY].y += step;
}

void GLWidget::movePointDown()
{
    pillowModel.points[selectedPointX][selectedPointY].y -= step;
}

void GLWidget::movePointLeft()
{
    pillowModel.points[selectedPointX][selectedPointY].x -= step;
}

void GLWidget::movePointRight()
{
    pillowModel.points[selectedPointX][selectedPointY].x += step;
}

void GLWidget::adjustR(bool way)
{
    if (way)
        pillowModel.r += 0.01;
    else
        pillowModel.r -= 0.01;
}

void GLWidget::adjustG(bool way)
{
    if (way)
        pillowModel.g += 0.01;
    else
        pillowModel.g -= 0.01;
}

void GLWidget::adjustB(bool way)
{
    if (way)
        pillowModel.b += 0.01;
    else
        pillowModel.b -= 0.01;
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
    configuration["General/virtualDesktop"] = virtualDesktop?"true":"false";

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
        selectedPointX = Center;
        selectedPointY = Down;
        break;
    }
    case 3:
    {
        selectedPointX = Right;
        selectedPointY = Down;
        break;
    }
    case 4:
    {
        selectedPointX = Left;
        selectedPointY = Middle;
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
        selectedPointX = Right;
        selectedPointY = Middle;
        break;
    }
    case 7:
    {
        selectedPointX = Left;
        selectedPointY = Up;
        break;
    }
    case 8:
    {
        selectedPointX = Center;
        selectedPointY = Up;
        break;
    }
    case 9:
    {
        selectedPointX = Right;
        selectedPointY = Up;
        break;
    }
    }
}

