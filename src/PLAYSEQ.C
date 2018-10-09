/*****************************************************************************
* @project N64 Music Teacher
* @info The game for learning music.
* @platform Nintendo 64
* @autor Valery P. (https://github.com/hww)
*****************************************************************************/

/* sequence player test */

#define COMP_SEQ_PLAY

#include <ultra64.h>
#include "playseq.h"
#include "debug.h"

extern u16 mycfb[320 * 240];

int firstframe = 1;

int backwards = 0, forwards = 0;

static OSTask *tlist[2];               /* globaltask list      */
static  void            stopOsc(void *oscState);
static  ALMicroTime     updateOsc(void *oscState, f32 *updateVal);
static  ALMicroTime     initOsc(void **oscState, f32 *initVal, u8 oscType,
    u8 oscRate, u8 oscDepth, u8 oscDelay);

extern OSMesgQueue dmaMessageQ, rspMessageQ, retraceMessageQ;
extern OSMesg dmaMessageBuf[DMA_QUEUE_SIZE], rspMessageBuf, retraceMessageBuf;
extern OSIoMesg dmaIOMessageBuf;



int get_tempo(void);
void set_tempo(int tempo);
int get_chvol(int ch);
void set_chvol(int ch, int vol);
void set_chins(int ch, int ins);
void ins_ch(int ch, int ins);
void accom_on(u8 *panelm);
void accom_off(u8 *panelm);
void ins_shift(int pitch, int instn);

int chvol[16];
int chins[16];
int f_tempo;
int ns_flag;
static int old_pitch = 0;
static int accom = 1;
ALCSeqMarker f_start, n_start, marks[1000];
s32 end_pos;

/*
 * Globals for audio
 */
static ALGlobals       g;
static ALSynConfig     c;
static ALSeqpConfig    seqc;
s32    seqNo;   // composition number (static disabled for visibility in other modules)

static u8              audioHeap[AUDIO_HEAP_SIZE];
static OSIoMesg        dmaIOMesgBuf[DMA_QUEUE_SIZE];
static s32             nextDMA = 0;
static s32             curBuf = 0;
static s32             curAudioBuf = 1;

/*
 * Double-buffered dynamic segments
 */
static Acmd		*cmdList[2];
static s16      audioSamples[3] = { 0, 0, 0 };
static s16      *audioBuffer[3];
static ALHeap   hp;

/*
 * This can usually be reduced - it depends on the sequence
 */

#define         NBUFFERS       96

typedef struct
{
    ALLink      node;
    int         startAddr;
    u32         lastFrame;
    char        *ptr;
} DMABuffer;

typedef struct
{
    u8          initialized;
    DMABuffer   *firstUsed;
    DMABuffer   *firstFree;
} DMAState;

DMAState    dmaState;
DMABuffer   dmaBuffs[NBUFFERS];
u32         gFrameCt;

void CleanDMABuffs(void);
void set_file(void);

s32 dmaCallBack(s32 addr, s32 len, void *state)
{
    void        *freeBuffer;
    int         delta;
    DMABuffer   *dmaPtr, *lastDmaPtr;
    s32         addrEnd, buffEnd;

    lastDmaPtr = 0;
    dmaPtr = dmaState.firstUsed;
    addrEnd = addr + len;

    while (dmaPtr)                                        /* see if buffer is already set up */
    {

        buffEnd = dmaPtr->startAddr + MAX_BUFFER_LENGTH;
        if (dmaPtr->startAddr > addr)                     /* since buffers are ordered */
            break;                                        /* abort if past possible */

        else if (addrEnd <= buffEnd)                      /* yes, found one */
        {
            dmaPtr->lastFrame = gFrameCt;                 /* mark it used */
            freeBuffer = dmaPtr->ptr + addr - dmaPtr->startAddr;
            return (int)osVirtualToPhysical(freeBuffer);
        }
        lastDmaPtr = dmaPtr;
        dmaPtr = (DMABuffer*)dmaPtr->node.next;
    }
    /* get here, and you didn't find a buffer, so dma a new one */
    /* get a buffer from the free list */
    dmaPtr = dmaState.firstFree;

    assert(dmaPtr);                                       /* be sure you have a buffer */

    dmaState.firstFree = (DMABuffer*)dmaPtr->node.next;
    alUnlink((ALLink*)dmaPtr);

    /* add it to the used list */
    if (lastDmaPtr)
    {
        /* normal procedure */
        alLink((ALLink*)dmaPtr, (ALLink*)lastDmaPtr);
    }
    else if (dmaState.firstUsed)
    {
        /* jam at beginning of list */
        lastDmaPtr = dmaState.firstUsed;
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = (ALLink*)lastDmaPtr;
        dmaPtr->node.prev = 0;
        lastDmaPtr->node.prev = (ALLink*)dmaPtr;
    }
    else
    {
        /* no buffers in list, this is the first one */
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = 0;
        dmaPtr->node.prev = 0;
    }

    freeBuffer = dmaPtr->ptr;
    delta = addr & 0x1;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = gFrameCt;  /* mark it */
    osPiStartDma(&dmaIOMesgBuf[nextDMA++], OS_MESG_PRI_NORMAL, OS_READ,
        (u32)addr, freeBuffer, MAX_BUFFER_LENGTH, &dmaMessageQ);

    return (int)osVirtualToPhysical(freeBuffer) + delta;
}

