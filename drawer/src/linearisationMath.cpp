#include "glwidget.h"

t_Point	calc_linear(t_Point P1, t_Point P2, t_Point P3)
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

float	calc_Alinear(float A, float B, float C)
{
    float G;

    G	= (A + B + C)/3;

    return (G);
}

t_Quad pillow2Quad(int x, int y, PillowMesh p)
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
