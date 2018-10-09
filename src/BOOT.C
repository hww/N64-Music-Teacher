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
#include <ramrom.h>
#include "boot.h"

void     boot(void);
static void     idle(void *);
extern void     mainproc(void *);

static u64      bootStack[STACKSIZE / 8];
static u64      idleThreadStack[STACKSIZE / 8];
static u64      mainThreadStack[STACKSIZE / 8];

static OSThread idleThread;
static OSThread mainThread;


static OSMesg           PiMessages[NUM_PI_MSGS];
static OSMesgQueue      PiMessageQ;

/*
 *		Si message queue
 */
OSMesgQueue             n_siMessageQ;
OSMesg                  n_siMessage;

OSMesgQueue             rdpMessageQ;
OSMesg                  rdpMessageBuf;

OSMesgQueue             rspMessageQ;
OSMesg                  rspMessageBuf;

OSMesgQueue             retraceMessageQ;
OSMesg                  retraceMessageBuf;

OSMesgQueue             dmaMessageQ;
OSMesg                  dmaMessageBuf[DMA_QUEUE_SIZE];
OSIoMesg                dmaIOMessageBuf;

/*
 * Boot procedure
 * Creates idleThread and then starts mainproc
 */

void boot(void)
{
    osInitialize();
    osCreateThread(&idleThread, TID_IDLE, idle, (void *)0,
        idleThreadStack + STACKSIZE / sizeof(u64), PRI_IDLE);
    osStartThread(&idleThread);
}

/*
 * Idle procedure
 *
 */

void idle(void *arg)
{
    /* Initialize video */
    osCreateViManager(OS_PRIORITY_VIMGR);
    osViSetMode(&osViModeNtscLpn2);

    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_GAMMA_DITHER_OFF);
    osViBlack(1);

    /* Start PI Mgr for access to cartridge */
    osCreatePiManager((OSPri)OS_PRIORITY_PIMGR, &PiMessageQ, PiMessages,
        NUM_PI_MSGS);

    /* Setup message queues */
    osCreateMesgQueue(&rspMessageQ, &rspMessageBuf, 1);
    osSetEventMesg(OS_EVENT_SP, &rspMessageQ, NULL);

    osCreateMesgQueue(&rdpMessageQ, &rdpMessageBuf, 1);
    osSetEventMesg(OS_EVENT_DP, &rdpMessageQ, NULL);

    osCreateMesgQueue(&retraceMessageQ, &retraceMessageBuf, 1);
    osViSetEvent(&retraceMessageQ, NULL, 1);

    osCreateMesgQueue(&n_siMessageQ, &n_siMessage, 1);
    osSetEventMesg(OS_EVENT_SI, &n_siMessageQ, (OSMesg)1);        /* SI */

    osCreateMesgQueue(&dmaMessageQ, dmaMessageBuf, 1);

    /* Create main thread */
    osCreateThread(&mainThread, TID_MAINPROC, mainproc, (void*)0,
        mainThreadStack + STACKSIZE / sizeof(u64), PRI_MAINPROC);

    /* Start main thread */
    osStartThread(&mainThread);

    /* Become the idle thread */
    osSetThreadPri(0, 0);
    // stop idleThread and call mainproc
    for (;;);
}

