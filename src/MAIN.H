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

#include "math.h"

#define TXTBAR_PART_MENU        0
#define TXTBAR_PART_3D          1
#define TXTBAR_PART_MOTOR       2
#define TXTBAR_PART_SKULL       3
#define TXTBAR_PART_POM         4


#define STATIC_SEGMENT  1


#define GLIST_LEN       2048*4

#define SCREEN_WD       320
#define SCREEN_HT       240
#define MAX_ENTITIES    20

/*
 * Layout of dynamic data.
 *
 * This structure holds the things which change per frame. It is advantageous
 * to keep dynamic data together so that we may selectively write back dirty
 * data cache lines to DRAM prior to processing by the RCP.
 *
 */

typedef struct {
  Mtx   projection;
  Mtx	viewing;
  Mtx   modeling_translate[MAX_ENTITIES];
  Mtx   modeling_rotate[MAX_ENTITIES][3];
  Gfx	glist[GLIST_LEN];
} dynamic_stuff;


extern u32 _cfb[][SCREEN_WD*SCREEN_HT];
extern unsigned short zbuffer[];

