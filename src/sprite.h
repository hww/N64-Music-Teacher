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

/* bitmap structs */
/* width, width, hor offset, ver offset, pointer to bm data, actual heigth, TLUT */
#include "logo/DevaN1.h"
#include "logo/DevaS1.h"
#include "logo/Kara2N1.h"
#include "logo/Kara2S1.h"
#include "logo/KaraLN1.h"
#include "logo/KaraLS1.h"
#include "logo/KaraPN1.h"
#include "logo/KaraPS1.h"
#include "logo/KlavaN1.h"
#include "logo/KlavaS1.h"
#include "logo/KnigaN1.h"
#include "logo/KnigaS1.h"
#include "logo/KrestN1.h"
#include "logo/KrestS1.h"
#include "logo/KubN1.h"
#include "logo/KubS1.h"
#include "logo/MetrN1.h"
#include "logo/MetrS1.h"
#include "logo/SvistkiN1.h"
#include "logo/SvistkiS1.h"
#include "logo/polosa.h"

#include "logo/panel1N.h"
#include "logo/panel1S.h"
#include "logo/panel2N.h"
#include "logo/panel2S.h"
#include "logo/panel3N.h"
#include "logo/panel3S.h"
#include "logo/panel4N.h"
#include "logo/panel4S.h"
#include "logo/panel5N.h"
#include "logo/panel5S.h"
#include "logo/panel6N.h"
#include "logo/panel6S.h"
#include "logo/panel7.h"

#include "textbar/ldo.h"
#include "textbar/ltu.h"
#include "textbar/lre.h"
#include "textbar/lmo.h"
#include "textbar/lmi.h"
#include "textbar/lfa.h"
#include "textbar/lzu.h"
#include "textbar/lso.h"
#include "textbar/llo.h"
#include "textbar/lla.h"
#include "textbar/lcu.h"
#include "textbar/lsi.h"

#include "textbar/hdo.h"
#include "textbar/htu.h"
#include "textbar/hre.h"
#include "textbar/hmo.h"
#include "textbar/hmi.h"
#include "textbar/hfa.h"
#include "textbar/hzu.h"
#include "textbar/hso.h"
#include "textbar/hlo.h"
#include "textbar/hla.h"
#include "textbar/hcu.h"
#include "textbar/hsi.h"

#include "textbar/defis.h"
#include "textbar/exflame.h"
#include "textbar/zap.h"
#include "textbar/shtrih.h"
#include "textbar/vopros.h"


Bitmap logo_bitmap[][21]  = {
  { 36,36,0,0,KnigaN1,34,0 },
  { 36,36,0,0,KnigaS1,34,0 },
  { 36,36,0,0,DevaN1,34,0 },
  { 36,36,0,0,DevaS1,34,0 },
  { 36,36,0,0,KlavaN1,34,0 },
  { 36,36,0,0,KlavaS1,34,0 },
  { 36,36,0,0,KrestN1,34,0 },
  { 36,36,0,0,KrestS1,34,0 },
  { 36,36,0,0,KubN1,34,0 },
  { 36,36,0,0,KubS1,34,0 },
  { 36,36,0,0,Kara2N1,34,0 },
  { 36,36,0,0,Kara2S1,34,0 },
  { 36,36,0,0,KaraLN1,34,0 },
  { 36,36,0,0,KaraLS1,34,0 },
  { 36,36,0,0,KaraPN1,34,0 },
  { 36,36,0,0,KaraPS1,34,0 },
  { 36,36,0,0,MetrN1,34,0 },
  { 36,36,0,0,MetrS1,34,0 },
  { 36,36,0,0,SvistkiN1,34,0 },
  { 36,36,0,0,SvistkiS1,34,0 },
  { 34,36,0,0,polosa,34,0 },
};

Bitmap copyr_bitmap[][5]  = {
  { 56,56,0,0,panel1N,12,0 },
  { 56,56,0,0,panel1S,12,0 },
  { 16,16,0,0,panel2N,12,0 },
  { 16,16,0,0,panel2S,12,0 },
  { 16,16,0,0,panel3N,12,0 },
  { 16,16,0,0,panel3S,12,0 },
  { 16,16,0,0,panel4N,12,0 },
  { 16,16,0,0,panel4S,12,0 },
  { 16,16,0,0,panel5N,12,0 },
  { 16,16,0,0,panel5S,12,0 },
  { 16,16,0,0,panel6N,12,0 },
  { 16,16,0,0,panel6S,12,0 },
  { 92,92,0,0,panel7,12,0 },
};