ALDMAproc dmaNew(DMAState **state)
{
    int         i;
    if (!dmaState.initialized)  /* only do this once */
    {
        dmaState.firstFree = &dmaBuffs[0];
        for (i = 0; i < NBUFFERS - 1; i++)
        {
            alLink((ALLink*)&dmaBuffs[i + 1], (ALLink*)&dmaBuffs[i]);
            dmaBuffs[i].ptr = alHeapAlloc(&hp, 1, MAX_BUFFER_LENGTH);
        }

        dmaState.initialized = 1;
    }
    *state = &dmaState;  /* state is never used in this case */
    return dmaCallBack;
}

void CleanDMABuffs(void)
{
    DMABuffer  *dmaPtr, *nextPtr;
    dmaPtr = dmaState.firstUsed;
    while (dmaPtr)
    {
        nextPtr = (DMABuffer*)dmaPtr->node.next;
        /* Can change this value.  Should be at least one.  */
        /* Larger values mean more buffers needed, but fewer DMA's */
        if (dmaPtr->lastFrame + 2 < gFrameCt) /* remove from used list */
        {
            if (dmaState.firstUsed == dmaPtr)
                dmaState.firstUsed = (DMABuffer*)dmaPtr->node.next;
            alUnlink((ALLink*)dmaPtr);
            if (dmaState.firstFree)
                alLink((ALLink*)dmaPtr, (ALLink*)dmaState.firstFree);
            else
            {
                dmaState.firstFree = dmaPtr;
                dmaPtr->node.next = 0;
                dmaPtr->node.prev = 0;
            }
        }
        dmaPtr = nextPtr;
    }
}

#define  TREMELO_SIN        1
#define  TREMELO_SQR        2
#define  TREMELO_DSC_SAW    3
#define  TREMELO_ASC_SAW    4
#define  VIBRATO_SIN        128
#define  VIBRATO_SQR        129
#define  VIBRATO_DSC_SAW    130
#define  VIBRATO_ASC_SAW    131
#define  OSC_HIGH   0
#define  OSC_LOW    1
#define  TWO_PI     6.2831853

typedef struct {
    u8      rate;
    u8      depth;
    u8      oscCount;
} defData;

typedef struct {
    u8      halfdepth;
    u8      baseVol;
} tremSinData;

typedef struct {
    u8      curVal;
    u8      hiVal;
    u8      loVal;
} tremSqrData;

typedef struct {
    u8      baseVol;
    u8      depth;
} tremSawData;

typedef struct {
    f32     depthcents;
} vibSinData;

typedef struct {
    f32     loRatio;
    f32     hiRatio;
} vibSqrData;

typedef struct {
    s32     hicents;
    s32     centsrange;
} vibDSawData;

typedef struct {
    s32     locents;
    s32     centsrange;
} vibASawData;

typedef struct oscData_s {
    struct oscData_s  *next;
    u8      type;
    u8      stateFlags;
    u16     maxCount;
    u16     curCount;
    union {
        defData         def;
        tremSinData     tsin;
        tremSqrData     tsqr;
        tremSawData     tsaw;
        vibSinData      vsin;
        vibSqrData      vsqr;
        vibDSawData     vdsaw;
        vibASawData     vasaw;
    } data;
} oscData;

/*
 * Number of osc states needed. In worst case will need two for each
 * voice. But if tremolo and vibrato not used on all instruments will
 * need less.
 */
#define  OSC_STATE_COUNT    2*MAX_VOICES
oscData  *freeOscStateList;
oscData  oscStates[OSC_STATE_COUNT];

/************************************************************************
 *   _depth2Cents()  convert a u8 (0-255) to a cents value. Convert using
 *   1.03099303^(depth). This gives an exponential range of values from
 *   1 to 2400.  (2400 cents is 2 octaves). Lots of small values for
 *   good control of depth in musical applications, and a couple of
 *   really broad ranges for special effects.
 ************************************************************************/

f32 _depth2Cents(u8 depth)
{
    f32 x = 1.03099303;
    f32 cents = 1.0;
    while (depth)
    {
        if (depth & 1)
            cents *= x;
        x *= x;
        depth >>= 1;
    }
    return(cents);
}

