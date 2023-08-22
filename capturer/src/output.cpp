#include <output.hpp>

//TTF_Font* loadfont(char* file, int ptsize)
//{
//	TTF_Font* tmpfont;
//	tmpfont = TTF_OpenFont(file, ptsize);
//	if (tmpfont == NULL)
//	{
//		printf("Unable to load font: %s %s \n", file, TTF_GetError());
//	}
//	return tmpfont;
//}
//
//
//GLuint			drawtext(	TTF_Font *fonttodraw, 
//											char fgR, char fgG, char fgB, char fgA, 
//											char bgR, char bgG, char bgB, char bgA,
//											char text[], textquality quality )
//{
//	SDL_Color		tmpfontcolor = {fgR,fgG,fgB,fgA};
//	SDL_Color		tmpfontbgcolor = {bgR, bgG, bgB, bgA};
//	SDL_Surface*	resulting_text;
//
//	if (quality == solid) resulting_text = TTF_RenderText_Solid(fonttodraw, text, tmpfontcolor);
//	else if (quality == shaded) resulting_text = TTF_RenderText_Shaded(fonttodraw, text, tmpfontcolor, tmpfontbgcolor);
//	else if (quality == blended) resulting_text = TTF_RenderText_Blended(fonttodraw, text, tmpfontcolor);
//	GLuint glID = 0;
//	glGenTextures(1, &glID);
//	glBindTexture  (GL_TEXTURE_2D, glID);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexImage2D   (GL_TEXTURE_2D, 0, 4, resulting_text->w,	resulting_text->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,	resulting_text->pixels);
//	return glID;
//}

