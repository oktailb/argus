#include "glwidget.h"


void		GLWidget::drawQuadFdf(t_Quad h, int sub, GLuint id, float level, int active)
{
    glCallList(glListIndexGrid);
}

void		GLWidget::calcQuadFdf(t_Quad h, int sub, GLuint id, float level, int active)
{
    calcQuadFdfRec(h, sub, id, level, active);
}

void		GLWidget::calcQuadFdfRec(t_Quad h, int sub, GLuint id, float level, int active)
{
    if (sub == 1)
    {
        glColor4f(0.0f, 0.8f, 0.0f, 0.0f);
        glVertex3d      (h.points[UpLeft].x       , h.points[UpLeft].y, level);
        glColor4f(0.0f, 0.8f, 0.0f, 0.0f);
        glVertex3d      (h.points[DownLeft].x     , h.points[DownLeft].y, level);
        glColor4f(0.0f, 0.8f, 0.0f, 0.0f);
        glVertex3d      (h.points[DownRight].x    , h.points[DownRight].y, level);
        glColor4f(0.0f, 0.8f, 0.0f, 0.0f);
        glVertex3d      (h.points[UpRight].x      , h.points[UpRight].y, level);
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

        calcQuadFdfRec(h1, sub - 1, id, level, active);
        calcQuadFdfRec(h2, sub - 1, id, level, active);
        calcQuadFdfRec(h3, sub - 1, id, level, active);
        calcQuadFdfRec(h4, sub - 1, id, level, active);
    }
}