ALMicroTime initOsc(void **oscState, f32 *initVal, u8 oscType,
    u8 oscRate, u8 oscDepth, u8 oscDelay)
{
    oscData         *statePtr;
    ALMicroTime     deltaTime = 0;

    if (freeOscStateList)  /* yes there are oscStates available */
    {
        statePtr = freeOscStateList;
        freeOscStateList = freeOscStateList->next;
        statePtr->type = oscType;
        *oscState = statePtr;
        /*
         * Convert delay into usec's, In this example, multiply by
         * 0x4000, but could easily use another conversion method.
         */
        deltaTime = oscDelay * 0x4000;

        switch (oscType) /* set the initVal */
        {
        case TREMELO_SIN:
            statePtr->curCount = 0;
            statePtr->maxCount = 259 - oscRate; /* gives values 4-259 */
            statePtr->data.tsin.halfdepth = oscDepth >> 1;
            statePtr->data.tsin.baseVol = AL_VOL_FULL - statePtr->data.tsin.halfdepth;
            *initVal = (f32)statePtr->data.tsin.baseVol;
            break;

        case TREMELO_SQR:
            statePtr->maxCount = 256 - oscRate; /* values from 1-256 */
            statePtr->curCount = statePtr->maxCount;
            statePtr->stateFlags = OSC_HIGH;
            statePtr->data.tsqr.loVal = AL_VOL_FULL - oscDepth;
            statePtr->data.tsqr.hiVal = AL_VOL_FULL;
            statePtr->data.tsqr.curVal = AL_VOL_FULL;
            *initVal = (f32)AL_VOL_FULL;
            break;

        case TREMELO_DSC_SAW:
            statePtr->maxCount = 256 - oscRate;
            statePtr->curCount = 0;
            statePtr->data.tsaw.depth = oscDepth;
            statePtr->data.tsaw.baseVol = AL_VOL_FULL;
            *initVal = (f32)statePtr->data.tsaw.baseVol;
            break;

        case TREMELO_ASC_SAW:
            statePtr->maxCount = 256 - oscRate;
            statePtr->curCount = 0;
            statePtr->data.tsaw.depth = oscDepth;
            statePtr->data.tsaw.baseVol = AL_VOL_FULL - oscDepth;
            *initVal = (f32)statePtr->data.tsaw.baseVol;
            break;

        case VIBRATO_SIN:
            statePtr->data.vsin.depthcents = _depth2Cents(oscDepth);
            statePtr->curCount = 0;
            statePtr->maxCount = 259 - oscRate; /* gives values 4-259 */
            *initVal = 1.0f; /* start at unity pitch */
            break;

        case VIBRATO_SQR:
        {
            s32     cents;
            statePtr->maxCount = 256 - oscRate; /* values from 1-256 */
            statePtr->curCount = statePtr->maxCount;
            statePtr->stateFlags = OSC_HIGH;
            cents = _depth2Cents(oscDepth);
            statePtr->data.vsqr.loRatio = alCents2Ratio(-cents);
            statePtr->data.vsqr.hiRatio = alCents2Ratio(cents);
            *initVal = statePtr->data.vsqr.hiRatio;
        }
        break;

        case VIBRATO_DSC_SAW:
        {
            s32     cents;
            statePtr->maxCount = 256 - oscRate; /* values from 1-256 */
            statePtr->curCount = statePtr->maxCount;
            cents = _depth2Cents(oscDepth);
            statePtr->data.vdsaw.hicents = cents;
            statePtr->data.vdsaw.centsrange = 2 * cents;
            *initVal = alCents2Ratio(statePtr->data.vdsaw.hicents);
        }
        break;

        case VIBRATO_ASC_SAW:
        {
            s32     cents;
            statePtr->maxCount = 256 - oscRate; /* values from 1-256 */
            statePtr->curCount = statePtr->maxCount;
            cents = _depth2Cents(oscDepth);
            statePtr->data.vasaw.locents = -cents;
            statePtr->data.vasaw.centsrange = 2 * cents;
            *initVal = alCents2Ratio(statePtr->data.vasaw.locents);
        }
        break;

        }
    }
    return(deltaTime);  /* if there are no oscStates, return zero, but if
                           oscState was available, return delay in usecs */
}

ALMicroTime updateOsc(void *oscState, f32 *updateVal)
{
    f32             tmpFlt;
    oscData         *statePtr = (oscData*)oscState;
    ALMicroTime     deltaTime = AL_USEC_PER_FRAME; /* in this example callback every */
                                              /* frame, but could be at any interval */

    switch (statePtr->type)   /* perform update calculations */
    {
    case TREMELO_SIN:
        statePtr->curCount++;
        if (statePtr->curCount >= statePtr->maxCount)
            statePtr->curCount = 0;
        tmpFlt = (f32)statePtr->curCount / (f32)statePtr->maxCount;
        tmpFlt = sinf(tmpFlt*TWO_PI);
        tmpFlt = tmpFlt * (f32)statePtr->data.tsin.halfdepth;
        *updateVal = (f32)statePtr->data.tsin.baseVol + tmpFlt;
        break;

    case TREMELO_SQR:
        if (statePtr->stateFlags == OSC_HIGH)
        {
            *updateVal = (f32)statePtr->data.tsqr.loVal;
            statePtr->stateFlags = OSC_LOW;
        }
        else
        {
            *updateVal = (f32)statePtr->data.tsqr.hiVal;
            statePtr->stateFlags = OSC_HIGH;
        }
        deltaTime *= statePtr->maxCount;
        break;

    case TREMELO_DSC_SAW:
        statePtr->curCount++;
        if (statePtr->curCount > statePtr->maxCount)
            statePtr->curCount = 0;

        tmpFlt = (f32)statePtr->curCount / (f32)statePtr->maxCount;
        tmpFlt *= (f32)statePtr->data.tsaw.depth;
        *updateVal = (f32)statePtr->data.tsaw.baseVol - tmpFlt;
        break;

    case TREMELO_ASC_SAW:
        statePtr->curCount++;
        if (statePtr->curCount > statePtr->maxCount)
            statePtr->curCount = 0;
        tmpFlt = (f32)statePtr->curCount / (f32)statePtr->maxCount;
        tmpFlt *= (f32)statePtr->data.tsaw.depth;
        *updateVal = (f32)statePtr->data.tsaw.baseVol + tmpFlt;
        break;

    case VIBRATO_SIN:
        /* calculate a sin value (from -1 to 1) and multiply it by depthcents.
           Then convert cents to ratio. */
        statePtr->curCount++;
        if (statePtr->curCount >= statePtr->maxCount)
            statePtr->curCount = 0;
        tmpFlt = (f32)statePtr->curCount / (f32)statePtr->maxCount;
        tmpFlt = sinf(tmpFlt*TWO_PI) * statePtr->data.vsin.depthcents;
        *updateVal = alCents2Ratio((s32)tmpFlt);
        break;

    case VIBRATO_SQR:
        if (statePtr->stateFlags == OSC_HIGH)
        {
            statePtr->stateFlags = OSC_LOW;
            *updateVal = statePtr->data.vsqr.loRatio;
        }
        else
        {
            statePtr->stateFlags = OSC_HIGH;
            *updateVal = statePtr->data.vsqr.hiRatio;
        }
        deltaTime *= statePtr->maxCount;
        break;

    case VIBRATO_DSC_SAW:
        statePtr->curCount++;
        if (statePtr->curCount > statePtr->maxCount)
            statePtr->curCount = 0;
        tmpFlt = (f32)statePtr->curCount / (f32)statePtr->maxCount;
        tmpFlt *= (f32)statePtr->data.vdsaw.centsrange;
        tmpFlt = (f32)statePtr->data.vdsaw.hicents - tmpFlt;
        *updateVal = alCents2Ratio((s32)tmpFlt);
        break;

    case VIBRATO_ASC_SAW:
        statePtr->curCount++;
        if (statePtr->curCount > statePtr->maxCount)
            statePtr->curCount = 0;
        tmpFlt = (f32)statePtr->curCount / (f32)statePtr->maxCount;
        tmpFlt *= (f32)statePtr->data.vasaw.centsrange;
        tmpFlt += (f32)statePtr->data.vasaw.locents;
        *updateVal = alCents2Ratio((s32)tmpFlt);
        break;
    }

    return(deltaTime);
}

