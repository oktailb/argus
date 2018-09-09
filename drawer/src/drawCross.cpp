#include "glwidget.h"

void GLWidget::drawCross(double x, double y, double size, GLuint id, float level, bool active)
{
    glBindTexture  (GL_TEXTURE_2D, 0);
    if (active)
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    else
        glColor4f(0.7f, 0.7f, 0.7f, 1.0f);

    glBegin( GL_LINE_LOOP );
    glVertex3d  (x - size, y - size, level - 0.02f);
    glVertex3d  (x + size, y - size, level - 0.02f);

    glVertex3d  (x + size, y - size, level - 0.02f);
    glVertex3d  (x + size, y + size, level - 0.02f);

    glVertex3d  (x + size, y + size, level - 0.02f);
    glVertex3d  (x - size, y + size, level - 0.02f);

    glVertex3d  (x - size, y + size, level - 0.02f);
    glVertex3d  (x - size, y - size, level - 0.02f);

    glVertex3d  (x - size, y - size, level - 0.02f);
    glVertex3d  (x + size, y + size, level - 0.02f);

    glVertex3d  (x + size, y - size, level - 0.02f);
    glVertex3d  (x - size, y + size, level - 0.02f);
    glEnd();
    glBindTexture  (GL_TEXTURE_2D, id);
}
