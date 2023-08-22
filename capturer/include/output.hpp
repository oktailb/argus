#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/freeglut.h>
#include <GL/GL.h>
#include <iostream>
//#include <opencv/cv.h>

#define TOP			0.999f
#define BOTTOM		0.001f
#define RIGHT		0.999f
#define LEFT		0.001f
#define MIDDLE		0.5f
#define RGBA(r,g,b,a) (r),(g),(b),(a)

enum textquality {solid, shaded, blended};

class Argus
{
public:
	Argus(int width, int height);
	~Argus();
	void process();
private:
	GLfloat Nx;
	GLfloat Sx;
	GLfloat Ex;
	GLfloat Wx;
	GLfloat NEx;
	GLfloat SEx;
	GLfloat NWx;
	GLfloat SWx;
	GLfloat Cx;
	GLfloat Ny;
	GLfloat Sy;
	GLfloat Ey;
	GLfloat Wy;
	GLfloat NEy;
	GLfloat SEy;
	GLfloat NWy;
	GLfloat SWy;
	GLfloat Cy;
	GLfloat Na;
	GLfloat Sa;
	GLfloat Ea;
	GLfloat Wa;
	GLfloat NEa;
	GLfloat SEa;
	GLfloat NWa;
	GLfloat SWa;
	GLfloat Ca;
	double Smooth;
	double SmoothLen;
	int PillowRec;
	int QuadRec;
};


typedef struct	s_Point
{
	GLfloat				x;
	GLfloat				y;
}								t_Point;

typedef struct		s_t_Quad
{
	t_Point	TargetUpLeft;
	t_Point	TargetUpRight;
	t_Point	TargetDownLeft;
	t_Point	TargetDownRight;

	t_Point	TargetUpLeftTP;
	t_Point	TargetUpRightTP;
	t_Point	TargetDownLeftTP;
	t_Point	TargetDownRightTP;

	GLfloat						TargetUpLeftAlpha;
	GLfloat						TargetUpRightAlpha;
	GLfloat						TargetDownLeftAlpha;
	GLfloat						TargetDownRightAlpha;
	GLfloat						Smooth;
	GLfloat						SmoothLen;
}								t_Quad;

typedef struct		s_PillowGraphy
{
	t_Quad			NE;
	t_Quad			NW;
	t_Quad			SW;
	t_Quad			SE;
	//CvMat			Homography;
}									PillowGraphy;

t_Point	calc_linear(t_Point A, t_Point B, t_Point C);
void		drawQuadRec(t_Quad h, int sub, GLuint id, float level, bool show_border = false, bool active = true);
void		drawPillow(PillowGraphy p, int sub, GLuint id, float level, bool show_border, bool active);
unsigned char	* mirrorv (unsigned char *src, int w, int h, int bpp);
//TTF_Font* loadfont(char* file, int ptsize);
//GLuint			drawtext(	TTF_Font *fonttodraw, 
//											char fgR, char fgG, char fgB, char fgA, 
//											char bgR, char bgG, char bgB, char bgA,
//											char text[], textquality quality );