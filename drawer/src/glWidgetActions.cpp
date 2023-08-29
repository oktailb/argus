#include "glwidget.h"
#include "configuration.h"
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
    calcPillow(pillowModel, recursionLevel, texture, Zlevel, true, true);
    calcPillowFdf(pillowModel, recursionLevel, 0, Zlevel + 1, true, true);
}

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