void		drawQuadRec(t_Quad h, int sub, GLuint id, float level, bool show_border, bool active)
{
	glBindTexture  (GL_TEXTURE_2D, id);

	if (sub == 1)
    {
		if (show_border)
		{
			glBindTexture  (GL_TEXTURE_2D, 0);
			if (active)
				glColor4f(0.0f, 0.8f, 0.0f, 1.0f);
			else
				glColor4f(0.7f, 0.7f, 0.7f, 0.4f);
			glBegin( GL_LINE_LOOP );
			glTexCoord2f( h.TargetUpLeftTP.x , h.TargetUpLeftTP.y );
			glVertex3f(h.TargetUpLeft.x, h.TargetUpLeft.y, level + 0.01f);
			glTexCoord2f( h.TargetDownLeftTP.x , h.TargetDownLeftTP.y );
			glVertex3f(h.TargetDownLeft.x, h.TargetDownLeft.y, level + 0.01f);
			glTexCoord2f( h.TargetDownRightTP.x , h.TargetDownRightTP.y );
			glVertex3f(h.TargetDownRight.x, h.TargetDownRight.y, level + 0.01f);
			glTexCoord2f( h.TargetUpRightTP.x , h.TargetUpRightTP.y );
			glVertex3f(h.TargetUpRight.x, h.TargetUpRight.y, level + 0.01f);
			glEnd();
			glBindTexture  (GL_TEXTURE_2D, id);
		}
		glBegin( GL_QUADS );
		glTexCoord2f( h.TargetUpLeftTP.x , h.TargetUpLeftTP.y );
		glColor4f(h.TargetUpLeftAlpha, h.TargetUpLeftAlpha, h.TargetUpLeftAlpha, h.TargetUpLeftAlpha);
		glVertex3f(h.TargetUpLeft.x, h.TargetUpLeft.y, level);
		glTexCoord2f( h.TargetDownLeftTP.x , h.TargetDownLeftTP.y );
		glColor4f(h.TargetDownLeftAlpha, h.TargetDownLeftAlpha, h.TargetDownLeftAlpha, h.TargetDownLeftAlpha);
		glVertex3f(h.TargetDownLeft.x, h.TargetDownLeft.y, level);
		glTexCoord2f( h.TargetDownRightTP.x , h.TargetDownRightTP.y );
		glColor4f(h.TargetDownRightAlpha, h.TargetDownRightAlpha, h.TargetDownRightAlpha, h.TargetDownRightAlpha);
		glVertex3f(h.TargetDownRight.x, h.TargetDownRight.y, level);
		glTexCoord2f( h.TargetUpRightTP.x , h.TargetUpRightTP.y );
		glColor4f(h.TargetUpRightAlpha, h.TargetUpRightAlpha, h.TargetUpRightAlpha, h.TargetUpRightAlpha);
		glVertex3f(h.TargetUpRight.x, h.TargetUpRight.y, level);
		glEnd();
    }
  else
    {
      t_Quad h1 = h;
      t_Quad h2 = h;
      t_Quad h3 = h;
      t_Quad h4 = h;

      //les quads
      h1.TargetUpRight.x		= (h.TargetUpLeft.x		+ h.TargetUpRight.x)/2;
      h1.TargetUpRight.y		= (h.TargetUpLeft.y		+ h.TargetUpRight.y)/2;
      h1.TargetDownLeft.x		= (h.TargetUpLeft.x		+ h.TargetDownLeft.x)/2;
      h1.TargetDownLeft.y		= (h.TargetUpLeft.y		+ h.TargetDownLeft.y)/2;
      h1.TargetDownRight.x	= (h.TargetUpLeft.x		+ h.TargetDownRight.x	+ h.TargetDownLeft.x		+ h.TargetUpRight.x)/4;
      h1.TargetDownRight.y	= (h.TargetUpLeft.y		+ h.TargetDownRight.y	+ h.TargetDownLeft.y		+ h.TargetUpRight.y)/4;

      h2.TargetUpLeft.x		= (h.TargetUpLeft.x		+ h.TargetUpRight.x)/2;
      h2.TargetUpLeft.y		= (h.TargetUpLeft.y		+ h.TargetUpRight.y)/2;
      h2.TargetDownLeft.x		= (h.TargetDownLeft.x	+ h.TargetUpRight.x	+	h.TargetUpLeft.x			+ h.TargetDownRight.x)/4;
      h2.TargetDownLeft.y		= (h.TargetDownLeft.y	+ h.TargetUpRight.y	+	h.TargetUpLeft.y			+ h.TargetDownRight.y)/4;
      h2.TargetDownRight.x	= (h.TargetDownRight.x	+ h.TargetUpRight.x)/2;
      h2.TargetDownRight.y	= (h.TargetDownRight.y	+ h.TargetUpRight.y)/2;

      h3.TargetUpLeft.x		= (h.TargetUpLeft.x		+ h.TargetDownLeft.x)/2;
      h3.TargetUpLeft.y		= (h.TargetUpLeft.y		+ h.TargetDownLeft.y)/2;
      h3.TargetUpRight.x		= (h.TargetUpLeft.x		+ h.TargetDownRight.x	+	h.TargetDownLeft.x		+ h.TargetUpRight.x)/4;
      h3.TargetUpRight.y		= (h.TargetUpLeft.y		+ h.TargetDownRight.y	+	h.TargetDownLeft.y		+ h.TargetUpRight.y)/4;
      h3.TargetDownRight.x	= (h.TargetDownLeft.x	+ h.TargetDownRight.x)/2;
      h3.TargetDownRight.y	= (h.TargetDownLeft.y	+ h.TargetDownRight.y)/2;

      h4.TargetUpLeft.x		= (h.TargetUpLeft.x		+ h.TargetDownRight.x	+	h.TargetDownLeft.x		+ h.TargetUpRight.x)/4;
      h4.TargetUpLeft.y		= (h.TargetUpLeft.y		+ h.TargetDownRight.y	+	h.TargetDownLeft.y		+ h.TargetUpRight.y)/4;
      h4.TargetUpRight.x		= (h.TargetUpRight.x	+ h.TargetDownRight.x)/2;
      h4.TargetUpRight.y		= (h.TargetUpRight.y	+ h.TargetDownRight.y)/2;
      h4.TargetDownLeft.x		= (h.TargetDownLeft.x	+ h.TargetDownRight.x)/2;
      h4.TargetDownLeft.y		= (h.TargetDownLeft.y	+ h.TargetDownRight.y)/2;


      //les reperes internes de la texture
      h1.TargetUpRightTP.x	= (h.TargetUpLeftTP.x		+ h.TargetUpRightTP.x)/2;
      h1.TargetUpRightTP.y	= (h.TargetUpLeftTP.y		+ h.TargetUpRightTP.y)/2;
      h1.TargetDownLeftTP.x	= (h.TargetUpLeftTP.x		+ h.TargetDownLeftTP.x)/2;
      h1.TargetDownLeftTP.y	= (h.TargetUpLeftTP.y		+ h.TargetDownLeftTP.y)/2;
      h1.TargetDownRightTP.x	= (h.TargetUpLeftTP.x		+ h.TargetDownRightTP.x	+ h.TargetDownLeftTP.x		+ h.TargetUpRightTP.x)/4;
      h1.TargetDownRightTP.y	= (h.TargetUpLeftTP.y		+ h.TargetDownRightTP.y	+ h.TargetDownLeftTP.y		+ h.TargetUpRightTP.y)/4;

      h2.TargetUpLeftTP.x		= (h.TargetUpLeftTP.x		+ h.TargetUpRightTP.x)/2;
      h2.TargetUpLeftTP.y		= (h.TargetUpLeftTP.y		+ h.TargetUpRightTP.y)/2;
      h2.TargetDownLeftTP.x	= (h.TargetDownLeftTP.x		+ h.TargetUpRightTP.x	+	h.TargetUpLeftTP.x			+ h.TargetDownRightTP.x)/4;
      h2.TargetDownLeftTP.y	= (h.TargetDownLeftTP.y		+ h.TargetUpRightTP.y	+	h.TargetUpLeftTP.y			+ h.TargetDownRightTP.y)/4;
      h2.TargetDownRightTP.x	= (h.TargetDownRightTP.x	+ h.TargetUpRightTP.x)/2;
      h2.TargetDownRightTP.y	= (h.TargetDownRightTP.y	+ h.TargetUpRightTP.y)/2;

      h3.TargetUpLeftTP.x		= (h.TargetUpLeftTP.x		+ h.TargetDownLeftTP.x)/2;
      h3.TargetUpLeftTP.y		= (h.TargetUpLeftTP.y		+ h.TargetDownLeftTP.y)/2;
      h3.TargetUpRightTP.x	= (h.TargetUpLeftTP.x		+ h.TargetDownRightTP.x	+	h.TargetDownLeftTP.x		+ h.TargetUpRightTP.x)/4;
      h3.TargetUpRightTP.y	= (h.TargetUpLeftTP.y		+ h.TargetDownRightTP.y	+	h.TargetDownLeftTP.y		+ h.TargetUpRightTP.y)/4;
      h3.TargetDownRightTP.x	= (h.TargetDownLeftTP.x		+ h.TargetDownRightTP.x)/2;
      h3.TargetDownRightTP.y	= (h.TargetDownLeftTP.y		+ h.TargetDownRightTP.y)/2;

      h4.TargetUpLeftTP.x		= (h.TargetUpLeftTP.x		+ h.TargetDownRightTP.x	+	h.TargetDownLeftTP.x		+ h.TargetUpRightTP.x)/4;
      h4.TargetUpLeftTP.y		= (h.TargetUpLeftTP.y		+ h.TargetDownRightTP.y	+	h.TargetDownLeftTP.y		+ h.TargetUpRightTP.y)/4;
      h4.TargetUpRightTP.x	= (h.TargetUpRightTP.x		+ h.TargetDownRightTP.x)/2;
      h4.TargetUpRightTP.y	= (h.TargetUpRightTP.y		+ h.TargetDownRightTP.y)/2;
      h4.TargetDownLeftTP.x	= (h.TargetDownLeftTP.x		+ h.TargetDownRightTP.x)/2;
      h4.TargetDownLeftTP.y	= (h.TargetDownLeftTP.y		+ h.TargetDownRightTP.y)/2;

      // les alphas
      h1.TargetUpRightAlpha	= (h.TargetUpLeftAlpha		+ h.TargetUpRightAlpha)/2;
      h1.TargetDownLeftAlpha	= (h.TargetUpLeftAlpha		+ h.TargetDownLeftAlpha)/2;
      h1.TargetDownRightAlpha	= (h.TargetUpLeftAlpha		+ h.TargetDownRightAlpha	+ h.TargetDownLeftAlpha		+ h.TargetUpRightAlpha)/4;

      h2.TargetUpLeftAlpha	= (h.TargetUpLeftAlpha		+ h.TargetUpRightAlpha)/2;
      h2.TargetDownLeftAlpha	= (h.TargetDownLeftAlpha	+ h.TargetUpRightAlpha	+	h.TargetUpLeftAlpha			+ h.TargetDownRightAlpha)/4;
      h2.TargetDownRightAlpha	= (h.TargetDownRightAlpha	+ h.TargetUpRightAlpha)/2;

      h3.TargetUpLeftAlpha	= (h.TargetUpLeftAlpha		+ h.TargetDownLeftAlpha)/2;
      h3.TargetUpRightAlpha	= (h.TargetUpLeftAlpha		+ h.TargetDownRightAlpha	+	h.TargetDownLeftAlpha		+ h.TargetUpRightAlpha)/4;
      h3.TargetDownRightAlpha	= (h.TargetDownLeftAlpha	+ h.TargetDownRightAlpha)/2;

      h4.TargetUpLeftAlpha	= (h.TargetUpLeftAlpha		+ h.TargetDownRightAlpha	+	h.TargetDownLeftAlpha		+ h.TargetUpRightAlpha)/4;
      h4.TargetUpRightAlpha	= (h.TargetUpRightAlpha		+ h.TargetDownRightAlpha)/2;
      h4.TargetDownLeftAlpha	= (h.TargetDownLeftAlpha	+ h.TargetDownRightAlpha)/2;


      drawQuadRec(h1, sub - 1, id, level, show_border, active);
      drawQuadRec(h2, sub - 1, id, level, show_border, active);
      drawQuadRec(h3, sub - 1, id, level, show_border, active);
      drawQuadRec(h4, sub - 1, id, level, show_border, active);
    }
}

