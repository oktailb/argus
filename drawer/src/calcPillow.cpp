#include "glwidget.h"

void GLWidget::calcPillow(PillowGraphy p, int sub, GLuint id, float level, int show_border, int active)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    glDeleteLists(glListIndexPicture, 1);

    glNewList(glListIndexPicture, GL_COMPILE);
    calcPillowRec(p, sub, id, level, show_border, active);
    glEndList();
}
