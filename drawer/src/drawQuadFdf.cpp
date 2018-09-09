#include "glwidget.h"

void GLWidget::drawQuadFdfFast()
{
    glCallList(glListIndexGrid);
}

void GLWidget::drawQuadFdf(int level, int x, int y, int w, int h)
{
    glPushAttrib(GL_CURRENT_BIT);
    double stepVertexCoordx = (double)w / (double)level;
    double stepVertexCoordy = (double)h / (double)level;
    glBegin(GL_LINES);
    glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
    for (int i = 0 ; i < level; i++)
    {
        double minVertexX = (double)i         * stepVertexCoordx / (double)w;
        double maxVertexX = ((double)i + 1.0f) * stepVertexCoordx / (double)w;

        for (int j = 0 ; j < level; j++)
        {
            double minVertexY = (double)j         * stepVertexCoordy / (double)h;
            double maxVertexY = ((double)j + 1.0f) * stepVertexCoordy / (double)h;

            glVertex3d(x + minVertexX, y + minVertexY, 0);
            glVertex3d(x + maxVertexX, y + minVertexY, 0);

            glVertex3d(x + maxVertexX, y + minVertexY, 0);
            glVertex3d(x + maxVertexX, y + maxVertexY, 0);

            glVertex3d(x + maxVertexX, y + maxVertexY, 0);
            glVertex3d(x + minVertexX, y + maxVertexY, 0);

            glVertex3d(x + minVertexX, y + maxVertexY, 0);
            glVertex3d(x + minVertexX, y + minVertexY, 0);
        }
    }
    glEnd();
    glPopAttrib();
}
