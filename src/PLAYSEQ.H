/*****************************************************************************
* @project N64 Music Teacher
* @info The game for learning music.
* @platform Nintendo 64
* @autor Valery P. (https://github.com/hww)
*****************************************************************************/

#define	STACKSIZE	  	  0x8000
#define MAX_VOICES        64		//64
#define MAX_PVOICES       32		//32
#define MAX_BUFFER_LENGTH 2048		//2048
#define DMA_QUEUE_SIZE    50		//50
#define MAX_UPDATES       128
#define EVT_COUNT         128		//128
#define FX_BUFFER_SIZE    8192
#define AUDIO_HEAP_SIZE   700000
#define MAX_CLIST_SIZE    8000
#define MAX_AUDIO_LENGTH  0x4000	//4096
#define OUTPUT_RATE       32000		//44100
#define EXTRA_SAMPLES     80    	//80    /* Extra samples to allow for jitter */
#define NUM_FIELDS        1
#define MAX_SEQ_LENGTH    50000

/*
 * Symbol generated by "makerom" to indicate the end of the code segment
 * in virtual (and physical) memory
 */
extern u8 _codeSegmentEnd[];

/*
 * Symbols generated by "makerom" to tell us where the segments are
 * in ROM.
 */
extern u8 _seqSegmentRomStart[], _seqSegmentRomEnd[];
extern u8 _midibankSegmentRomStart[], _midibankSegmentRomEnd[];
extern u8 _miditableSegmentRomStart[], _miditableSegmentRomEnd[];

void open_midi(void);
void stop_midi(void);
void play_midi(void);
void audio_update(void);
s32 getpos_midi(void);
void make_karaoke(void);
void midi_prg(u8 chan, s32 prog);
void midi_vol(u8 chan, u8 vol);
int playing(void);

typedef struct
{
    u8 chanel;
    s8 base_note;
    s16 precision;
    s16 base_kara;
    s8  size;
} kara_cfg;

#define  KCHAR_L  (s8)0
#define  KCHAR_H  (s8)36
#define  KMINUS   (s8)72
#define  KNOP     (s8)73
#define  KCOMON   (s8)74
#define  KBAD     (s8)75
#define  KEOF     (s8)76

typedef struct
{
    s8  note;
    s8  com;
    s32 pos;
    s32 dur;
} karatxt;