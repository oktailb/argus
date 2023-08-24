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
#endif

#include "types.h"

struct	s_Point
{
    GLdouble		x;
    GLdouble		y;

    inline struct s_Point operator+(const struct s_Point& other) const {
        s_Point res {x+other.x, y+other.y};
        return res;
    }

    inline struct s_Point& operator+=(const struct s_Point& other)
    {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    inline struct s_Point& operator-=(const struct s_Point& other)
    {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }

    inline struct s_Point operator-(const struct s_Point& other) const {
        s_Point res {x+other.x, y-other.y};
        return res;
    }

    inline struct s_Point operator/(float div) const {
        s_Point res {x / div, y / div};
        return res;
    }

    inline struct s_Point operator*(float mul) const {
        s_Point res {x * mul, y * mul};
        return res;
    }

    inline struct s_Point  operator- () {
        s_Point res {-x, -y};
        return res;
    }

    inline bool operator== (const struct s_Point &other) {
        return (   (other.x == this->x)
                && (other.y == this->y)
               );
    }
};

typedef struct s_Point t_Point;

enum {
    Down = 0,
    Middle = 1,
    Up = 2
};

enum {
    Left = 0,
    Center = 1,
    Right = 2
};

enum {
    UpLeft    = 0,
    DownLeft  = 1,
    DownRight = 2,
    UpRight   = 3,
};

enum {
    BUpLeft         = 0,
    BLeftUp         = 1,
    BDownLeft       = 2,
    BLeftDown       = 3,
    BDownRight      = 4,
    BRightDown      = 5,
    BRightUp        = 6,
    BUpRight        = 7,

    JoinUp          = 8,
    JoinLeft        = 9,
    JoinDown        = 10,
    JoinRight       = 11,

    CenterUpLeft    = 12,
    CenterDownLeft  = 13,
    CenterDownRight = 14,
    CenterUpRight   = 15,

    OriginUp        = 16,
    OriginLeft      = 17,
    OriginDown      = 18,
    OriginRight     = 19,
    OriginCenter    = 20,
    OriginUpLeft    = 21,
    OriginDownLeft  = 22,
    OriginDownRight = 23,
    OriginUpRight   = 24
};

typedef struct		s_t_Quad
{
    t_Point	points[4];
    t_Point	texture[4];
    GLfloat	alpha[4];

    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat	Smooth;
    GLfloat	SmoothLen;
}								t_Quad;

typedef struct		s_PillowGraphy
{
    //t_Quad  quads[4];
    t_Point	points[3][3];
    t_Point	texture[3][3];
    GLfloat	alpha[3][3];

    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat	Smooth;
    GLfloat	SmoothLen;
} PillowGraphy;


#define BETWEEN(min, val, max) ((((val) >= (min)) && ((val) <= (max)))?1:0)

//#define DIST(ax, ay, bx, by) sqrt(((ax) - (bx)) * ((ax) - (bx)) + ((ay) - (by)) * ((ay) - (by)))
#define DIST(a, b) sqrt(((a.x) - (b.x)) * ((a.x) - (b.x)) + ((a.y) - (b.y)) * ((a.y) - (b.y)))


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
    GLWidget(std::map<std::string, std::string> configuration);
    ~GLWidget();

    bool inEditMode() {return editMode;}
    void toggleEditMode();
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
    void updateTextureFromSharedMemory();
    void drawQuad(double coords[4][2], double textures[4][2], int level = 1);

    void resizeGL(int width, int height);

private:
    std::map<std::string, std::string> configuration;
    GLdouble clearColorR;
    GLdouble clearColorG;
    GLdouble clearColorB;
    GLdouble clearColorA;
    t_Point lastPos;
    unsigned int texture;

#ifdef WIN32
    LPVOID shm;
#elif __linux__
    void * shm;
#endif
    GLdouble width;
    GLdouble height;

    int windowWidth;
    int windowHeight;

    void    draw           ();
    void    drawCross      (double x, double y, double size, GLuint id, float level, bool active);
    void    drawQuadFdf    (int level, int x, int y, int w, int h);
    void    drawQuadFdfRec ();
    void	drawQuadRec(t_Quad h, int sub, GLuint id, float level, int show_border, int active);
    void    drawQuadFast   ();
    void    drawQuadFdfFast();
    void    calcQuad       ();
    void    calcQuadFdf    ();
    void    drawPillow(PillowGraphy p, int sub, GLuint id, float level, int show_border, int active);
    void    drawEditMode(PillowGraphy p, int sub, GLuint id, float level, int show_border, int active);
    void	drawQuadFdf(t_Quad h, int sub, GLuint id, float level, int active);
    void	calcQuadFdf(t_Quad h, int sub, GLuint id, float level, int active);
    void	calcQuadFdfRec(t_Quad h, int sub, GLuint id, float level, int active);

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
    int                         fps;
    bool                        videoSync;
#endif
    int                         endLoop;
    bool                        editMode;
    int                         recursionLevel;
    int                         quadLevel;
    std::string                     child;
    std::string                     prefix;
    std::string                     title;
    PillowGraphy                pillowModel;
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
