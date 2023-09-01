#include "glwidget.h"

void GLWidget::calcPillow(PillowGraphy p, int sub, GLuint id, float level)
{
    glBindTexture(GL_TEXTURE_2D, textureCurrent);

    glDeleteLists(glListIndexPicture, 1);

    glNewList(glListIndexPicture, GL_COMPILE);
    calcPillowRec(p, sub, id, level);
    glEndList();
}
