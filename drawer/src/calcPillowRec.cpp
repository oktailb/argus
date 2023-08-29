#include "glwidget.h"
#include "types.h"

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
