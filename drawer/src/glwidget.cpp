#include "GL/glew.h"
#include "glwidget.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include "shm.h"
#include "resources.h"
#include "configuration.h"
#include "argusConfig.h"

GLWidget::GLWidget(std::string filename)
    : filename(filename)
{
    argus::ArgusConfig config;
    argus::ConfigLoadResult loadResult = argus::loadConfig(filename, config);
#ifdef WIN32
    virtualDesktop = config.general.virtualDesktop;
#elif __linux__
    capturer = new input(filename);
    width = capturer->getWidth();
    height = capturer->getHeight();
#endif
    videoSync = config.general.videoSync;
    stats = config.general.stats;
    fps = config.general.fps;

    prefix = config.general.prefix;
    child = config.general.child;
    title = config.general.title;

    std::string out0 = prefix + " Argus SharedMemory";

    selectedPointX = 1;
    selectedPointY = 1;
    step = 1;
    editMode = false;
    edited   = false;

    Zlevel = 1;
    recursionLevel = config.general.pillowRec;
    quadLevel = config.general.quadRec;
    t_Point N  = {config.geometry.Nx , config.geometry.Ny };
    t_Point S  = {config.geometry.Sx , config.geometry.Sy };
    t_Point E  = {config.geometry.Ex , config.geometry.Ey };
    t_Point W  = {config.geometry.Wx , config.geometry.Wy };
    t_Point SE = {config.geometry.SEx, config.geometry.SEy};
    t_Point NE = {config.geometry.NEx, config.geometry.NEy};
    t_Point NW = {config.geometry.NWx, config.geometry.NWy};
    t_Point SW = {config.geometry.SWx, config.geometry.SWy};
    t_Point C  = {config.geometry.Cx , config.geometry.Cy };
    GLfloat Na  = static_cast<GLfloat>(config.blending.Na);
    GLfloat Sa  = static_cast<GLfloat>(config.blending.Sa);
    GLfloat Ea  = static_cast<GLfloat>(config.blending.Ea);
    GLfloat Wa  = static_cast<GLfloat>(config.blending.Wa);
    GLfloat SEa = static_cast<GLfloat>(config.blending.SEa);
    GLfloat NEa = static_cast<GLfloat>(config.blending.NEa);
    GLfloat NWa = static_cast<GLfloat>(config.blending.NWa);
    GLfloat SWa = static_cast<GLfloat>(config.blending.SWa);
    GLfloat Ca  = static_cast<GLfloat>(config.blending.Ca);

    GLfloat r  = static_cast<GLfloat>(config.color.r);
    GLfloat g  = static_cast<GLfloat>(config.color.g);
    GLfloat b  = static_cast<GLfloat>(config.color.b);

    cropX       = config.cropping.x;
    cropY       = config.cropping.y;
    cropWidth   = config.cropping.width;
    cropHeight  = config.cropping.height;
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
    smoothLen = config.general.smoothLen;
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
    input::CaptureBuffer buf = capturer->getCaptureBuffer();
    bool haveData = false;
    if (buf.data && buf.width > 0 && buf.height > 0) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, buf.width, buf.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, buf.data);
        haveData = true;
    } else if (capturer->getXimg() && width > 0 && height > 0) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, capturer->getXimg()->data);
        haveData = true;
    }
    if (!haveData) {
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }
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
        if (edited)
        {
            calcPillow(pillowModel, recursionLevel, textureCapture, Zlevel);
            calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1);
        }
        drawPillow();
        drawPillowFdf();
        drawEditMode(pillowModel, recursionLevel, textureCurrent, Zlevel + 1);
    }
    else
        drawPillow();
}