void stopOsc(void *oscState)
{
    ((oscData*)oscState)->next = freeOscStateList;
    freeOscStateList = (oscData*)oscState;
}

long asdfg = 0;

#ifdef COMP_SEQ_PLAY
static    ALCSPlayer
sequencePlayer,
*seqp = &sequencePlayer;

ALCSeq
sequence,
*seq = &sequence;
#else

static    ALSeqPlayer
sequencePlayer,
*seqp = &sequencePlayer;

static    ALSeq
sequence,
*seq = &sequence;
#endif

static    ALSeqFile
*sfile;

static    ALBank
*midiBank = 0;

static    Acmd
*cmdlp;

static    OSTask
*tlistp;

static    f32
fsize;

static    s32
samplesLeft = 0,
clcount,
len,
bankLen,
buf,
frame = 0,
i,
frameSize,
minFrameSize;

static    s16
*audioOp;

static    u8
*ptr,
*seqPtr,
*midiBankPtr;

static    OSMesgQueue
seqMessageQ;

static    OSMesg
seqMessageBuf;

static    OSIoMesg
seqIOMessageBuf;

static    oscData
*oscStatePtr;

void open_midi(void)
{
    /*
     * Audio heap
     */
     //InitDebug();
    alHeapInit(&hp, (u8 *)audioHeap, AUDIO_HEAP_SIZE);

    /*
     * Allocate storage for command list and task headers
     */
    cmdList[0] = alHeapAlloc(&hp, 1, MAX_CLIST_SIZE * sizeof(Acmd));
    cmdList[1] = alHeapAlloc(&hp, 1, MAX_CLIST_SIZE * sizeof(Acmd));
    tlist[0] = alHeapAlloc(&hp, 1, sizeof(OSTask));
    tlist[1] = alHeapAlloc(&hp, 1, sizeof(OSTask));
    audioBuffer[0] = alHeapAlloc(&hp, 1, sizeof(s32)*MAX_AUDIO_LENGTH);
    audioBuffer[1] = alHeapAlloc(&hp, 1, sizeof(s32)*MAX_AUDIO_LENGTH);
    audioBuffer[2] = alHeapAlloc(&hp, 1, sizeof(s32)*MAX_AUDIO_LENGTH);
    /***********************************************
     * MIDI bank is CTL file Samples header
     ***********************************************/
    bankLen = _midibankSegmentRomEnd - _midibankSegmentRomStart;
    midiBankPtr = alHeapAlloc(&hp, 1, bankLen);
    osWritebackDCacheAll();
    osPiStartDma(&dmaIOMessageBuf, OS_MESG_PRI_NORMAL, OS_READ,
        (u32)_midibankSegmentRomStart, midiBankPtr,
        bankLen, &dmaMessageQ);
    (void)osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);
    /**********************************************
     * Copy over the MIDI sequence file header
     **********************************************/
    sfile = alHeapAlloc(&hp, 1, 4);
    osWritebackDCacheAll();
    osPiStartDma(&dmaIOMessageBuf, OS_MESG_PRI_NORMAL, OS_READ,
        (u32)_seqSegmentRomStart, sfile,
        4, &dmaMessageQ);          // 4 первых слова в память
    (void)osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);

    len = 4 + sfile->seqCount * sizeof(ALSeqData);
    sfile = alHeapAlloc(&hp, 1, len);
    osWritebackDCacheAll();
    osPiStartDma(&dmaIOMessageBuf, OS_MESG_PRI_NORMAL, OS_READ,
        (u32)_seqSegmentRomStart, sfile,
        len, &dmaMessageQ);
    (void)osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);

    alSeqFileNew(sfile, (u8 *)_seqSegmentRomStart);
    /*
     * Initialize DAC output rate
     */
    c.outputRate = osAiSetFrequency(OUTPUT_RATE);
    // NUM_FIELDS=1   44100 / 60 -> 735 samples per frame
    fsize = (f32)NUM_FIELDS * c.outputRate / (f32)20; 
    frameSize = (s32)fsize;
    if (frameSize < fsize)
        frameSize++;                // round to most 
    if (frameSize & 0xf)
        frameSize = (frameSize & ~0xf) + 0x10;  // round to 16 bytes
    minFrameSize = frameSize - 16;

    /*
     * Audio synthesizer initialization
     */
    c.maxVVoices = MAX_VOICES;
    c.maxPVoices = MAX_PVOICES;
    c.maxUpdates = MAX_UPDATES;
    c.dmaproc = &dmaNew; // DMA procedure
    c.heap = &hp;
    c.fxType = AL_FX_CUSTOM;

    if (c.fxType == AL_FX_CUSTOM) {
        s32 delay_size = 0;
        /*
         * if you wanted to build an effect, you would specify c.fxType
         * to be AL_FX_CUSTOM, and then allocate and fill in the effect
         * parameters. Some examples follows:
         */
         
#define ms *(((s32) ((f32) OUTPUT_RATE/1000))&~0x7)
#define SECTION_COUNT 8
#define SECTION_SIZE  8

        s32 params[SECTION_COUNT*SECTION_SIZE + 2] = {

            /* sections    total length */
            SECTION_COUNT,   325 ms,
            /*       chorus  chorus   filter
            input  output  fbcoef  ffcoef   gain     rate   depth     coef      */
            325 ms,   0 ms,  8192,  -8192, 0x0000,      0,      0, 0x0000,
            259 ms,   0 ms,  9830,  -9830, 0x0000,      0,      0, 0x0000,
            139 ms,   0 ms, 16384, -16384, 0x0000,      0,      0, 0x0000,
             78 ms,   0 ms,  8192,  -8192, 0x0000,      0,      0, 0x0000,
             39 ms,   0 ms, 16384, -16384, 0x0000,      0,      0, 0x0000,
             18 ms,   0 ms,  9830,  -9830, 0x0000,      0,      0, 0x0000,
              9 ms,   0 ms,  8192,  -8192, 0x0000,      0,      0, 0x0000,
            325 ms,   0 ms,  8192,  -8192, 0x2000,    000,   0000, 0x0000
        };

        c.params = params;

        /*
         * since fx params are only needed in alInit, call from here
         * so stack storage is freed up
         */

        alInit(&g, &c);
    }
    else {
        alInit(&g, &c);

    }
    freeOscStateList = &oscStates[0];
    oscStatePtr = &oscStates[0];
    for (i = 0; i < (OSC_STATE_COUNT - 1); i++)
    {
        oscStatePtr->next = &oscStates[i + 1];
        oscStatePtr = oscStatePtr->next;
    }
    oscStatePtr->next = 0;  /* set last pointer to zero */

    seqc.maxVoices = MAX_VOICES;
    seqc.maxEvents = EVT_COUNT;
    seqc.maxChannels = 16;
    seqc.heap = &hp;
    seqc.initOsc = &initOsc;
    seqc.updateOsc = &updateOsc;
    seqc.stopOsc = &stopOsc;

