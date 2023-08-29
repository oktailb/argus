#pragma once
#include <GL/gl.h>

/**
 * @struct s_argusExchange
 * @brief simple data map to handle the Argus SHM data exchanges
 * @param width
 * @param height
 * @param size
 * @param inWrite kind of basic mutex to avoid image flickering due to re-writing avec acquisition
 */
typedef struct s_argusExchange {
    int width;
    int height;
    int size;
    bool firstBufferInWrite;
} t_argusExchange;


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
    t_Point	points[3][3];
    t_Point	texture[3][3];
    GLfloat	alpha[3][3];

    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat	Smooth;
    GLfloat	SmoothLen;
} PillowGraphy;
