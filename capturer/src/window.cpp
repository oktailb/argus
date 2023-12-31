#include <window.hpp>
#include <iostream>
#include <vector>
#include <GL/freeglut.h>

#ifdef WITH_DX
#include <d3d9.h>
#endif

void			window::openglContext()
{
#ifdef WITH_SDL
  //SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  //SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );

  //SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,0);
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho(-(_width)/2, (_width)/2,
	  -(_height)/2, (_height)/2,
	  -1000		, 1000 );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glEnable(GL_NORMALIZE);	//_a_elv to be put in the effects
  glLoadIdentity();
  glEnable(GL_DEPTH_TEST);	//_a_elv to be put in the effects
  glLoadIdentity();
  glEnable(GL_TEXTURE_2D);
  glLoadIdentity();
  glDisable( GL_LIGHTING );
  glLoadIdentity();

  glClearColor(0.0, 0.0 ,0.0, 0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glFlush();
#endif
	gl_enabled = true;

}

void my_exit(void *data)
{
	data = data;
	exit(0);
}

void my_clear(void *data)
{
	window*	f;

	f = (window*)data;
	f->putSprite(0, 0, f->_width, f->_height, square, fill, 0x00000000, NORMAL);
	exit(0);
}

window::window(int argc, char **argv, int winW,int winH, int winX, int winY, int fullscreen)
{
	_fullscreen = fullscreen;
	_width = winW;
	_height = winH;
	_bpp = 4;
	_stepy = _width * _bpp;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_BORDERLESS | GLUT_CAPTIONLESS); //GLUT_DEPTH | 
	glutInitWindowSize(winW, winH);
	glutCreateWindow("W2W");
	
#ifdef _WIN32
	HWND hwnd = FindWindow(NULL,(LPCSTR)"W2W");
	::SetWindowLong(hwnd, GWL_STYLE, ::GetWindowLong(hwnd, GWL_STYLE) & ~(WS_BORDER | WS_DLGFRAME | WS_THICKFRAME));
    ::SetWindowLong(hwnd, GWL_EXSTYLE, ::GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_DLGMODALFRAME);
#endif

	glutInitWindowPosition(winX, winY);
	glutInitWindowSize(winW, winH);
	glutInitWindowPosition(winX, winY);
	gl_enabled = true;
}

void window::title(std::string title)
{

}

void window::putSprite(int x,  int y, int w, int h, int (*drawer)(int, int, int, int), int (*colorer)(int, int, int, int, int), int mask, int mode)
{
	for(register int i = 0; i < w; i++)
	{
		for(register int j = 0; j < h; j++)
		{
			if (drawer(i, j, w, h))
			{
				if (mode & CENTER)
					putPixel(x + i - w/2, y + j - h/2, colorer(i, j, w, h, mask), mode);
				else
					putPixel(x + i, y + j, colorer(i, j, w, h, mask), mode);
			}
		}
	}
}
void window::putPixel(int x, int y, int color, int mode)
{
//#ifdef WITH_SDL
//	if (mode & LOOP)
//	{
//		x = (x + this->_width)%this->_width;
//		y = (y + this->_height)%this->_height;
//	}
//	if (mode & CROP)
//	{
//		if (x >= this->_width)
//			return;
//		if (y >= this->_height)
//			return;
//		if (x < 0)
//			return;
//		if (y < 0)
//			return;
//	}
//	if (mode & VMIRROR)
//		x = this->_width - x - 1;
//	if (mode & HMIRROR)
//		y = this->_height - y - 1;
//	if (mode & MIRROR)
//	{
//		x = this->_width - x - 1;
//		y = this->_height - y - 1;
//	}
//
//	Uint8 * p = ((Uint8 *)_buffer) + (y) * _stepy + (x) * _bpp;
//    switch(_bpp)
//    {
//	  case 1:
//		*p = (Uint8) color;
//		break;
//        case 2:
//            *(Uint16 *)p = (Uint16) color;
//            break;
//        case 3:
//            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
//            {
//                *(Uint16 *)p = ((color >> 8) & 0xff00) | ((color >> 8) & 0xff);
//                *(p + 2) = color & 0xff;
//            }
//            else
//            {
//                *(Uint16 *)p = color & 0xffff;
//                *(p + 2) = ((color >> 16) & 0xff) ;
//            }
//            break;
//        case 4:
//            *(Uint32 *)p = color;
//            break;
//    }
//#endif
}