#ifdef _DEBUG
    seqc.debugFlags = NO_VOICE_ERR_MASK | NOTE_OFF_ERR_MASK | NO_SOUND_ERR_MASK;
#endif

#ifdef COMP_SEQ_PLAY
    alCSPNew(seqp, &seqc);
#else
    alSeqpNew(seqp, &seqc);
#endif

    alBnkfNew((ALBankFile *)midiBankPtr, (u8 *)_miditableSegmentRomStart);
    midiBank = ((ALBankFile *)midiBankPtr)->bankArray[0];

    // EPrint("Keymin=%d\n",midiBank->instArray[0]->soundArray[0]->keyMap->keyMin);
    // EPrint("Keymax=%d\n",midiBank->instArray[0]->soundArray[0]->keyMap->keyMax);
    // EPrint("Keybase=%d\n",midiBank->instArray[0]->soundArray[0]->keyMap->keyBase);


#ifdef COMP_SEQ_PLAY
    alCSPSetBank(seqp, midiBank);
#else
    alSeqpSetBank(seqp, midiBank);
#endif

   /*
    * Allocate storage for sequence
    */
    seqPtr = alHeapAlloc(&hp, 1, MAX_SEQ_LENGTH);
    set_file();
}

void set_file(void) {  // seqNo - File number
    int i;
    ALEvent event;

    //EPrint("1\n");
    if (seqNo >= sfile->seqCount || seqNo < 0) seqNo = 0;
    //EPrint("2\n");
    ptr = sfile->seqArray[seqNo].offset;
    len = sfile->seqArray[seqNo].len;
    if (len & 0x1) len++;
    //EPrint("3\n");
    osInvalDCache(seqPtr, len);
    osPiStartDma(&dmaIOMessageBuf, OS_MESG_PRI_NORMAL, OS_READ,
        (u32)ptr, seqPtr, len, &dmaMessageQ);
    //EPrint("4\n");
    (void)osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);
    //EPrint("5\n");

    alCSeqNew(seq, seqPtr);
    alCSPSetSeq(seqp, seq);
    alCSeqNewMarker(seq, &f_start, alCSeqGetTicks(seq));

    for (i = 0; i < 16; i++)
    {
        chvol[i] = 0;
        chins[i] = 0;
    }
    f_tempo = ns_flag = 0;

    do
    {
        alCSeqNextEvent(seq, &event);
        //if (!((event.type == AL_SEQ_MIDI_EVT)&&
            //   ((event.msg.midi.status&0xF0) == AL_MIDI_NoteOn)))
            //   EPrint("Type=%2X  Status=%2X  Byte1=%d  Byte2=%d\n",event.type,event.msg.midi.status,event.msg.midi.byte1,event.msg.midi.byte2);
                        //Сохраним темп
        if ((event.type == AL_TEMPO_EVT) && (f_tempo == 0))
            f_tempo = (int)((s32)60000000 / (s32)(65536 * (s32)event.msg.tempo.byte1 + 256 * (s32)event.msg.tempo.byte2 + (s32)event.msg.tempo.byte3));

        //Сохраним громкость всех каналов
        if ((event.type == AL_SEQ_MIDI_EVT) &&
            ((event.msg.midi.status & 0xf0) == AL_MIDI_ControlChange) &&
            (event.msg.midi.byte1 == AL_MIDI_VOLUME_CTRL))
        {
            if (chvol[event.msg.midi.status & 0x0f] == 0)
                chvol[event.msg.midi.status & 0x0f] = (int)event.msg.midi.byte2;
        }

        //Сохраним инструменты всех каналов
        if ((event.type == AL_SEQ_MIDI_EVT) &&
            ((event.msg.midi.status & 0xf0) == AL_MIDI_ProgramChange))
        {
            if (chins[(event.msg.midi.status & 0x0f)] == 0)
                chins[(event.msg.midi.status & 0x0f)] = (int)event.msg.midi.byte1;
        }
        //Исполним MIDI команду в offline
        if ((event.type == AL_SEQ_MIDI_EVT) && (event.msg.midi.status != 0xC9))
        {
            alCSPSendMidi(seqp, alCSeqGetTicks(seq), event.msg.midi.status, event.msg.midi.byte1, event.msg.midi.byte2);
        }

        //Найдем первую ноту
        if ((event.type == AL_SEQ_MIDI_EVT) &&
            ((event.msg.midi.status & 0xF0) == AL_MIDI_NoteOn) && (ns_flag == 0))
        {
            alCSeqNewMarker(seq, &n_start, alCSeqGetTicks(seq));
            ns_flag = 1;
            break;
        }
        //     EPrint("Event=%d Status=%2X Flag=%d\n",event.type,event.msg.midi.status,ns_flag);

    } while (event.type != AL_SEQ_END_EVT);
    //Встаем на первую ноту
    alCSeqSetLoc(seq, &n_start);
    set_tempo(f_tempo);
    alCSPSetChlPriority(seqp, 14, 127);
    alCSPSetChlPriority(seqp, 15, 126);
    for (i = 0; i < 14; i++) alCSPSetChlPriority(seqp, i, 120 - i * 5);
}

