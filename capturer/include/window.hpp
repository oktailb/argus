#pragma once
/*!
     *	\file		window.hpp
     *	\brief		basic window object
     *	\author		Vincent LECOQ
     *	\version	0.1
     */

#pragma warning (disable : 4996)

void my_exit(void *data);
void my_clear(void *data);

enum gfxmode {
	SDL,
	GDI
};

#ifdef WITH_SDL
#include <windows.h>
#include <GL/GL.h>
#include <sdl/SDL.h>
//#include <windows.h>
//#include <SDL/SDL_syswm.h>
//#include <SDL/SDL_ttf.h>
#endif
#ifdef WITH_DX
#include <windows.h>
#pragma comment(lib, "d3d9.lib")
#include <d3d9.h>D3dx9.lib
#endif

#include <argument.hpp>
#include <vector>

#define GETA(p) (char)(((p) >> 24) & 0xff)
#define GETR(p) (char)(((p) >> 16) & 0xff)
#define GETG(p) (char)(((p) >> 8) & 0xff)
#define GETB(p) (char)((p) & 0xff)

typedef union		u_pixel32Math
{
	unsigned int	p;
	unsigned char	map[4];
}					t_pixelMath;

typedef struct		s_eventInfo
{
	int				type;
	int				state;
	unsigned int	x;
	unsigned int	y;
	unsigned int	code;
	unsigned int	id;
}					t_eventInfo;

typedef struct		s_eventChar
{
	unsigned char	token;
	unsigned int	state;
	void			(*handler)(void *data);
	void			*data;
}					t_eventChar;

typedef struct		s_eventInt
{
	unsigned int	token;
	unsigned int	state;
	void			(*handler)(void *data, int x, int y);
	void			*data;
}					t_eventInt;

typedef struct		s_eventString
{
	unsigned char	*token;
	unsigned int	state;
	void			(*handler)(void *data);
	void			*data;
}					t_eventString;

#define NORMAL	1
#define LOOP		2
#define CROP		4
#define VMIRROR	8
#define HMIRROR	16
#define MIRROR	32
#define CENTER	64
/*!
     *	\class		window
     *	\brief		basic window object
     *	\author		Vincent LECOQ
     *	\version	0.1
     */
class window
{
	public:
		/*!
       *	\fn		window
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
		window(argument* args, int fullscreen);
		window(argument* args, int, int, int fullscreen);
		window(int winW,int winH, int winX, int winY, int fullscreen);
		window(int argc, char **argv, int winW,int winH, int winX, int winY, int fullscreen);
#ifdef WITH_DX
		LPDIRECT3D9       g_pDirect3D;
		LPDIRECT3DDEVICE9 g_pDirect3D_Device;
		LPDIRECT3DSURFACE9 m_bakBuf;
		window::window(int winW,int winH, int winX, int winY, HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShow);
#endif

		/*!
       *	\fn		~window
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
		~window();

		/*!
       *	\fn		getPixel
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
		int							getPixel(int x, int y);
		/*!
       *	\fn		putPixel
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
		void						putPixel(int x, int y, int color, int mode);
		/*!
       *	\fn		putSprite
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
		void						putSprite(int x, int y, int w, int h, int (*drawer)(int, int, int, int), int (*colorer)(int, int, int, int, int), int mask, int mode);
		/*!
       *	\fn		addKeyboardEventH
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
		void						addKeyboardEventH(unsigned int token, unsigned int	state, void (*handler)(void *data, int, int),	void *data);
		/*!
       *	\fn		waitEventH
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
#ifdef WITH_SDL
		void						waitEventH(void (*)(SDL_Event	*));
#endif
#ifdef WITH_DX
		static LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void	waitEventH();
			MSG msg;
#endif
		/*!
       *	\fn		addMouseEventH
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
		void						addMouseEventH(unsigned int token, unsigned int	state, void (*handler)(void *data, int x, int y),	void *data);
		/*!
       *	\fn		update
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
		void						update();

		/*!
       *	\fn		title
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
		void						title(std::string title);

		/*!
       *	\fn		getScreenSize
       *	\brief		Constructor.
       *	\author		Vincent LECOQ
       *	\version	0.1
       */
		int*						getScreenSize();

		/*!
     *	\fn		getPosition
     *	\brief		Constructor.
     *	\author		Vincent LECOQ
     *	\version	0.1
     */
		int* getPosition();

		int				_width;
		int				_height;
		void			*directBuffer;
#ifdef WITH_SDL
		SDL_Surface*				_screen;
#endif
	void			openglContext();









void			gdiGetDrawable();
void			gdiPutPixel(int x, int y, int pixel);
unsigned int	gdiGetPixel(int x, int y);
void			gdiUpdate(int x, int y, int w, int h);






void			sdlGetDrawable();
void			sdlPutPixel(int x, int y, int pixel);
unsigned int	sdlGetPixel(int x, int y);
void			sdlUpdate(int x, int y, int w, int h);

		std::vector<t_eventInt *>	eventKeyboardList;
		std::vector<t_eventInt *>	eventMouseList;



	private:
bool gl_enabled;
		HINSTANCE					hInstance;
		HDC							*hDC;
		HWND						*hWnd;
		BITMAPINFO					dib;
		LPBYTE						lpBits;
		//LRESULT CALLBACK 			WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		void*								_buffer;
		unsigned int				_bpp;
		unsigned int				_stepy;

		unsigned int				_fullscreen;
		int _gfxMode;
};

inline int circle(int x, int y, int w, int h)
{
	--w;
	--h;
	w >>= 1;
	h >>= 1;
	x -= w;
	y -= h;
	if ((x*x + y*y) < (w)*(h))
		return (1);
	else
		return (0);
}

inline int square(int x, int y, int w, int h)
{
	x = x;
	y = y;
	h = h;
	w = w;
	return (1);
}

inline int line(int x, int y, int w, int h)
{
	h = h;
	w = w;
	if (abs(x*h - y*w) <= 10)
		return (1);
	return (0);
}

inline int fill(int x, int y, int w, int h, int mask)
{
	x = x;
	y = y;
	h = h;
	w = w;
	return (mask);
}

inline int pifo(int x,int y, int w, int h, int mask)
{
	h = h;
	w = w;
	return ((255*x/w) * (GETR(mask)) | (((255*y/h) * (GETB(mask))) << 16));
}

inline int circle_blur(int x,int y, int w, int h, int mask)
{
	h = h;
	w = w;
	x -= w/2;
	y -= h/2;
	return (mask * ((x*x + y*y)/((w/2)*(h/2))));
}


