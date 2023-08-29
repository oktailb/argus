#include "glwidget.h"

void		GLWidget::calcQuadRec(t_Quad h, int sub, GLuint id, float level, int show_border, int active)
{
    glBindTexture  (GL_TEXTURE_2D, id);
    if (sub == 1)
    {
        glBegin( GL_QUADS );
        glTexCoord2d    (h.texture[UpLeft].x      , h.texture[UpLeft].y );
        glColor4d       (h.r, h.g, h.b, h.alpha[UpLeft]);
        glVertex3d      (h.points[UpLeft].x       , h.points[UpLeft].y, level);
        glTexCoord2d    (h.texture[DownLeft].x    , h.texture[DownLeft].y );
        glColor4d       (h.r, h.g, h.b, h.alpha[DownLeft]);
        glVertex3d      (h.points[DownLeft].x     , h.points[DownLeft].y, level);
        glTexCoord2d    (h.texture[DownRight].x   , h.texture[DownRight].y );
        glColor4d       (h.r, h.g, h.b, h.alpha[DownRight]);
        glVertex3d      (h.points[DownRight].x    , h.points[DownRight].y, level);
        glTexCoord2d    (h.texture[UpRight].x     , h.texture[UpRight].y );
        glColor4d       (h.r, h.g, h.b, h.alpha[UpRight]);
        glVertex3d      (h.points[UpRight].x      , h.points[UpRight].y, level);
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

        //les reperes internes de la texture
        h1.texture[UpRight]     = (h.texture[UpLeft]	+ h.texture[UpRight])/2;
        h1.texture[DownLeft]	= (h.texture[UpLeft]	+ h.texture[DownLeft])/2;
        h1.texture[DownRight]	= (h.texture[UpLeft]	+ h.texture[DownRight]	+ h.texture[DownLeft]		+ h.texture[UpRight])/4;

        h2.texture[UpLeft]      = (h.texture[UpLeft]		+ h.texture[UpRight])/2;
        h2.texture[DownLeft]	= (h.texture[DownLeft]	+ h.texture[UpRight]	+	h.texture[UpLeft]		+ h.texture[DownRight])/4;
        h2.texture[DownRight]	= (h.texture[DownRight]	+ h.texture[UpRight])/2;

        h3.texture[UpLeft]      = (h.texture[UpLeft]	+ h.texture[DownLeft])/2;
        h3.texture[UpRight]     = (h.texture[UpLeft]	+ h.texture[DownRight]	+	h.texture[DownLeft]		+ h.texture[UpRight])/4;
        h3.texture[DownRight]	= (h.texture[DownLeft]	+ h.texture[DownRight])/2;

        h4.texture[UpLeft]      = (h.texture[UpLeft]	+ h.texture[DownRight]	+	h.texture[DownLeft]		+ h.texture[UpRight])/4;
        h4.texture[UpRight]     = (h.texture[UpRight]	+ h.texture[DownRight])/2;
        h4.texture[DownLeft]	= (h.texture[DownLeft]	+ h.texture[DownRight])/2;

        // les alphas
        h1.alpha[UpRight]	= (h.alpha[UpLeft]		+ h.alpha[UpRight])/2;
        h1.alpha[DownLeft]	= (h.alpha[UpLeft]		+ h.alpha[DownLeft])/2;
        h1.alpha[DownRight]	= (h.alpha[UpLeft]		+ h.alpha[DownRight]	+ h.alpha[DownLeft]		+ h.alpha[UpRight])/4;

        h2.alpha[UpLeft]	= (h.alpha[UpLeft]		+ h.alpha[UpRight])/2;
        h2.alpha[DownLeft]	= (h.alpha[DownLeft]	+ h.alpha[UpRight]	+	h.alpha[UpLeft]			+ h.alpha[DownRight])/4;
        h2.alpha[DownRight]	= (h.alpha[DownRight]	+ h.alpha[UpRight])/2;

        h3.alpha[UpLeft]	= (h.alpha[UpLeft]		+ h.alpha[DownLeft])/2;
        h3.alpha[UpRight]	= (h.alpha[UpLeft]		+ h.alpha[DownRight]	+	h.alpha[DownLeft]	+ h.alpha[UpRight])/4;
        h3.alpha[DownRight]	= (h.alpha[DownLeft]	+ h.alpha[DownRight])/2;

        h4.alpha[UpLeft]	= (h.alpha[UpLeft]		+ h.alpha[DownRight]	+	h.alpha[DownLeft]	+ h.alpha[UpRight])/4;
        h4.alpha[UpRight]	= (h.alpha[UpRight]		+ h.alpha[DownRight])/2;
        h4.alpha[DownLeft]	= (h.alpha[DownLeft]	+ h.alpha[DownRight])/2;


        calcQuadRec(h1, sub - 1, id, level, show_border, active);
        calcQuadRec(h2, sub - 1, id, level, show_border, active);
        calcQuadRec(h3, sub - 1, id, level, show_border, active);
        calcQuadRec(h4, sub - 1, id, level, show_border, active);
    }
}