void play_midi(void) {

#ifdef COMP_SEQ_PLAY
    alCSPPlay(seqp);
#else
    alSeqpPlay(seqp);
#endif
    frame = 0;
    /*
     * Sync up on vertical retrace - read more than 1 to be sure!
     */
     //(void)osRecvMesg(&retraceMessageQ, NULL, OS_MESG_BLOCK);
     //(void)osRecvMesg(&retraceMessageQ, NULL, OS_MESG_BLOCK);
}

int get_tempo(void)
{
    return((int)(s32)60000000 / alCSPGetTempo(seqp));
}

void set_tempo(int tempo)
{
    alCSPSetTempo(seqp, (s32)(60000000) / (s32)tempo);
}

int get_chvol(int ch)
{
    return(chvol[ch]);
}

void set_chvol(int ch, int vol)
{
    alCSPSetChlVol(seqp, (u8)ch, (u8)vol);
}

int playing(void)
{
    return(alCSeqGetTicks(seq) < end_pos);
}

void set_chins(int ch, int ins)
{
    alCSPSetChlProgram(seqp, (u8)ch, (s32)ins);
}

void ins_ch(int ch, int ins)
{
    ch &= 1;
    switch (ins)
    {
    case 2: //M_VOICE
        set_chins(14 + ch, 0);
        set_chvol(14 + ch, chvol[14 + ch]);
        set_chvol(ch, 0);
        break;
    case 4: //M_ROYAL
        if (accom)
        {
            set_chvol(ch, chvol[ch]);
            set_chvol(14 + ch, 0);
        }
        else
        {
            set_chins(14 + ch, 1);
            set_chvol(14 + ch, chvol[14 + ch]);
            set_chvol(ch, 0);
        }
        break;
    case 6: //M_KREST
        set_chvol(14 + ch, 0);
        set_chvol(ch, 0);
        break;
    }
}

void accom_on(u8 *panelm)
{
    int i;
    for (i = 2; i < 14; i++) set_chvol(i, chvol[i]);
    accom = 1;
    ins_ch(0, panelm[2]);
    ins_ch(1, panelm[3]);
}

void accom_off(u8 *panelm)
{
    int i;
    for (i = 2; i < 14; i++) set_chvol(i, 0);
    accom = 0;
    ins_ch(0, panelm[2]);
    ins_ch(1, panelm[3]);
}

void solf_shift(int pitch)
{
    int ii, ix, fl;
    ins_shift(pitch, 0);
    ins_shift(pitch, 1);
    for (ii = 0; ii < 14; ii++)
    {
        fl = 0;
        if ((ii == 9) || (chins[ii] == 0) || (chins[ii] == 1)) continue;
        if (ii > 0)
            for (ix = 0; ix < ii; ix++)
            {
                if (chins[ii] == chins[ix])
                {
                    fl = 1;
                    break;
                }
            }
        if (fl == 0) ins_shift(pitch, chins[ii]);
    }
    //if (chins[0] != chins[1]) ins_shift(pitch,chins[1]);
    old_pitch = pitch;
}

void ins_shift(int pitch, int instn)
{
    int p1, p2, p3;
    //EPrint("Instn=%d\n",instn);
    for (i = 0; i < midiBank->instArray[instn]->soundCount; i++)
    {
        if (instn == 0)
        {
            p1 = (midiBank->instArray[instn]->soundArray[i]->keyMap->keyMax -= (pitch - old_pitch));
            p2 = (midiBank->instArray[instn]->soundArray[i]->keyMap->keyMin -= (pitch - old_pitch));
        }
        p3 = (midiBank->instArray[instn]->soundArray[i]->keyMap->keyBase -= (pitch - old_pitch));
        //EPrint("Max=%d, min=%d, base=%d\n",p1,p2,p3);
    }
}