void		GLWidget::calcQuadRec(t_Quad h, int sub, GLuint id, float level, int show_border, int active)
{
    glBindTexture  (GL_TEXTURE_2D, id);
    if (sub == 1)
    {
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

inline t_Point	calc_linear(t_Point P1, t_Point P2, t_Point P3)
{
    t_Point A1;
    t_Point A2;

    A1 = P1/2.0f + P2*(sqrt(2.0f)/2.0f) + P3*(0.5f - (sqrt(2.0f)/2.0f));
    A2 = P3/2.0f + P2*(sqrt(2.0f)/2.0f) + P1*(0.5f - (sqrt(2.0f)/2.0f));

    t_Point res;
    res = A1/2.0f + P2*(sqrt(2.0f)/2.0f) + A2*(0.5f - (sqrt(2.0f)/2.0f));

    GLfloat factor = M_PI;
    res = (res*factor + P1 + P2) / (factor + 2.0f);

    return res;
}

inline float	calc_Alinear(float A, float B, float C)
{
    float G;

    //centre de gravité de ABC
    G	= (A + B + C)/3;
    //centre de gravité de AGB
    //G	= (A + G + B)/3;
    //G	= (A + G + B)/3;
    return (G);
}

t_Quad pillow2Quad(int x, int y, PillowGraphy p)
{
    t_Quad res;
    res.points [0] = p.points [0+x][0+y];
    res.texture[0] = p.texture[0+x][0+y];
    res.alpha  [0] = p.alpha  [0+x][0+y];

    res.points [1] = p.points [0+x][1+y];
    res.texture[1] = p.texture[0+x][1+y];
    res.alpha  [1] = p.alpha  [0+x][1+y];

    res.points [2] = p.points [1+x][1+y];
    res.texture[2] = p.texture[1+x][1+y];
    res.alpha  [2] = p.alpha  [1+x][1+y];

    res.points [3] = p.points [1+x][0+y];
    res.texture[3] = p.texture[1+x][0+y];
    res.alpha  [3] = p.alpha  [1+x][0+y];

    res.r = p.r;
    res.g = p.g;
    res.b = p.b;

    return res;
}

void GLWidget::drawPillowFdf()
{
    glCallList(glListIndexGrid);
}

void GLWidget::drawPillow()
{
    glCallList(glListIndexPicture);
}

void GLWidget::calcPillowFdf(PillowGraphy p, int sub, GLuint id, float level, int show_border, int active)
{
    glBindTexture(GL_TEXTURE_2D, 0);

    glEndList();

    glNewList(glListIndexGrid, GL_COMPILE);
        glBegin( GL_QUADS );
            calcPillowFdfRec(p, sub, id, level, show_border, active);
        glEnd();
    glEndList();
}

void GLWidget::calcPillowFdfRec(PillowGraphy p, int sub, GLuint id, float level, int show_border, int active)
{
    if (sub == 1)
    {
        calcQuadFdfRec(pillow2Quad(0, 0, p), quadLevel, id, level, active);
        calcQuadFdfRec(pillow2Quad(0, 1, p), quadLevel, id, level, active);
        calcQuadFdfRec(pillow2Quad(1, 0, p), quadLevel, id, level, active);
        calcQuadFdfRec(pillow2Quad(1, 1, p), quadLevel, id, level, active);
    }
    else
    {
        // linearisation !
        t_Point	    linear[25];

        // les bords
        linear[BUpLeft]     = calc_linear(p.points[Up]    [Left]    , p.points[Up]    [Center]   , p.points[Up]  [Right]);
        linear[BLeftUp]     = calc_linear(p.points[Up]    [Left]    , p.points[Middle][Left]     , p.points[Down][Left] );
        linear[BDownLeft]   = calc_linear(p.points[Down]  [Left]    , p.points[Middle][Left]     , p.points[Up]  [Left] );
        linear[BLeftDown]   = calc_linear(p.points[Down]  [Left]    , p.points[Down]  [Center]   , p.points[Down][Right]);
        linear[BDownRight]  = calc_linear(p.points[Down]  [Right]   , p.points[Down]  [Center]   , p.points[Down][Left]);
        linear[BRightDown]  = calc_linear(p.points[Down]  [Right]   , p.points[Middle][Right]    , p.points[Up]  [Right]);
        linear[BRightUp]    = calc_linear(p.points[Up]    [Right]   , p.points[Middle][Right]    , p.points[Down][Right]);
        linear[BUpRight]    = calc_linear(p.points[Up]    [Right]   , p.points[Up]    [Center]   , p.points[Up]  [Left]);

        // les joints
        linear[JoinUp]      = calc_linear(p.points[Up]    [Center]  , p.points[Middle][Center]   , p.points[Down]  [Center]);
        linear[JoinDown]    = calc_linear(p.points[Down]  [Center]  , p.points[Middle][Center]   , p.points[Up]    [Center]);
        linear[JoinLeft]    = calc_linear(p.points[Middle][Left]    , p.points[Middle][Center]   , p.points[Middle][Right]);
        linear[JoinRight]   = calc_linear(p.points[Middle][Right]   , p.points[Middle][Center]   , p.points[Middle][Left]);

        // les centres
        linear[CenterUpLeft]    = calc_linear(linear[BLeftUp]   , linear[JoinUp]    , linear[BRightUp]  );
        linear[CenterDownLeft]  = calc_linear(linear[BDownLeft] , linear[JoinDown]  , linear[BRightDown]);
        linear[CenterDownRight] = calc_linear(linear[BDownRight], linear[JoinRight] , linear[BUpRight]  );
        linear[CenterUpRight]   = calc_linear(linear[BUpRight]  , linear[JoinRight] , linear[BDownRight]);

        linear[OriginUp       ] = p.points[Up][Center];
        linear[OriginLeft     ] = p.points[Middle][Left];
        linear[OriginDown     ] = p.points[Down][Center];
        linear[OriginRight    ] = p.points[Middle][Right];
        linear[OriginCenter   ] = p.points[Middle][Center];
        linear[OriginUpLeft   ] = p.points[Up][Left];
        linear[OriginDownLeft ] = p.points[Down][Left];
        linear[OriginDownRight] = p.points[Down][Right];
        linear[OriginUpRight  ] = p.points[Up][Right];

        PillowGraphy p1 =
            {
                {
                 {linear[OriginUpLeft], linear[BUpLeft]     , linear[OriginUp]    },
                 {linear[BLeftUp]     , linear[CenterUpLeft], linear[JoinUp]      },
                 {linear[OriginLeft]  , linear[JoinLeft]    , linear[OriginCenter]},
                 },
                {
                 {0, 0, 0},
                 {0, 0, 0},
                 {0, 0, 0},
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                },
                p.r, p.g, p.b,
                0.0, 0.0
            };

        PillowGraphy p2 =
            {
                {
                 {linear[OriginLeft]    , linear[JoinLeft]      , linear[OriginCenter]},
                 {linear[BDownLeft]     , linear[CenterDownLeft], linear[JoinDown]    },
                 {linear[OriginDownLeft], linear[BLeftDown]     , linear[OriginDown]  },
                 },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                 },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                 },
                p.r, p.g, p.b,
                0.0, 0.0
            };

        PillowGraphy p3 =
            {
                {
                 {linear[OriginCenter], linear[JoinRight]      , linear[OriginRight]    },
                 {linear[JoinDown]    , linear[CenterDownRight], linear[BRightDown]     },
                 {linear[OriginDown]  , linear[BDownRight]     , linear[OriginDownRight]},
                 },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                 },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                 },
                p.r, p.g, p.b,
                0.0, 0.0
            };

        PillowGraphy p4 =
            {
                {
                 {linear[OriginUp]    , linear[BUpRight]     , linear[OriginUpRight]},
                 {linear[JoinUp]      , linear[CenterUpRight], linear[BRightUp]     },
                 {linear[OriginCenter], linear[JoinRight]    , linear[OriginRight]  },
                 },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                 },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                 },
                p.r, p.g, p.b,
                0.0, 0.0
            };

        calcPillowFdfRec(p1, sub - 1, id, level, show_border, active);
        calcPillowFdfRec(p2, sub - 1, id, level, show_border, active);
        calcPillowFdfRec(p3, sub - 1, id, level, show_border, active);
        calcPillowFdfRec(p4, sub - 1, id, level, show_border, active);
    }
}


