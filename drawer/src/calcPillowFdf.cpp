#include "glwidget.h"

void GLWidget::calcPillowFdf(PillowGraphy p, int sub, GLuint id, float level)
{
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteLists(glListIndexGrid, 1);

    glNewList(glListIndexGrid, GL_COMPILE);
        calcPillowFdfRec(p, sub, id, level);
    glEndList();
}