int window::getPixel(int x, int y)
{
//#ifdef WITH_SDL
//  Uint8 *p = ((Uint8 *)_buffer) + y * _stepy + x * _bpp;
//
//  switch(_bpp) {
//  case 1:
//    return *p;
//    break;
//
//  case 2:
//    return *(unsigned short int *)p;
//    break;
//
//  case 3:
//    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
//      return p[0] << 16 | p[1] << 8 | p[2];
//    else
//      return p[0] | p[1] << 8 | p[2] << 16;
//    break;
//
//  case 4:
//    return *(unsigned int *)p;
//    break;
//
//  default:
//    return 0;       /* shouldn't happen, but avoids warnings */
//  }
//#endif
	return (0);
}

void window::update()
{
#ifdef WITH_SDL
	if (gl_enabled)
	{

	 // glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (_gfxMode == SDL)
			;//SDL_GL_SwapBuffers();
		else if (_gfxMode == GDI)
		{
			//InvalidateRect(*hWnd, NULL, true);
			//UpdateWindow(*hWnd);
		 //   SwapBuffers(*hDC);
		}
		else
		{
			glutSwapBuffers();
			glutPostRedisplay();
		}

        //if (GetAsyncKeyState(VK_ESCAPE))
        //    SysShutdown();
	}
	else
	{
		//SDL_UpdateRect(_screen, 0, 0, _width, _height); 
		//SDL_Flip(_screen);
	}
	
#endif
}

int* window::getScreenSize()
{
//#ifdef WITH_SDL
//	int	*res;
//
//	res = (int*)malloc(2 * sizeof(*res));
//	const SDL_VideoInfo* myPointer = SDL_GetVideoInfo();
//	res[0] = myPointer->current_w;
//	res[1] = myPointer->current_h;
//	return(res);
//#endif
	return (NULL);
}

int* window::getPosition()
{
	//int *res;

	//res = (int*)malloc(2 * sizeof(*res));
	////static SDL_SysWMinfo pInfo;
	////SDL_VERSION(&pInfo.version);
	////SDL_GetWMInfo(&pInfo);
	////RECT r;
	////GetWindowRect(pInfo.window, &r);
	return (NULL);
}

#ifdef WITH_DX
LRESULT WINAPI window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
switch(msg)
{
   case WM_DESTROY:
      PostQuitMessage(0);
			exit(0);
      return(0);
   case WM_PAINT: // <— ADD THIS BLOCK
    //g_pDirect3D_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
    //g_pDirect3D_Device->Present(NULL, NULL, NULL, NULL);

      ValidateRect(hwnd, NULL);
      return(0);
}

   return(DefWindowProc(hwnd, msg, wParam, lParam));
}
#endif

