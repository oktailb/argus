#include "glwidget.h"

void		GLWidget::calcQuadFdfRec(t_Quad h, int sub, GLuint id)
{
    if (sub == 1)
    {
        glBegin(GL_QUADS);
        glColor4f(0.0f, 0.8f, 0.0f, 0.0f);
        glVertex3d      (h.points[UpLeft].x       , h.points[UpLeft].y, Zlevel + 0.02);
        glColor4f(0.0f, 0.8f, 0.0f, 0.0f);
        glVertex3d      (h.points[DownLeft].x     , h.points[DownLeft].y, Zlevel + 0.02);
        glColor4f(0.0f, 0.8f, 0.0f, 0.0f);
        glVertex3d      (h.points[DownRight].x    , h.points[DownRight].y, Zlevel + 0.02);
        glColor4f(0.0f, 0.8f, 0.0f, 0.0f);
        glVertex3d      (h.points[UpRight].x      , h.points[UpRight].y, Zlevel + 0.02);
        glEnd();
    }
    else
    {
        t_Quad h1 = h;
        t_Quad h2 = h;
        t_Quad h3 = h;
        t_Quad h4 = h;

        //les quads
        h1.points[UpRight]		= (h.points[UpLeft]		+ h.points[UpRight]) / 2;
        h1.points[DownLeft]		= (h.points[UpLeft]		+ h.points[DownLeft]) / 2;
        h1.points[DownRight]    = (h.points[UpLeft]		+ h.points[DownRight]	+ h.points[DownLeft]		+ h.points[UpRight]) / 4;

        h2.points[UpLeft]         = (h.points[UpLeft]	+ h.points[UpRight])/2;
        h2.points[DownLeft]		= (h.points[DownLeft]   + h.points[UpRight]	+	h.points[UpLeft]			+ h.points[DownRight])/4;
        h2.points[DownRight]      = (h.points[DownRight]+ h.points[UpRight])/2;

        h3.points[UpLeft]         = (h.points[UpLeft]		+ h.points[DownLeft])/2;
        h3.points[UpRight]		= (h.points[UpLeft]		+ h.points[DownRight]	+	h.points[DownLeft]		+ h.points[UpRight])/4;
        h3.points[DownRight]      = (h.points[DownLeft]     + h.points[DownRight])/2;

        h4.points[UpLeft]         = (h.points[UpLeft]		+ h.points[DownRight]	+	h.points[DownLeft]		+ h.points[UpRight])/4;
        h4.points[UpRight]		= (h.points[UpRight]      + h.points[DownRight])/2;
        h4.points[DownLeft]		= (h.points[DownLeft]     + h.points[DownRight])/2;

        calcQuadFdfRec(h1, sub - 1, id);
        calcQuadFdfRec(h2, sub - 1, id);
        calcQuadFdfRec(h3, sub - 1, id);
        calcQuadFdfRec(h4, sub - 1, id);
    }
}
