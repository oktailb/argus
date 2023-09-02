#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <math.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <map>
#include <string>

#ifdef WIN32
#include <windows.h>
#include <GL/wglext.h>
#elif __linux__
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include "input.hpp"
#endif

#include "types.h"

t_Point	calc_linear(t_Point P1, t_Point P2, t_Point P3);
float	calc_Alinear(float A, float B, float C);
t_Quad  pillow2Quad(int x, int y, PillowMesh p);

#define BETWEEN(min, val, max) ((((val) >= (min)) && ((val) <= (max)))?1:0)

//#define DIST(ax, ay, bx, by) sqrt(((ax) - (bx)) * ((ax) - (bx)) + ((ay) - (by)) * ((ay) - (by)))
#define DIST(a, b) sqrt(((a.x) - (b.x)) * ((a.x) - (b.x)) + ((a.y) - (b.y)) * ((a.y) - (b.y)))

#define GLERR {GLenum error = glGetError(); \
if (error != GL_NO_ERROR) { \
        std::cerr << "OpenGL error: " << error << " in file " << __FILE__ << " on line " << __LINE__ << ": "  << std::endl; \
} \
} \


#define QUAD_FROM_LIST(points, texture, alpha, C1, C2, C3, C4, Smooth, SmoothLen) \
{                                   \
    {                               \
        points[C1], points[C2],     \
        points[C3], points[C4]      \
    },{                             \
        texture[C1], texture[C2],   \
        texture[C3], texture[C4]    \
    },{                             \
        alpha[C1], alpha[C2],       \
        alpha[C3], alpha[C4]        \
    },                              \
    Smooth, SmoothLen               \
}                                   \

#define QUAD_NULL \
{                                   \
    {                               \
        0.0, 0.0,     \
        0.0, 0.0      \
    },{                             \
        0.0, 0.0,   \
        0.0, 0.0    \
    },{                             \
        0.0, 0.0,       \
        0.0, 0.0        \
    },                              \
    0.0, 0.0               \
}                                   \


class GLWidget
{
public:
    GLWidget(std::string filename);
    ~GLWidget();

    bool inEditMode() {return editMode;}
    void toggleEditMode();
    void enableDebugMode();
    void enableCaptureMode();
    void increaseAlpha();
    void decreaseAlpha();
    void increaseSmoothLen();
    void decreaseSmoothLen();
    void increasePillowRecursion();
    void decreasePillowRecursion();
    void increaseQuadRecursion();
    void decreaseQuadRecursion();
    void movePointTo(int x, int y);
    void movePointUp();
    void movePointDown();
    void movePointLeft();
    void movePointRight();
    void adjustR(bool way);
    void adjustG(bool way);
    void adjustB(bool way);

    void save(std::string conf);

    void selectPoint(int id);

    void setLastPos(int x, int y);

    void setStep(int newStep);

    void initializeGL();
    void paintGL();
    void updateTextureFromSharedMemory(char *data);
    void loadHSVTexture();
    void drawQuad(double coords[4][2], double textures[4][2], int level = 1);

    void resizeGL(int width, int height);
#ifdef __linux__
    input *getCapturer() const;
#endif

private:
    std::map<std::string, std::string> configuration;
    GLdouble clearColorR;
    GLdouble clearColorG;
    GLdouble clearColorB;
    GLdouble clearColorA;
    t_Point lastPos;
    unsigned int textureCapture;
    unsigned int textureHSV;
    unsigned int textureCurrent;

    std::string filename;

#ifdef WIN32
    LPVOID shm;
#elif __linux__
    void * shm;
    input * capturer;
#endif
    GLdouble width;
    GLdouble height;

    int windowWidth;
    int windowHeight;

    void    drawCross      (double x, double y, double size, GLuint id, float level, bool active);
    void	calcQuadRec(t_Quad h, int sub, GLuint id, float level);
    void    drawPillowFdf();
    void    drawPillow();
    void    calcPillow(PillowMesh p, int sub, GLuint id, float level);
    void    calcPillowRec(PillowMesh p, int sub, GLuint id, float level);
    void    calcPillowFdf(PillowMesh p, int sub, GLuint id, float level);
    void    calcPillowFdfRec(PillowMesh p, int sub, GLuint id, float level);
    void    drawEditMode(PillowMesh p, int sub, GLuint id, float level);
    void	drawQuadFdf(t_Quad h, int sub, GLuint id);
    void	calcQuadFdf(t_Quad h, int sub, GLuint id);
    void	calcQuadFdfRec(t_Quad h, int sub, GLuint id);

    /// delta ms between readibg two consecutive frame
    unsigned int                deltaMs;
    /// depth of the image
    int                         bitsPerPixel;
    int                         x;
    int                         y;
    /// raw image captured
    char *                      pixels;
    GLuint                      TextureID;
#ifdef __linux__
    Display *                   display;
    Window                      root;
    XWindowAttributes           attributes;
    XShmSegmentInfo             shminfo;
    XImage *                    ximg;
#elif _WIN32
    HWND                        hDesktopWnd;
    HDC                         hDesktopDC;
    HDC                         hCaptureDC;
    HBITMAP                     hCaptureBitmap;
    BITMAPINFOHEADER   			hBitmapInfo;
    bool                        virtualDesktop;
#endif
    int                         fps;
    bool                        videoSync;
    bool                        stats;
    int                         endLoop;
    bool                        editMode;
    bool                        edited;
    int                         recursionLevel;
    int                         quadLevel;
    std::string                     child;
    std::string                     prefix;
    std::string                     title;
    PillowMesh                pillowModel;
    double                      crossSize;
    GLuint                      glListIndexPicture;
    GLuint                      glListIndexGrid;
    int                         flip;
    int                         Zlevel;

    int                         selectedPointX;
    int                         selectedPointY;
    int                         step;
    double                      ramp;
    double                      aramp;
    double                      smoothLen;
    t_argusExchange *           header;
};

#endif
