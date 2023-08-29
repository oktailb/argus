#include "glwidget.h"

void		GLWidget::calcQuadFdf(t_Quad h, int sub, GLuint id, float level, int active)
{
    calcQuadFdfRec(h, sub, id, level, active);
}
