/**************************************************************************
 *                                                                        *
 *               Copyright (C) 1997, Dynamix, Inc.                        *
 *                                                                        *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Dynamix, Inc.,  and are      *
 *  protected by Federal copyright  law.  They  may not be disclosed to   *
 *  third parties  or copied or duplicated in any form, in whole or       *
 *  in part, without the prior written consent of Dynamix, Inc.           *
 *                                                                        *
 *************************************************************************/

#include <ultra64.h>
#include "main.h"
#include "sprite.h"

#define NUM_template_BMS        (24*40)

static Gfx  sprite_dl[NUM_DL(NUM_template_BMS)];

Sprite default_sprite = {
 0,0,                   /* position: x,y */
 0,0,                   /* sprite size in texels: x,y */
 1.0,1.0,               /* sprite scale: x,y */
 0,0,                   /* explosion: x,y */
 3,                     /* attribute: just copy :) */
 0x1234,                /* sprite depth: z */
 255,255,255,255,       /* sprite coloration RGBA */
 0,0,NULL,              /* color lookup table: start_index, length, address */
 0,1,                   /* sprite bitmap index: start_index, increment */
 1,                     /* number of bitmaps */
 1,                     /* number of dl locations allocated */
 0,0,                   /* sprite bitmap height: used_height, physical height */
 G_IM_FMT_RGBA,         /* sprite bitmap format */
 G_IM_SIZ_16b,          /* sprite bitmap texel size */
 NULL,                  /* pointer to bitmaps */
 sprite_dl,             /* rsp_dl: display list memory */
 NULL,                  /* rsp_next_dl: dynamic_dl pointer */
 0,0                    /* texture fractional offsets */
};

void drawSpriteCol(Gfx **glistp, Bitmap *bm, int x, int y, f32 scale_x, f32 scale_y, int alphabit ,u8 r,u8 g,u8 b)
{
  Sprite *sp;
  Gfx *gxp, *dl;

  gxp = *glistp;
  sp = &default_sprite;

  sp->x         = x;
  sp->y         = y;
  sp->width     = bm->width;
  sp->height    = bm->actualHeight;
  sp->bmheight  = bm->actualHeight;
  sp->bmHreal   = bm->actualHeight;
  sp->bitmap    = bm;
  
  spScale(sp, scale_x, scale_y);
  spColor(sp, r,g,b, alphabit);

  dl = spDraw(sp); 
  gSPDisplayList(gxp++, dl);

  *glistp = gxp;
}

void drawSprite( Gfx **glistp, Bitmap *bm, int x, int y, f32 scale_x, f32 scale_y, int alphabit)
{
        drawSpriteCol(glistp, bm, x, y,  scale_x, scale_y, alphabit ,(u8) 255,(u8) 255,(u8) 255);
}

void SpriteFinish(Gfx **glistp)
{
  Gfx *gxp;
  gxp = *glistp;
  spFinish( &gxp );
  *glistp = (gxp-1);          /* Don't use final EndDisplayList() */
}

void SpriteInit(Gfx **glistp)
{
    Gfx *gxp;

    gxp = *glistp; 
    spInit( &gxp );

    default_sprite.rsp_dl_next = default_sprite.rsp_dl;

    *glistp = gxp;
}


