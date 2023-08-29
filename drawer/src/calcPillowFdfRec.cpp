#include "glwidget.h"

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

