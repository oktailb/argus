#include "glwidget.h"
#include "configuration.h"
#include "argusConfig.h"
#include <iostream>
#include <sstream>
#include <fstream>


#ifdef __linux__
input *GLWidget::getCapturer() const
{
    return capturer;
}
#endif

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

void GLWidget::enableDebugMode()
{
    loadHSVTexture();
    textureCurrent = textureHSV;
}

void GLWidget::enableCaptureMode()
{
    textureCurrent = textureCapture;
}

void GLWidget::increaseAlpha()
{
    pillowModel.alpha[selectedPointX][selectedPointY] += 0.01;
    edited = true;
}

void GLWidget::decreaseAlpha()
{
    pillowModel.alpha[selectedPointX][selectedPointY] -= 0.01;
    edited = true;
}

void GLWidget::increaseSmoothLen()
{
    smoothLen += 0.01;
    edited = true;
}

void GLWidget::decreaseSmoothLen()
{
    smoothLen -= 0.01;
    edited = true;
}

void GLWidget::increasePillowRecursion()
{
    std::cerr << "recursionLevel++";

    recursionLevel++;
    if (recursionLevel >= 16)
        recursionLevel = 15;
    edited = true;
}

void GLWidget::decreasePillowRecursion()
{
    std::cerr << "recursionLevel--";

    recursionLevel--;
    if (recursionLevel <= 1)
        recursionLevel = 1;
    edited = true;
}

void GLWidget::increaseQuadRecursion()
{
    std::cerr << "recursionLevel++";

    quadLevel++;
    if (quadLevel >= 16)
        quadLevel = 15;
    edited = true;
}

void GLWidget::decreaseQuadRecursion()
{
    std::cerr << "recursionLevel--";

    quadLevel--;
    if (quadLevel <= 1)
        quadLevel = 1;
    edited = true;
}

void GLWidget::movePointTo(int x, int y)
{
    pillowModel.points[selectedPointX][selectedPointY].x = x;
    pillowModel.points[selectedPointX][selectedPointY].y = height - y;
    edited = true;
}

void GLWidget::movePointUp()
{
    pillowModel.points[selectedPointX][selectedPointY].y += step;
    edited = true;
}

void GLWidget::movePointDown()
{
    pillowModel.points[selectedPointX][selectedPointY].y -= step;
    edited = true;
}

void GLWidget::movePointLeft()
{
    pillowModel.points[selectedPointX][selectedPointY].x -= step;
    edited = true;
}

void GLWidget::movePointRight()
{
    pillowModel.points[selectedPointX][selectedPointY].x += step;
    edited = true;
}

void GLWidget::adjustR(bool way)
{
    if (way)
        pillowModel.r += 0.01;
    else
        pillowModel.r -= 0.01;
    edited = true;
}

void GLWidget::adjustG(bool way)
{
    if (way)
        pillowModel.g += 0.01;
    else
        pillowModel.g -= 0.01;
    edited = true;
}

void GLWidget::adjustB(bool way)
{
    if (way)
        pillowModel.b += 0.01;
    else
        pillowModel.b -= 0.01;
    edited = true;
}

void GLWidget::save(std::string conf)
{
    argus::ArgusConfig c;
    c.general.child = child;
    c.general.prefix = prefix;
    c.general.title = title;
    c.general.pillowRec = recursionLevel;
    c.general.quadRec = quadLevel;
    c.general.smoothLen = static_cast<float>(pillowModel.SmoothLen);
    c.general.fps = fps;
    c.general.videoSync = videoSync;
    c.general.stats = stats;
#ifdef WIN32
    c.general.virtualDesktop = virtualDesktop;
#endif
    c.geometry.Nx  = pillowModel.points[Down]  [Center].x;
    c.geometry.Ny  = pillowModel.points[Down]  [Center].y;
    c.geometry.Sx  = pillowModel.points[Up]    [Center].x;
    c.geometry.Sy  = pillowModel.points[Up]    [Center].y;
    c.geometry.Wx  = pillowModel.points[Middle][Left].x;
    c.geometry.Wy  = pillowModel.points[Middle][Left].y;
    c.geometry.Ex  = pillowModel.points[Middle][Right].x;
    c.geometry.Ey  = pillowModel.points[Middle][Right].y;
    c.geometry.SWx = pillowModel.points[Up]    [Left].x;
    c.geometry.SWy = pillowModel.points[Up]    [Left].y;
    c.geometry.NWx = pillowModel.points[Down]  [Left].x;
    c.geometry.NWy = pillowModel.points[Down]  [Left].y;
    c.geometry.SEx = pillowModel.points[Up]    [Right].x;
    c.geometry.SEy = pillowModel.points[Up]    [Right].y;
    c.geometry.NEx = pillowModel.points[Down]  [Right].x;
    c.geometry.NEy = pillowModel.points[Down]  [Right].y;
    c.geometry.Cx  = pillowModel.points[Middle][Center].x;
    c.geometry.Cy  = pillowModel.points[Middle][Center].y;
    c.blending.Na  = pillowModel.alpha[Down]  [Center];
    c.blending.Sa  = pillowModel.alpha[Up]    [Center];
    c.blending.Ea  = pillowModel.alpha[Middle][Right];
    c.blending.Wa  = pillowModel.alpha[Middle][Left];
    c.blending.NEa = pillowModel.alpha[Down] [Right];
    c.blending.SEa = pillowModel.alpha[Up]   [Right];
    c.blending.NWa = pillowModel.alpha[Down] [Left];
    c.blending.SWa = pillowModel.alpha[Up]   [Left];
    c.blending.Ca  = pillowModel.alpha[Middle][Center];
    c.color.r = pillowModel.r;
    c.color.g = pillowModel.g;
    c.color.b = pillowModel.b;
    c.cropping.x = cropX;
    c.cropping.y = cropY;
    c.cropping.width = cropWidth;
    c.cropping.height = cropHeight;
    argus::saveConfig(filename, c);
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
}