void audio_update(void) {

    /*
     * Note that this must be a do-while in order for seqp's state to
     * get updated during the alAudioFrame processing.
     */
    frame++;
    /*
     * Where the task list goes in DRAM
     */
    tlistp = tlist[curBuf];
    cmdlp = cmdList[curBuf];

    /*
     * Where the audio goes in DRAM
     */
    buf = curAudioBuf % 3;
    audioOp = (s16 *)osVirtualToPhysical(audioBuffer[buf]);
    audioSamples[buf] = 16 + (frameSize - samplesLeft + EXTRA_SAMPLES) & ~0xf;
    if (audioSamples[buf] < minFrameSize)
        audioSamples[buf] = minFrameSize;

    /*
     * Call the frame handler
     * Вот этой командой весь комманд лист и создаёться
     */

    cmdlp = alAudioFrame(cmdlp, &clcount, audioOp, audioSamples[buf]);

    /*
     * Build the audio task
     */
    tlistp->t.type = M_AUDTASK;
    tlistp->t.flags = 0x0;
    tlistp->t.ucode_boot = (u64 *)rspbootTextStart;
    tlistp->t.ucode_boot_size = ((s32)rspbootTextEnd -
        (s32)rspbootTextStart);
    tlistp->t.ucode = (u64 *)aspMainTextStart;
    tlistp->t.ucode_data = (u64 *)aspMainDataStart;
    tlistp->t.ucode_size = 4096;
    tlistp->t.ucode_data_size = sizeof(u64)*(aspMainDataEnd - aspMainDataStart);
    tlistp->t.data_ptr = (u64 *)cmdList[curBuf];
    tlistp->t.data_size = (cmdlp - cmdList[curBuf]) * sizeof(Acmd);

    /*
     * Video does nothing - just syncs up on the frame boundary.
     */
     //(void)osRecvMesg(&retraceMessageQ, NULL, OS_MESG_BLOCK);

     /*
      * Find out how many samples left in the currently running
      * audio buffer
      */
    samplesLeft = IO_READ(AI_LEN_REG) >> 2;

    /*
     * The last task should have finished before the frame message
     * so this just clears the message queue
     */
     //(void)osRecvMesg(&rspMessageQ, NULL, OS_MESG_BLOCK);
     /*
      * Point the DAC at the next buffer
      */
    buf = (curAudioBuf - 1) % 3;

    osAiSetNextBuffer(audioBuffer[buf], audioSamples[buf] << 2);

    for (i = 0; i < nextDMA; i++)
        if (osRecvMesg(&dmaMessageQ, NULL, OS_MESG_NOBLOCK) == -1);

    // DMA not done

   /*
    * Flush the cache and start task on RSP
    */
    osWritebackDCacheAll();
    osSpTaskStart(tlistp);

    CleanDMABuffs();
    /*
     * Swap buffers for wavetable storage and output
     */
    gFrameCt++;
    curBuf ^= 1;
    curAudioBuf++;
    nextDMA = 0;
    (void)osRecvMesg(&retraceMessageQ, NULL, OS_MESG_BLOCK);
    (void)osRecvMesg(&retraceMessageQ, NULL, OS_MESG_BLOCK);
    (void)osRecvMesg(&rspMessageQ, NULL, OS_MESG_BLOCK);
}

void stop_midi(void)
{

#ifdef COMP_SEQ_PLAY
    alCSPStop(seqp);
#else
    alSeqpStop(seqp);
#endif
    frame = 0;
}

void close_midi(void)
{
    alClose(&g);
}

s32 getpos_midi(void)
{
    return  alCSeqGetTicks(seq);
}

// Variable for KARAOKE module
static  u16     markp, karap, big_note;
static  ALEvent event, *event_ptr = &event;
static s8   note;
static s32  pos, dur;
karatxt kara[2000];     // array by 2000 slogs
u8 marksp[1000];        // pointer to marks

kara_cfg kcnf;

// Get next
void get_event(void)
{
    do
    {
        alCSeqNextEvent(seq, &event);
        //EPrint("Event=%d\n",event.type);
        if ((event.type == AL_SEQ_MIDI_EVT)
            && (event.msg.midi.status == (AL_MIDI_NoteOn + kcnf.chanel)))
            break;
    } while (event.type != AL_SEQ_END_EVT);

    note = (s8)event.msg.midi.byte1;
    dur = event.msg.midi.duration;
    pos = getpos_midi();
    //EPrint("pos:%d dur:%d note:%d\n",pos,dur,note);
    //pos = (( pos + (kcnf.precision >> 1)) / kcnf.precision) * kcnf.precision;
    //dur = (( dur + (kcnf.precision >> 1)) / kcnf.precision) * kcnf.precision;
    //EPrint("pos:%d dur:%d note:%d\n\n",pos,dur,note);
    note -= kcnf.base_note;
    while (note < 7) note += 12;
    while (note >= 29) note -= 12;
    if (event.type == AL_SEQ_END_EVT)
    {
        note = KEOF;
        end_pos = alCSeqGetTicks(seq);
        //EPrint("End pos=%d\n",end_pos);
    }
    //Print("General %d %d %d\n",pos,dur,note);
}

void note_write(s8 nota, s32 position, s32 duration)
{
    //EPrint("pos:%d dur:%d note:%d \n",position,duration,nota);
    kara[karap].pos = position;
    kara[karap].note = nota;
    kara[karap].dur = duration;
    karap++;
    //alCSeqNewMarker(seq, &marks[markp++], alCSeqGetTicks(seq));
}

