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

/*
 * RSP view of the frame buffer.  It exists to create an RSP address for
 * the frame buffer, which is remapped on each frame to either of two
 * regions of physical memory that store that actual bits.
 */

#include <ultra64.h>
#include "main.h"

u32    _cfb[2][SCREEN_WD * SCREEN_HT];