inline t_Point	calc_linear(t_Point A, t_Point B, t_Point C)
{
	t_Point G;
	t_Point Gp;
	t_Point Cp;

	/* changement de repere */
	A.x -= B.x;
	A.y -= B.y;
	C.x -= B.x;
	C.y -= B.y;

	/* mirroir de C */
	Cp.x = -C.x;
	Cp.y = -C.y;

	//centre de gravité de AB0Cp
	G.x		= (A.x + 0 + Cp.x)/3;
	G.y		= (A.y + 0 + Cp.y)/3;
	//centre de gravité de AGB0
	Gp.x	= (A.x + G.x + 0)/3;
	Gp.y	= (A.y + G.y + 0)/3;

	/* retour au repère initial */
	Gp.x += B.x;
	Gp.y += B.y;

	return (Gp);
}

inline float	calc_Alinear(float A, float B, float C)
{
	float G;

	//centre de gravité de ABC
	G	= (A + B + C)/3;
	//centre de gravité de AGB
	G	= (A + G + B)/3;
	G	= (A + G + B)/3;
	return (G);
}

int quadLevel = 1;

void drawPillow(PillowGraphy p, int sub, GLuint id, float level, bool show_border, bool active)
{
	glBindTexture  (GL_TEXTURE_2D, id);

	static int count = 0;
	if (sub == 1)
	{
		count ++;
		drawQuadRec(p.NE, quadLevel, id, level, show_border, active);
		drawQuadRec(p.NW, quadLevel, id, level, show_border, active);
		drawQuadRec(p.SW, quadLevel, id, level, show_border, active);
		drawQuadRec(p.SE, quadLevel, id, level, show_border, active);
	}
	else
	{
		// linéarisation !
		t_Point	linear[16];
		t_Point	linearTP[16];
		float		linearA[16];
		PillowGraphy pillows[4];

		// les bords
		linear[0x0] = calc_linear(p.NW.TargetUpLeft		, p.NW.TargetUpRight	, p.NE.TargetUpRight);
		linear[0x1] = calc_linear(p.NW.TargetUpLeft		, p.NW.TargetDownLeft	, p.SW.TargetDownLeft);
		linear[0x2] = calc_linear(p.SW.TargetDownLeft	, p.SW.TargetUpLeft		, p.NW.TargetUpLeft);
		linear[0x3] = calc_linear(p.SW.TargetDownLeft	, p.SW.TargetDownRight, p.SE.TargetDownRight);
		linear[0x4] = calc_linear(p.SE.TargetDownRight, p.SE.TargetDownLeft	, p.SW.TargetDownLeft);
		linear[0x5] = calc_linear(p.SE.TargetDownRight, p.SE.TargetUpRight	, p.NE.TargetUpRight);
		linear[0x6] = calc_linear(p.NE.TargetUpRight	, p.NE.TargetDownRight, p.SE.TargetDownRight);
		linear[0x7] = calc_linear(p.NE.TargetUpRight	, p.NE.TargetUpLeft		, p.NW.TargetUpLeft);

		// les joints
		linear[0x8] = calc_linear(p.NE.TargetUpLeft		, p.NE.TargetDownLeft	, p.SE.TargetDownLeft);
		linear[0x9] = calc_linear(p.NW.TargetDownLeft	, p.NW.TargetDownRight, p.NE.TargetDownRight);
		linear[0xA] = calc_linear(p.SE.TargetDownLeft	, p.SE.TargetUpLeft		, p.NE.TargetUpLeft);
		linear[0xB] = calc_linear(p.SE.TargetUpRight	, p.SE.TargetUpLeft		, p.SW.TargetUpLeft);
		
		// les centres
		linear[0xC] = calc_linear(linear[0x0]		, linear[0x9]		, linear[0x3]);
		linear[0xD] = calc_linear(linear[0x3]		, linear[0x9]		, linear[0x0]);
		linear[0xE] = calc_linear(linear[0x4]		, linear[0xB]		, linear[0x7]);
		linear[0xF] = calc_linear(linear[0x7]		, linear[0xB]		, linear[0x4]);

		// re dans la texture ...
		// les bords
		linearTP[0x0] = calc_linear(p.NW.TargetUpLeftTP		, p.NW.TargetUpRightTP	, p.NE.TargetUpRightTP);
		linearTP[0x1] = calc_linear(p.NW.TargetUpLeftTP		, p.NW.TargetDownLeftTP	, p.SW.TargetDownLeftTP);
		linearTP[0x2] = calc_linear(p.SW.TargetDownLeftTP	, p.SW.TargetUpLeftTP		, p.NW.TargetUpLeftTP);
		linearTP[0x3] = calc_linear(p.SW.TargetDownLeftTP	, p.SW.TargetDownRightTP, p.SE.TargetDownRightTP);
		linearTP[0x4] = calc_linear(p.SE.TargetDownRightTP, p.SE.TargetDownLeftTP	, p.SW.TargetDownLeftTP);
		linearTP[0x5] = calc_linear(p.SE.TargetDownRightTP, p.SE.TargetUpRightTP	, p.NE.TargetUpRightTP);
		linearTP[0x6] = calc_linear(p.NE.TargetUpRightTP	, p.NE.TargetDownRightTP, p.SE.TargetDownRightTP);
		linearTP[0x7] = calc_linear(p.NE.TargetUpRightTP	, p.NE.TargetUpLeftTP		, p.NW.TargetUpLeftTP);

		// les joints
		linearTP[0x8] = calc_linear(p.NE.TargetUpLeftTP		, p.NE.TargetDownLeftTP	, p.SE.TargetDownLeftTP);
		linearTP[0x9] = calc_linear(p.NW.TargetDownLeftTP	, p.NW.TargetDownRightTP, p.NE.TargetDownRightTP);
		linearTP[0xA] = calc_linear(p.SE.TargetDownLeftTP	, p.SE.TargetUpLeftTP		, p.NE.TargetUpLeftTP);
		linearTP[0xB] = calc_linear(p.SE.TargetUpRightTP	, p.SE.TargetUpLeftTP		, p.SW.TargetUpLeftTP);

		// les centres
		linearTP[0xC] = calc_linear(linearTP[0x0]		, linearTP[0x9]		, linearTP[0x3]);
		linearTP[0xD] = calc_linear(linearTP[0x3]		, linearTP[0x9]		, linearTP[0x0]);
		linearTP[0xE] = calc_linear(linearTP[0x4]		, linearTP[0xB]		, linearTP[0x7]);
		linearTP[0xF] = calc_linear(linearTP[0x7]		, linearTP[0xB]		, linearTP[0x4]);

		// re dans les alphas ...
		// les bords
		linearA[0x0] = calc_Alinear(p.NW.TargetUpLeftAlpha		, p.NW.TargetUpRightAlpha		, p.NE.TargetUpRightAlpha);
		linearA[0x1] = calc_Alinear(p.NW.TargetUpLeftAlpha		, p.NW.TargetDownLeftAlpha	, p.SW.TargetDownLeftAlpha);
		linearA[0x2] = calc_Alinear(p.SW.TargetDownLeftAlpha	, p.SW.TargetUpLeftAlpha		, p.NW.TargetUpLeftAlpha);
		linearA[0x3] = calc_Alinear(p.SW.TargetDownLeftAlpha	, p.SW.TargetDownRightAlpha	, p.SE.TargetDownRightAlpha);
		linearA[0x4] = calc_Alinear(p.SE.TargetDownRightAlpha	, p.SE.TargetDownLeftAlpha	, p.SW.TargetDownLeftAlpha);
		linearA[0x5] = calc_Alinear(p.SE.TargetDownRightAlpha	, p.SE.TargetUpRightAlpha		, p.NE.TargetUpRightAlpha);
		linearA[0x6] = calc_Alinear(p.NE.TargetUpRightAlpha		, p.NE.TargetDownRightAlpha	, p.SE.TargetDownRightAlpha);
		linearA[0x7] = calc_Alinear(p.NE.TargetUpRightAlpha		, p.NE.TargetUpLeftAlpha		, p.NW.TargetUpLeftAlpha);

		// les joints
		linearA[0x8] = calc_Alinear(p.NE.TargetUpLeftAlpha		, p.NE.TargetDownLeftAlpha	, p.SE.TargetDownLeftAlpha);
		linearA[0x9] = calc_Alinear(p.NW.TargetDownLeftAlpha	, p.NW.TargetDownRightAlpha , p.NE.TargetDownRightAlpha);
		linearA[0xA] = calc_Alinear(p.SE.TargetDownLeftAlpha	, p.SE.TargetUpLeftAlpha		, p.NE.TargetUpLeftAlpha);
		linearA[0xB] = calc_Alinear(p.SE.TargetUpRightAlpha		, p.SE.TargetUpLeftAlpha		, p.SW.TargetUpLeftAlpha);

		// les centres
		linearA[0xC] = calc_Alinear(linearA[0x0]		, linearA[0x9]		, linearA[0x3]);
		linearA[0xD] = calc_Alinear(linearA[0x3]		, linearA[0x9]		, linearA[0x0]);
		linearA[0xE] = calc_Alinear(linearA[0x4]		, linearA[0xB]		, linearA[0x7]);
		linearA[0xF] = calc_Alinear(linearA[0x7]		, linearA[0xB]		, linearA[0x4]);

		PillowGraphy p1 = {
			{
				linear[0x0]		, p.NW.TargetUpRight,
				linear[0xC]		, linear[0x8],
				linearTP[0x0]	, p.NW.TargetUpRightTP,
				linearTP[0xC]	, linearTP[0x8],
				linearA[0x0]	, p.NW.TargetUpRightAlpha,
				linearA[0xC]	, linearA[0x8],
				0.0, 0.0
			},{
				p.NW.TargetUpLeft		, linear[0x0],
				linear[0x1]		, linear[0xC],
				p.NW.TargetUpLeftTP	, linearTP[0x0],
				linearTP[0x1]	, linearTP[0xC],
				p.NW.TargetUpLeftAlpha		, linearA[0x0],
				linearA[0x1]		, linearA[0xC],
				0.0, 0.0
			},{
				linear[0x1]		, linear[0xC],
				p.NW.TargetDownLeft		, linear[0x9],
				linearTP[0x1]	, linearTP[0xC],
				p.NW.TargetDownLeftTP	, linearTP[0x9],
				linearA[0x1]		, linearA[0xC],
				p.NW.TargetDownLeftAlpha		, linearA[0x9],
				0.0, 0.0
			},{
				linear[0xC]		, linear[0x8],
				linear[0x9]		, p.NW.TargetDownRight,
				linearTP[0xC]	, linearTP[0x8],
				linearTP[0x9]	, p.NW.TargetDownRightTP,
				linearA[0xC]		, linearA[0x8],
				linearA[0x9]		, p.NW.TargetDownRightAlpha,
				0.0, 0.0
			}
		};

		PillowGraphy p2 = {
			{
				linear[0x9]		, p.SW.TargetUpRight,
				linear[0xD]		, linear[0xA],
				linearTP[0x9]	, p.SW.TargetUpRightTP,
				linearTP[0xD]	, linearTP[0xA],
				linearA[0x9]		, p.SW.TargetUpRightAlpha,
				linearA[0xD]		, linearA[0xA],
				0.0, 0.0
			},{
				p.SW.TargetUpLeft		, linear[0x9],
				linear[0x2]		, linear[0xD],
				p.SW.TargetUpLeftTP	, linearTP[0x9],
				linearTP[0x2]	, linearTP[0xD],
				p.SW.TargetUpLeftAlpha		, linearA[0x9],
				linearA[0x2]		, linearA[0xD],
				0.0, 0.0
			},{
				linear[0x2]		, linear[0xD],
				p.SW.TargetDownLeft		, linear[0x3],
				linearTP[0x2]	, linearTP[0xD],
				p.SW.TargetDownLeftTP	, linearTP[0x3],
				linearA[0x2]		, linearA[0xD],
				p.SW.TargetDownLeftAlpha		, linearA[0x3],
				0.0, 0.0
			},{
				linear[0xD]		, linear[0xA],
				linear[0x3]		, p.SW.TargetDownRight,
				linearTP[0xD]	, linearTP[0xA],
				linearTP[0x3]	, p.SW.TargetDownRightTP,
				linearA[0xD]		, linearA[0xA],
				linearA[0x3]		, p.SW.TargetDownRightAlpha,
				0.0, 0.0
			}
		};

		PillowGraphy p3 = {
			{
				linear[0xB], p.SE.TargetUpRight, linear[0xE], linear[0x5],
				linearTP[0xB], p.SE.TargetUpRightTP, linearTP[0xE], linearTP[0x5],
				linearA[0xB], p.SE.TargetUpRightAlpha, linearA[0xE], linearA[0x5],
				0.0, 0.0
			},{
				p.SE.TargetUpLeft, linear[0xB], linear[0xA], linear[0xE],
				p.SE.TargetUpLeftTP, linearTP[0xB], linearTP[0xA], linearTP[0xE],
				p.SE.TargetUpLeftAlpha, linearA[0xB], linearA[0xA], linearA[0xE],
				0.0, 0.0
			},{
				linear[0xA], linear[0xE], p.SE.TargetDownLeft, linear[0x4],
				linearTP[0xA], linearTP[0xE], p.SE.TargetDownLeftTP, linearTP[0x4],
				linearA[0xA], linearA[0xE], p.SE.TargetDownLeftAlpha, linearA[0x4],
				0.0, 0.0
			},{
				linear[0xE], linear[0x5], linear[0x4], p.SE.TargetDownRight,
				linearTP[0xE], linearTP[0x5], linearTP[0x4], p.SE.TargetDownRightTP,
				linearA[0xE], linearA[0x5], linearA[0x4], p.SE.TargetDownRightAlpha,
				0.0, 0.0
			}
		};

		PillowGraphy p4 = {
			{
				linear[0x7], p.NE.TargetUpRight, linear[0xF], linear[0x6],
				linearTP[0x7], p.NE.TargetUpRightTP, linearTP[0xF], linearTP[0x6],
				linearA[0x7], p.NE.TargetUpRightAlpha, linearA[0xF], linearA[0x6],
				0.0, 0.0
			},{
				p.NE.TargetUpLeft, linear[0x7], linear[0x8], linear[0xF],
				p.NE.TargetUpLeftTP, linearTP[0x7], linearTP[0x8], linearTP[0xF],
				p.NE.TargetUpLeftAlpha, linearA[0x7], linearA[0x8], linearA[0xF],
				0.0, 0.0
			},{
				linear[0x8], linear[0xF], p.NE.TargetDownLeft, linear[0xB],
				linearTP[0x8], linearTP[0xF], p.NE.TargetDownLeftTP, linearTP[0xB],
				linearA[0x8], linearA[0xF], p.NE.TargetDownLeftAlpha, linearA[0xB],
				0.0, 0.0
			},{
				linear[0xF], linear[0x6], linear[0xB], p.NE.TargetDownRight,
				linearTP[0xF], linearTP[0x6], linearTP[0xB], p.NE.TargetDownRightTP,
				linearA[0xF], linearA[0x6], linearA[0xB], p.NE.TargetDownRightAlpha,
				0.0, 0.0
			}
		};
		pillows[0] = p1;
		pillows[1] = p2;
		pillows[2] = p3;
		pillows[3] = p4;
//#pragma omp parallel for
		for (int i = 0; i < 4; i++)
			drawPillow(pillows[i], sub - 1, id, level, show_border, active);
		//drawPillow(p2, sub - 1, show_border);
		//drawPillow(p3, sub - 1, show_border);
		//drawPillow(p4, sub - 1, show_border);
	}
}
