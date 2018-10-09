/*****************************************************************************
* @project N64 Music Teacher
* @info The game for learning music.
* @platform Nintendo 64
* @autor Valery P. (https://github.com/hww)
*****************************************************************************/

#include <ultra64.h>

/* app specific includes */
#include "boot.h"

extern OSMesgQueue  n_siMessageQ;
extern OSPfs        pfs[MAXCONTROLLERS];

OSContStatus  statusdata[MAXCONTROLLERS];
OSContPad     dummycontrollerdata = { 0, 0, 0, 0 };
OSContPad     controllerdata[MAXCONTROLLERS];
OSContPad     *validcontrollerdata[MAXCONTROLLERS];
      int     activeControllers[MAXCONTROLLERS];
      int     numControllers = 0;

/*
 * Return how many controllers are connected
 * if there are more than <maxcontrollers> connected, return maxcontrollers
 * (ie specify how many controllers you want with maxcontrollers, and
 *  the return result is the number of controllers actually hooked up)
 */
void initControllers(void)
{
    int i;
    u8  pattern;

    osContInit(&n_siMessageQ, &pattern, statusdata);
}

u32 initRumblePack(int contno)
{
    //  return osMotorInit(&n_siMessageQ, &pfs[contno], contno);
}

/*
 * return pointer to controller data for each connected controller
 * oneshot = which buttons to treat as one-shots ("fire" buttons)
 * oneshot is any of the button macros (eg CONT_B, CONT_LEFT) ored together)
 */
void ReadController(void)
{
    //  MotorSiGetAccess();
    osContStartReadData(&n_siMessageQ);
    osWritebackDCacheAll();
    osRecvMesg(&n_siMessageQ, NULL, OS_MESG_BLOCK);
    //  MotorSiRelAccess();
    osContGetReadData(controllerdata);
}
