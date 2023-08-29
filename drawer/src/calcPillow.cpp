#include "glwidget.h"

void GLWidget::calcPillow(PillowGraphy p, int sub, GLuint id, float level, int show_border, int active)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    glNewList(glListIndexPicture, GL_COMPILE);
    calcPillowRec(p, sub, id, level, show_border, active);
    glEndList();
}