Bitmap kara_bms[] = {
// LOW CHARS AND _____
        { 20, 20, 0, 2, ldo, 27, 0 },
        { 20, 20, 0, 2, ltu, 27, 0 },
        { 20, 20, 0, 2, lre, 27, 0 },
        { 24, 24, 0, 2, lmo, 27, 0 },
        { 24, 24, 0, 2, lmi, 27, 0 },
        { 24, 24, 0, 2, lfa, 27, 0 },
        { 20, 20, 0, 2, lzu, 27, 0 },
        { 36, 36, 0, 2, lso, 27, 0 },
        { 20, 20, 0, 2, llo, 27, 0 },
        { 20, 20, 0, 2, lla, 27, 0 },
        { 20, 20, 0, 2, lcu, 27, 0 },
        { 20, 20, 0, 2, lsi, 27, 0 },
// LOW SIMPLE
        { 20, 20, 0, 2, ldo, 24, 0 },
        { 20, 20, 0, 2, ltu, 24, 0 },
        { 20, 20, 0, 2, lre, 24, 0 },
        { 24, 24, 0, 2, lmo, 24, 0 },
        { 24, 24, 0, 2, lmi, 24, 0 },
        { 24, 24, 0, 2, lfa, 24, 0 },
        { 20, 20, 0, 2, lzu, 24, 0 },
        { 36, 36, 0, 2, lso, 24, 0 },
        { 20, 20, 0, 2, llo, 24, 0 },
        { 20, 20, 0, 2, lla, 24, 0 },
        { 20, 20, 0, 2, lcu, 24, 0 },
        { 20, 20, 0, 2, lsi, 24, 0 },
// LO AND ---
        { 20, 20, 0, 0, ldo, 26, 0 },
        { 20, 20, 0, 0, ltu, 26, 0 },
        { 20, 20, 0, 0, lre, 26, 0 },
        { 24, 24, 0, 0, lmo, 26, 0 },
        { 24, 24, 0, 0, lmi, 26, 0 },
        { 24, 24, 0, 0, lfa, 26, 0 },
        { 20, 20, 0, 0, lzu, 26, 0 },
        { 36, 36, 0, 0, lso, 26, 0 },
        { 20, 20, 0, 0, llo, 26, 0 },
        { 20, 20, 0, 0, lla, 26, 0 },
        { 20, 20, 0, 0, lcu, 26, 0 },
        { 20, 20, 0, 0, lsi, 26, 0 },
// HI CHARS AND ___
        { 24, 24, 0, 2, hdo, 27, 0 },
        { 20, 20, 0, 2, htu, 27, 0 },
        { 20, 20, 0, 2, hre, 27, 0 },
        { 24, 24, 0, 2, hmo, 27, 0 },
        { 24, 24, 0, 2, hmi, 27, 0 },
        { 24, 24, 0, 2, hfa, 27, 0 },
        { 20, 20, 0, 2, hzu, 27, 0 },
        { 40, 40, 0, 2, hso, 27, 0 },
        { 24, 24, 0, 2, hlo, 27, 0 },
        { 20, 20, 0, 2, hla, 27, 0 },
        { 20, 20, 0, 2, hcu, 27, 0 },
        { 24, 24, 0, 2, hsi, 27, 0 },
// HI SIMPLE
        { 24, 24, 0, 2, hdo, 24, 0 },
        { 20, 20, 0, 2, htu, 24, 0 },
        { 20, 20, 0, 2, hre, 24, 0 },
        { 24, 24, 0, 2, hmo, 24, 0 },
        { 24, 24, 0, 2, hmi, 24, 0 },
        { 24, 24, 0, 2, hfa, 24, 0 },
        { 20, 20, 0, 2, hzu, 24, 0 },
        { 40, 40, 0, 2, hso, 24, 0 },
        { 24, 24, 0, 2, hlo, 24, 0 },
        { 20, 20, 0, 2, hla, 24, 0 },
        { 20, 20, 0, 2, hcu, 24, 0 },
        { 24, 24, 0, 2, hsi, 24, 0 },
// HI CHARS AND ----
        { 24, 24, 0, 0, hdo, 26, 0 },
        { 20, 20, 0, 0, htu, 26, 0 },
        { 20, 20, 0, 0, hre, 26, 0 },
        { 24, 24, 0, 0, hmo, 26, 0 },
        { 24, 24, 0, 0, hmi, 26, 0 },
        { 24, 24, 0, 0, hfa, 26, 0 },
        { 20, 20, 0, 0, hzu, 26, 0 },
        { 40, 40, 0, 0, hso, 26, 0 },
        { 24, 24, 0, 0, hlo, 26, 0 },
        { 20, 20, 0, 0, hla, 26, 0 },
        { 20, 20, 0, 0, hcu, 26, 0 },
        { 24, 24, 0, 0, hsi, 26, 0 },
// Other chars      
        { 8, 8, 0, 2, defis,  24, 0 }, // -
        { 4, 4, 0, 2, shtrih, 24, 0 }, // '
        { 4, 4, 0, 2, zap,    24, 0 },  // ,
        {12,12, 0, 2, vopros, 24, 0 }, //?
        { 4, 4, 0, 2, exflame,24, 0 },  //!

        { 0, 0, 0, 0, NULL, 0, 0}
};