void get_allevent(void)
{
    s32 tpos, tdur, npos, cheap;
    s8 tnote;
    get_event();
    tpos = pos; tnote = note; tdur = dur;
    karap = 0;
    do
    {
        //EPrint("dur %d\n",dur);
        if (tnote == KEOF)
        {
            //EPrint("EOF %d \n", tnote);
            note_write(tnote, tpos, (s32)0); // writen previous note
            return;
        }
        get_event();
        if ((tpos + tdur) > pos)
            tdur = pos - tpos; // cut length notes
        //Print("TP:%d n:%d d:%d\n",tpos,tnote,tdur);
        //Print("SP:%d n:%d d:%d\n",pos,note,dur);
        npos = ((tpos / kcnf.base_kara) + 1) * kcnf.base_kara; // MIDI ticks to one karaoke step
        //Print("NPos1:%d\n",npos);
        // npos is next big note pos
        while (npos < (tpos + tdur))
        {
            cheap = npos - tpos;          // length to next big note
            note_write(tnote, tpos, cheap); // written previous note
            tnote = KMINUS;
            tdur -= cheap;
            tpos = npos;
            npos += kcnf.base_kara;
        }

        note_write(tnote, tpos, tdur);      //  previous note

        // npos is next big note position
        npos = ((tpos / kcnf.base_kara) + 1) * kcnf.base_kara;
        //Print("NPos2:%d\n",npos);

        while (npos < pos)
        {
            //Print("2\n");
            cheap = pos - npos; // cheap is length to next note
            if (cheap > kcnf.base_kara)
            {
                note_write(KNOP, npos, kcnf.base_kara); // "'"
            }
            else
            {
                note_write(KNOP, npos, cheap); // "'"
            }
            npos += kcnf.base_kara;
        }
        tpos = pos; tnote = note; tdur = dur;
    } while (1);
}

void time_norm(void)
{
    s32 min_dur;  // minimum of duration
    u16 xp;
    karap = 0;
    while (kara[karap].note != KEOF)
    {
        // ya dobavil etot cusok dlya vikidivaniya lishih zapyatih
        if (kara[karap + 1].note >= KCHAR_H)
            kara[karap].dur = kara[karap + 1].pos - kara[karap].pos;
        min_dur = kara[karap].dur; // Min duration ot this note
        //EPrint("1\n");
        xp = 1;
        while (kara[karap + xp].note < KCHAR_H)
        {
            if (min_dur > kara[karap + xp].dur) min_dur = kara[karap + xp].dur;
            xp++;
        }

        do
        {
            //EPrint("%d %d %d\n",karap,kara[karap].pos,kara[karap+1].pos);
            kara[karap].com = (kara[karap + 1].pos - kara[karap].pos) / min_dur;
            kara[karap].dur = min_dur;
            karap++;
        } while (kara[karap].note < KCHAR_H);
    }
}

void note_norm(void)
{
    u8 x;
    x = 0;
    karap = 0;
    marksp[x++] = karap;
    alCSeqNewMarker(seq, &marks[karap], kara[karap].pos);
    do
    {
        if (((kara[karap].pos % (kcnf.base_kara * kcnf.size)) == 0) && (karap != 0))
        {
            marksp[x++] = karap;
            alCSeqNewMarker(seq, &marks[karap], kara[karap].pos);
        }
        if ((kara[karap].note < KCHAR_H) && ((kara[karap].pos % kcnf.base_kara) <= kcnf.precision))
            kara[karap].note += KCHAR_H;        // Hight char
        karap++;
    } while (kara[karap].note != KEOF);
    marksp[x++] = karap;
    marksp[x++] = 0xFF;
    marksp[x++] = 0xFF;
    marksp[x++] = 0xFF;
    marksp[x++] = 0xFF;
    //EPrint("Karap=%d\n",karap);
}

typedef char line[5];
static line  kstrings[80] =
{
  "_do","_tu","_re","_mo","_mi","_fa","_zu","_sol","_lo","_la","_cu","_si",
  "do","tu","re","mo","mi","fa","zu","sol","lo","la","cu","si",
  "^do","^tu","^re","^mo","^mi","^fa","^zu","^sol","^lo","^la","^cu","^si",
  " _Do"," _Tu"," _Re"," _Mo"," _Mi"," _Fa"," _Zu"," _So"," _Lo"," _La"," _Cu"," _Si",
  " Do"," Tu"," Re"," Mo"," Mi"," Fa"," Zu"," Sol"," Lo"," La"," Cu"," Si",
  " ^Do"," ^Tu"," ^Re"," ^Mo"," ^Mi"," ^Fa"," ^Zu"," ^So"," ^Lo"," ^La"," ^Cu"," ^Si",
  "-","'",",","?","0"
};

void print_karaoke(void)
{
    u8 n, l;
    s32 tmp;
    karap = 0;
    l = 1;
    while (kara[karap].note != KEOF)
    {
        tmp = kara[karap].dur * kara[karap].com;
        //Print("%s",kstrings[kara[karap].note]);
        //Print("%d [%d %d]",kara[karap].pos,kara[karap].dur, kara[karap].com);
        n = kara[karap].com;
        while (n > 1)
        {
            //Print(",");
            n--;
        }

        karap++;
        if (karap == marksp[l])
        {
            //EPrint("\n");
            l++;
        }
    }
}

void make_karaoke(void)
{
    // Begin of karaoke module 
    markp = 0;
    //EPrint("MakeKARA\n");
    get_allevent();
    //EPrint("events Ok\n");
    note_norm();
    //EPrint("notes OK\n");
    time_norm();
    //EPrint("time OK\n");
    //print_karaoke();
    alCSeqSetLoc(seq, &n_start);
    //EPrint("\nEnding\n");
}