#ifdef WITH_SDL
void					window::waitEventH(void (*addon)(SDL_Event	*))
{
	//if (_gfxMode == SDL)
	//{
	//	std::vector		<SDL_Event> events;
	//	SDL_Event		the_event;

	//	//std::cout << "wait event ..." << std::endl;
	//	while(SDL_PollEvent(&the_event))
	//	{
	//		std::cout << (int) the_event.type << std::endl;

	//		if ((the_event.type == SDL_KEYDOWN))
	//		{
	//			//std::cout << "key down" << std::endl;
	//			unsigned char	token = (unsigned char)the_event.key.keysym.sym;
	//			//std::cout << "event ... " << (int)token << std::endl;
	//			for (unsigned int i = 0; i < this->eventKeyboardList.size(); i++)
	//			{
	//				if (this->eventKeyboardList[i] && (this->eventKeyboardList[i]->state == the_event.type) && this->eventKeyboardList[i]->token == token)
	//					this->eventKeyboardList[i]->handler(this->eventKeyboardList[i]->data);
	//			}
	//		}
	//		else if ((the_event.type == SDL_MOUSEBUTTONDOWN))
	//		{
	//			//std::cout << "click/unclick" << std::endl;
	//			unsigned int	token = the_event.button.button;

	//			for (unsigned int i = 0; i < this->eventMouseList.size(); i++)
	//			{
	//				if (this->eventMouseList[i] && (this->eventMouseList[i]->state == the_event.type) && this->eventMouseList[i]->token == token)
	//					this->eventMouseList[i]->handler(this->eventMouseList[i]->data, the_event.motion.x, the_event.motion.y);
	//			}
	//			break;
	//		}
	//		else if ((the_event.type == SDL_MOUSEBUTTONUP))
	//		{
	//			//std::cout << "click/unclick" << std::endl;
	//			unsigned int	token = the_event.button.button;

	//			for (unsigned int i = 0; i < this->eventMouseList.size(); i++)
	//			{
	//				if (this->eventMouseList[i] && (this->eventMouseList[i]->state == the_event.type) && (this->eventMouseList[i]->token == token))
	//					this->eventMouseList[i]->handler(this->eventMouseList[i]->data, the_event.motion.x, the_event.motion.y);
	//			}
	//			break;
	//		}
	//		else if ((the_event.type == SDL_MOUSEMOTION))
	//		{
	//			//std::cout << "move" << std::endl;
	//			for (unsigned int i = 0; i < this->eventMouseList.size(); i++)
	//			{
	//				if (this->eventMouseList[i] && (this->eventMouseList[i]->state == the_event.type))
	//					this->eventMouseList[i]->handler(this->eventMouseList[i]->data, the_event.motion.x, the_event.motion.y);
	//			}
	//			break;
	//		}
	//		//if (addon != NULL)
	//		//	addon(&(*it));
	//	}
	//	events.clear();
	//}
	//else
	//{
	//	MSG msg;
	//	if (GetMessage(&msg, NULL, 0, 0) == TRUE) 
	//	{
	//        TranslateMessage(&msg);
	//	    DispatchMessage(&msg);
	//	}
	//}
}
#endif
#ifdef WITH_DX
void			window::waitEventH()
{
  if(GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}
#endif
void	window::addKeyboardEventH(unsigned int token, unsigned int	state, void (*handler)(void *, int, int),	void *data)
{
  t_eventInt	*tmp = new t_eventInt;
  tmp->token = token;
  tmp->state = state;
  tmp->handler = handler;
  tmp->data = data;
  this->eventKeyboardList.push_back(tmp);
}

void	window::addMouseEventH(unsigned int token, unsigned int	state, void (*handler)(void *data, int x, int y),	void *data)
{
  t_eventInt	*tmp = new t_eventInt;
  tmp->token = token;
  tmp->state = state;
  tmp->handler = handler;
  tmp->data = data;
  this->eventMouseList.push_back(tmp);
}










/**
*	Les methodes GDI
*/

t_eventChar *	eventKeyboardList[256];
t_eventInt *	eventMouseList[256];

inline unsigned int	GDI_MOUSE_BUTTON(int e)		{return ((((e) == WM_LBUTTONDOWN || (e) == WM_LBUTTONUP)?1:(((e) == WM_MBUTTONDOWN || (e) == WM_MBUTTONUP)?2:(((e) == WM_RBUTTONDOWN || (e) == WM_RBUTTONUP)?3:0))));						}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MSG			event;

	PeekMessage(&event, hwnd, NULL, NULL, PM_REMOVE);
	{
		unsigned int state = event.message;

		if (state == WM_CREATE)
		{

		}
		else if (state == WM_DESTROY)
		{
			//ReleaseDC(*hWnd, *hDC);
			//PostQuitMessage(0);
		}
		else if (state == WM_KEYDOWN || state == WM_KEYUP)
		{
			unsigned char	token = 'e'; //event.jesaispasquoi;
			for (int i = 0; i < 256; i++)
			{
				if (eventKeyboardList[i] && eventKeyboardList[i]->state == state && eventKeyboardList[i]->token == token)
					eventKeyboardList[i]->handler(eventKeyboardList[i]->data);
			}
		}
		else if (state == WM_LBUTTONDOWN || state == WM_LBUTTONUP 
				|| state == WM_MBUTTONDOWN || state == WM_MBUTTONUP 
					|| state == WM_RBUTTONDOWN || state == WM_RBUTTONUP)
		{
			unsigned int	token = GDI_MOUSE_BUTTON(event.message);
			for (int i = 0; i < 256; i++)
			{
				if (eventMouseList[i] && eventMouseList[i]->state == state && eventMouseList[i]->token == token)
					eventMouseList[i]->handler(eventMouseList[i]->data, 0, 0);
			}
		}
		/* else "La suite avec OF" */
	}
	return (DefWindowProc(hwnd,message,wParam,lParam));
}

