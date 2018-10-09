/*****************************************************************************
* @project N64 Music Teacher
* @info The game for learning music.
* @platform Nintendo 64
* @autor Valery P. (https://github.com/hww)
*****************************************************************************/

/*
 * This file holds display list segments that are 'static' data.
 */

#include <ultra64.h>
#include "main.h"
#include "static.h"

static Vp vp = {
        SCREEN_WD * 2, SCREEN_HT * 2, G_MAXZ / 2, 0, /* scale */
        SCREEN_WD * 2, SCREEN_HT * 2, G_MAXZ / 2, 0, /* translate */
};

Gfx init_dl[] = {
    /* init SP */
    gsSPViewport(&vp),
    gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH |
                          G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD),
    gsSPTexture(0, 0, 0, 0, G_OFF),
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
    gsSPClipRatio(FRUSTRATIO_2),

    /* init DP */
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPPipelineMode(G_PM_NPRIMITIVE),
    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD, SCREEN_HT),
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetTextureDetail(G_TD_CLAMP),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetTextureConvert(G_TC_FILT),
    //gsSPSetLights1(thelight),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetCombineKey(G_CK_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetColorDither(G_CD_BAYER),
    gsDPSetAlphaDither(G_AD_NOISE),
    //gsDPPipeSync(),
    gsSPEndDisplayList()
};

Gfx clearcfb_dl[] = {
    gsDPSetCycleType(G_CYC_FILL),
    gsDPSetFillColor(GPACK_RGBA5551(0,0,0,1) << 16 |
                     GPACK_RGBA5551(0,0,0,1)),
    gsDPFillRectangle(0, 0, SCREEN_WD - 1, SCREEN_HT - 1),
       // gsDPPipeSync(),
        gsSPEndDisplayList()
};

Gfx clearzbuffer_dl[] = {
    gsDPSetScissor(G_SC_NON_INTERLACE,0,0,SCREEN_WD,SCREEN_HT),
    gsDPSetDepthImage(OS_K0_TO_PHYSICAL(zbuffer)),
    gsDPSetCycleType(G_CYC_FILL),
    gsDPSetColorImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
                      OS_K0_TO_PHYSICAL(zbuffer)),
    gsDPSetFillColor(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)),
    gsDPFillRectangle(0, 0, SCREEN_WD - 1, SCREEN_HT - 1),
    //gsDPPipeSync(),
    gsSPEndDisplayList()
};


