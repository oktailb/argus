#include "glwidget.h"

void GLWidget::drawEditMode(PillowGraphy p, int sub, GLuint id, float level)
{
    t_Point                     mouseCoord;

    mouseCoord.x = width * lastPos.x / windowWidth;
    mouseCoord.y = height - height * lastPos.y / windowHeight;

    int find = 0;
    for (int pointx = 0 ; pointx < 3 ; pointx++)
    {
        for (int pointy = 0 ; pointy < 3 ; pointy++)
        {
            if (DIST(p.points[pointx][pointy], mouseCoord) < crossSize)
            {
                drawCross(p.points[pointx][pointy].x, p.points[pointx][pointy].y, crossSize, TextureID, Zlevel + level, true);
                find ++;
                selectedPointX = pointx;
                selectedPointY = pointy;

                break;
            }
            else
                drawCross(p.points[pointx][pointy].x, p.points[pointx][pointy].y, crossSize, TextureID, Zlevel + level, false);
        }
    }
    if (find == 0)
    {
        for (int pointx = 0 ; pointx < 3 ; pointx++)
        {
            for (int pointy = 0 ; pointy < 3 ; pointy++)
            {
                drawCross(p.points[pointx][pointy].x, p.points[pointx][pointy].y, crossSize, TextureID, Zlevel, false);
            }
        }
    }
}