void	window::gdiGetDrawable()
{
	WNDCLASSEX  windowClass;		//window class

	hWnd = (HWND *)malloc(sizeof(*(hWnd)));
	hDC = (HDC*)malloc(sizeof(*(hDC)));
	hInstance = GetModuleHandle(0);
	if (hWnd)
	{
		/*	Fill out the window class structure*/
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WndProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = hInstance;
		windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = "W2W";
		windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
		/*	Register window class*/
		if (!RegisterClassEx(&windowClass))
		{
			MessageBox(0, "Impossible d'initialiser le mode vidéo", "Création fenêtre échoué", MB_ICONHAND);
			return;
		}
		/*	Class registerd, so now create window     info->fullScreen?~(WS_CAPTION|WS_SIZEBOX):    */
		*(hWnd) = CreateWindowEx(NULL,	//extended style
			"W2W",					//class name
			"W2W",			//app name
			(WS_VISIBLE|WS_SYSMENU|WS_POPUP | WS_SYSMENU),
			0,0,					//x/y coords
			_width, _height,			//width,height
			NULL,						//handle to parent
			NULL,						//handle to menu
			hInstance,			//application instance
			NULL);						//no extra parameter's
		/*	Check if window creation failed*/
		if (!*(hWnd))
		{
			int toto;
			toto = GetLastError();
			MessageBox(0, "Impossible d'initialiser le mode vidéo", "Création fenêtre échoué", MB_ICONHAND);
			return;
		}
		*(hDC) = ::GetDC(*(hWnd));
		SetBkMode(*(hDC),TRANSPARENT);
		dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		dib.bmiHeader.biHeight = _height;		//Hauteur
		dib.bmiHeader.biWidth = _width;			//Largeur
		dib.bmiHeader.biPlanes = 1;				//Nombre de faces (on laisse ça à 1 pour cette application)
		dib.bmiHeader.biBitCount = 8 * _bpp;	//Nombre de couleurs (ne pas toucher sinon le tableau courrant ne sera plus valide)
		dib.bmiHeader.biCompression = BI_RGB;	//Compression (RGB Red Green Blue Pas de compression spécifique)
		lpBits = (LPBYTE)malloc(sizeof(*(lpBits))*4*_width*_height);
		_bpp = 4;
		_stepy = _width;
		_buffer = (unsigned char *)lpBits;
		

			HGLRC hGLRC = wglCreateContext(*hDC);
			wglMakeCurrent(*hDC, hGLRC);

			glShadeModel(GL_SMOOTH);					// Enable Smooth Shading
			glClearColor(0.0f, 0.0f, 0.0f, 0.5f);		// Black Background
			glClearDepth(1.0f);							// Depth Buffer Setup
			glEnable(GL_DEPTH_TEST);					// Enables Depth Testing
			glDepthFunc(GL_LEQUAL);						// The Type Of Depth Testing To Do
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations


			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			glOrtho(-(_width)/2, (_width)/2,
					-(_height)/2, (_height)/2,
					-1000		, 1000 );
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			glEnable(GL_NORMALIZE);	//_a_elv to be put in the effects
			glLoadIdentity();
			glEnable(GL_DEPTH_TEST);	//_a_elv to be put in the effects
			glLoadIdentity();
			glEnable(GL_TEXTURE_2D);
			glLoadIdentity();
			glDisable( GL_LIGHTING );
			glLoadIdentity();

			glClearColor(0.0, 0.0 ,0.0, 0);
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glFlush();
	//		OpenGLInit();
			gl_enabled = true;


		_gfxMode = GDI;

		return;
	}
	return;
}

void window::sdlGetDrawable()
{
	//if( SDL_Init( SDL_INIT_VIDEO ) == -1 )
	//	std::cout << "Can't init SDL: " << SDL_GetError() << std::endl;
	//atexit( SDL_Quit );
	////if(TTF_Init())
	////	TTF_GetError();
	////atexit(TTF_Quit);
	//_screen = SDL_SetVideoMode( _width, _height, 8 * _bpp, SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT | SDL_NOFRAME | (_fullscreen * SDL_FULLSCREEN));
	//_buffer = _screen->pixels;
	//if( _screen == NULL )
	//	std::cout << "Can't set video mode: " << SDL_GetError( ) << std::endl;
	//SDL_EnableKeyRepeat(10, 10);
	//directBuffer = _screen->pixels;
	//gl_enabled = false;
	//openglContext();
	//_gfxMode = SDL;
}



void window::gdiPutPixel(int x, int y, int pixel)
{
	unsigned int tmp = (_stepy * (_height - y - 1) + x) * _stepy;
	((unsigned char *)(_buffer))[tmp]   = 0;
	((unsigned char *)(_buffer))[tmp+1] = GETB(pixel);
	((unsigned char *)(_buffer))[tmp+2] = GETG(pixel);
	((unsigned char *)(_buffer))[tmp+3] = GETR(pixel);
}

unsigned int window::gdiGetPixel(int x, int y)
{
	unsigned int tmp = (_stepy * (y) + x) * _bpp;
	return (((unsigned char *)(_buffer))[tmp] | (((unsigned char *)(_buffer))[tmp+1] << 8) | (((unsigned char *)(_buffer))[tmp+2] << 16) | (((unsigned char *)(_buffer))[tmp+3] << 24));
}

void window::gdiUpdate(int x, int y, int w, int h)
{
	SetDIBitsToDevice(*(hDC), x, y, w, h, 0, 0, 0, h, _buffer, &(dib), DIB_RGB_COLORS);
}