void GLWidget::calcPillow(PillowGraphy p, int sub, GLuint id, float level, int show_border, int active)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    glNewList(glListIndexPicture, GL_COMPILE);
        glBegin( GL_LINE_LOOP );
            calcPillowRec(p, sub, id, level, show_border, active);
        glEnd();
    glEndList();
}

void GLWidget::calcPillowRec(PillowGraphy p, int sub, GLuint id, float level, int show_border, int active)
{
    if (sub == 1)
    {
        calcQuadRec(pillow2Quad(0, 0, p), quadLevel, id, level, show_border, active);
        calcQuadRec(pillow2Quad(0, 1, p), quadLevel, id, level, show_border, active);
        calcQuadRec(pillow2Quad(1, 0, p), quadLevel, id, level, show_border, active);
        calcQuadRec(pillow2Quad(1, 1, p), quadLevel, id, level, show_border, active);
    }
    else
    {
        // linearisation !
        t_Point	    linear[25];
        t_Point	    linearTP[25];
        GLfloat		linearA[25];

        // les bords
        linear[BUpLeft]     = calc_linear(p.points[Up]    [Left]    , p.points[Up]    [Center]   , p.points[Up]  [Right]);
        linear[BLeftUp]     = calc_linear(p.points[Up]    [Left]    , p.points[Middle][Left]     , p.points[Down][Left] );
        linear[BDownLeft]   = calc_linear(p.points[Down]  [Left]    , p.points[Middle][Left]     , p.points[Up]  [Left] );
        linear[BLeftDown]   = calc_linear(p.points[Down]  [Left]    , p.points[Down]  [Center]   , p.points[Down][Right]);
        linear[BDownRight]  = calc_linear(p.points[Down]  [Right]   , p.points[Down]  [Center]   , p.points[Down][Left]);
        linear[BRightDown]  = calc_linear(p.points[Down]  [Right]   , p.points[Middle][Right]    , p.points[Up]  [Right]);
        linear[BRightUp]    = calc_linear(p.points[Up]    [Right]   , p.points[Middle][Right]    , p.points[Down][Right]);
        linear[BUpRight]    = calc_linear(p.points[Up]    [Right]   , p.points[Up]    [Center]   , p.points[Up]  [Left]);

        // les joints
        linear[JoinUp]      = calc_linear(p.points[Up]    [Center]  , p.points[Middle][Center]   , p.points[Down]  [Center]);
        linear[JoinDown]    = calc_linear(p.points[Down]  [Center]  , p.points[Middle][Center]   , p.points[Up]    [Center]);
        linear[JoinLeft]    = calc_linear(p.points[Middle][Left]    , p.points[Middle][Center]   , p.points[Middle][Right]);
        linear[JoinRight]   = calc_linear(p.points[Middle][Right]   , p.points[Middle][Center]   , p.points[Middle][Left]);

        // les centres
        linear[CenterUpLeft]    = calc_linear(linear[BLeftUp]   , linear[JoinUp]    , linear[BRightUp]  );
        linear[CenterDownLeft]  = calc_linear(linear[BDownLeft] , linear[JoinDown]  , linear[BRightDown]);
        linear[CenterDownRight] = calc_linear(linear[BDownRight], linear[JoinRight] , linear[BUpRight]  );
        linear[CenterUpRight]   = calc_linear(linear[BUpRight]  , linear[JoinRight] , linear[BDownRight]);

        linear[OriginUp       ] = p.points[Up][Center];
        linear[OriginLeft     ] = p.points[Middle][Left];
        linear[OriginDown     ] = p.points[Down][Center];
        linear[OriginRight    ] = p.points[Middle][Right];
        linear[OriginCenter   ] = p.points[Middle][Center];
        linear[OriginUpLeft   ] = p.points[Up][Left];
        linear[OriginDownLeft ] = p.points[Down][Left];
        linear[OriginDownRight] = p.points[Down][Right];
        linear[OriginUpRight  ] = p.points[Up][Right];

        // la texture => cartesien
        // les bords
        linearTP[BUpLeft]   = (p.texture[Up]    [Left]    + p.texture[Up]    [Center]) / 2.0;
        linearTP[BLeftUp]   = (p.texture[Up]    [Left]    + p.texture[Middle][Left]  ) / 2.0;
        linearTP[BDownLeft] = (p.texture[Middle][Left]    + p.texture[Down]  [Left]  ) / 2.0;
        linearTP[BLeftDown] = (p.texture[Down]  [Left]    + p.texture[Down]  [Center]) / 2.0;
        linearTP[BDownRight]= (p.texture[Down]  [Center]  + p.texture[Down]  [Right] ) / 2.0;
        linearTP[BRightDown]= (p.texture[Down]  [Right]   + p.texture[Middle][Right] ) / 2.0;
        linearTP[BRightUp]  = (p.texture[Middle][Right]   + p.texture[Up]    [Right] ) / 2.0;
        linearTP[BUpRight]  = (p.texture[Up]    [Center]  + p.texture[Up]    [Right] ) / 2.0;

        // les joints
        linearTP[JoinUp]    = (p.texture[Up]    [Center]  + p.texture[Middle][Center]) / 2.0;
        linearTP[JoinLeft]  = (p.texture[Middle][Left]    + p.texture[Middle][Center]) / 2.0;
        linearTP[JoinDown]  = (p.texture[Middle][Center]  + p.texture[Down]  [Center]) / 2.0;
        linearTP[JoinRight] = (p.texture[Middle][Center]  + p.texture[Middle][Right] ) / 2.0;

        // les centres
        linearTP[CenterUpLeft]      = (linearTP[BLeftUp]   + linearTP[JoinUp]   ) / 2.0;
        linearTP[CenterDownLeft]    = (linearTP[BDownLeft] + linearTP[JoinDown] ) / 2.0;
        linearTP[CenterDownRight]   = (linearTP[BDownRight]+ linearTP[JoinRight]) / 2.0;
        linearTP[CenterUpRight]     = (linearTP[BUpRight]  + linearTP[JoinRight]) / 2.0;

        linearTP[OriginUp       ] = p.texture[Up][Center];
        linearTP[OriginLeft     ] = p.texture[Middle][Left];
        linearTP[OriginDown     ] = p.texture[Down][Center];
        linearTP[OriginRight    ] = p.texture[Middle][Right];
        linearTP[OriginCenter   ] = p.texture[Middle][Center];
        linearTP[OriginUpLeft   ] = p.texture[Up][Left];
        linearTP[OriginDownLeft ] = p.texture[Down][Left];
        linearTP[OriginDownRight] = p.texture[Down][Right];
        linearTP[OriginUpRight  ] = p.texture[Up][Right];

        // re dans les alphas ...
        // les bords
        linearA[BUpLeft]   = (ramp * p.alpha[Up]    [Left]    + aramp * p.alpha[Up]    [Center]);
        linearA[BLeftUp]   = (ramp * p.alpha[Up]    [Left]    + aramp * p.alpha[Middle][Left]  );
        linearA[BDownLeft] = (ramp * p.alpha[Middle][Left]    + aramp * p.alpha[Down]  [Left]  );
        linearA[BLeftDown] = (ramp * p.alpha[Down]  [Left]    + aramp * p.alpha[Down]  [Center]);
        linearA[BDownRight]= (ramp * p.alpha[Down]  [Center]  + aramp * p.alpha[Down]  [Right] );
        linearA[BRightDown]= (ramp * p.alpha[Down]  [Right]   + aramp * p.alpha[Middle][Right] );
        linearA[BRightUp]  = (ramp * p.alpha[Middle][Right]   + aramp * p.alpha[Up]    [Right] );
        linearA[BUpRight]  = (ramp * p.alpha[Up]    [Center]  + aramp * p.alpha[Up]    [Right] );

        // les joints
        linearA[JoinUp]    = (ramp * p.alpha[Up]    [Center]  + aramp * p.alpha[Middle][Center]);
        linearA[JoinLeft]  = (ramp * p.alpha[Middle][Left]    + aramp * p.alpha[Middle][Center]);
        linearA[JoinDown]  = (ramp * p.alpha[Middle][Center]  + aramp * p.alpha[Down]  [Center]);
        linearA[JoinRight] = (ramp * p.alpha[Middle][Center]  + aramp * p.alpha[Middle][Right] );

        // les centres
        linearA[CenterUpLeft]      = (ramp * linearA[BLeftUp]   + aramp * linearA[JoinUp]   );
        linearA[CenterDownLeft]    = (ramp * linearA[BDownLeft] + aramp * linearA[JoinDown] );
        linearA[CenterDownRight]   = (ramp * linearA[BDownRight]+ aramp * linearA[JoinRight]);
        linearA[CenterUpRight]     = (ramp * linearA[BUpRight]  + aramp * linearA[JoinRight]);

        linearA[OriginUp       ] = p.alpha[Up][Center];
        linearA[OriginLeft     ] = p.alpha[Middle][Left];
        linearA[OriginDown     ] = p.alpha[Down][Center];
        linearA[OriginRight    ] = p.alpha[Middle][Right];
        linearA[OriginCenter   ] = p.alpha[Middle][Center];
        linearA[OriginUpLeft   ] = p.alpha[Up][Left];
        linearA[OriginDownLeft ] = p.alpha[Down][Left];
        linearA[OriginDownRight] = p.alpha[Down][Right];
        linearA[OriginUpRight  ] = p.alpha[Up][Right];

        PillowGraphy p1 =
            {
                {
                 {linear[OriginUpLeft], linear[BUpLeft]     , linear[OriginUp]    },
                 {linear[BLeftUp]     , linear[CenterUpLeft], linear[JoinUp]      },
                 {linear[OriginLeft]  , linear[JoinLeft]    , linear[OriginCenter]},
                 },
                {
                 {linearTP[OriginUpLeft], linearTP[BUpLeft]     , linearTP[OriginUp]    },
                 {linearTP[BLeftUp]     , linearTP[CenterUpLeft], linearTP[JoinUp]      },
                 {linearTP[OriginLeft]  , linearTP[JoinLeft]    , linearTP[OriginCenter]},
                 },
                {
                 {linearA[OriginUpLeft], linearA[BUpLeft]     , linearA[OriginUp]    },
                 {linearA[BLeftUp]     , linearA[CenterUpLeft], linearA[JoinUp]      },
                 {linearA[OriginLeft]  , linearA[JoinLeft]    , linearA[OriginCenter]},
                 },
                p.r, p.g, p.b,
                0.0, 0.0
            };

        PillowGraphy p2 =
            {
                {
                 {linear[OriginLeft]    , linear[JoinLeft]      , linear[OriginCenter]},
                 {linear[BDownLeft]     , linear[CenterDownLeft], linear[JoinDown]    },
                 {linear[OriginDownLeft], linear[BLeftDown]     , linear[OriginDown]  },
                 },
                {
                 {linearTP[OriginLeft]    , linearTP[JoinLeft]      , linearTP[OriginCenter]},
                 {linearTP[BDownLeft]     , linearTP[CenterDownLeft], linearTP[JoinDown]    },
                 {linearTP[OriginDownLeft], linearTP[BLeftDown]     , linearTP[OriginDown]  },
                 },
                {
                 {linearA[OriginLeft]    , linearA[JoinLeft]      , linearA[OriginCenter]},
                 {linearA[BDownLeft]     , linearA[CenterDownLeft], linearA[JoinDown]    },
                 {linearA[OriginDownLeft], linearA[BLeftDown]     , linearA[OriginDown]  },
                 },
                p.r, p.g, p.b,
                0.0, 0.0
            };

        PillowGraphy p3 =
            {
                {
                 {linear[OriginCenter], linear[JoinRight]      , linear[OriginRight]    },
                 {linear[JoinDown]    , linear[CenterDownRight], linear[BRightDown]     },
                 {linear[OriginDown]  , linear[BDownRight]     , linear[OriginDownRight]},
                 },
                {
                 {linearTP[OriginCenter], linearTP[JoinRight]      , linearTP[OriginRight]    },
                 {linearTP[JoinDown]    , linearTP[CenterDownRight], linearTP[BRightDown]     },
                 {linearTP[OriginDown]  , linearTP[BDownRight]     , linearTP[OriginDownRight]},
                 },
                {
                 {linearA[OriginCenter], linearA[JoinRight]      , linearA[OriginRight]    },
                 {linearA[JoinDown]    , linearA[CenterDownRight], linearA[BRightDown]     },
                 {linearA[OriginDown]  , linearA[BDownRight]     , linearA[OriginDownRight]},
                 },
                p.r, p.g, p.b,
                0.0, 0.0
            };

        PillowGraphy p4 =
            {
                {
                 {linear[OriginUp]    , linear[BUpRight]     , linear[OriginUpRight]},
                 {linear[JoinUp]      , linear[CenterUpRight], linear[BRightUp]     },
                 {linear[OriginCenter], linear[JoinRight]    , linear[OriginRight]  },
                 },
                {
                 {linearTP[OriginUp]    , linearTP[BUpRight]     , linearTP[OriginUpRight]},
                 {linearTP[JoinUp]      , linearTP[CenterUpRight], linearTP[BRightUp]     },
                 {linearTP[OriginCenter], linearTP[JoinRight]    , linearTP[OriginRight]  },
                 },
                {
                 {linearA[OriginUp]    , linearA[BUpRight]     , linearA[OriginUpRight]},
                 {linearA[JoinUp]      , linearA[CenterUpRight], linearA[BRightUp]     },
                 {linearA[OriginCenter], linearA[JoinRight]    , linearA[OriginRight]  },
                 },
                p.r, p.g, p.b,
                0.0, 0.0
            };

        calcPillowRec(p1, sub - 1, id, level, show_border, active);
        calcPillowRec(p2, sub - 1, id, level, show_border, active);
        calcPillowRec(p3, sub - 1, id, level, show_border, active);
        calcPillowRec(p4, sub - 1, id, level, show_border, active);
    }
}
