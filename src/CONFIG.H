/*****************************************************************************
* @project N64 Music Teacher
* @info The game for learning music.
* @platform Nintendo 64
* @autor Valery P. (https://github.com/hww)
*****************************************************************************/

/* Nintendo 64 Karaoke system configuration */

typedef struct {
    char name[16];  // Song name
    int size;       // Quarters in line
    int base1;      // Base tone channel 1
    int base2;      // Base tone channel 2
    int ticks;      // Ticks for quoter
    int chanels;    // Number of tracks
}   files;

#define MAX_PITCH_U 7   // Max tone offset up
#define MAX_PITCH_D -7  // Max tone offset down
#define MAX_TEMPO 250   // Max tempo
#define MIN_TEMPO 40    // Min tempo
#define STP_TEMPO 5     // Tempo step
#define MAX_FILE_SBK 9  // Quantity MIDI files in the SBK
#define LINES 4         // Quantity lines in karaoke text box
#define KARA_CH0 14     // Chanel 0 for karaoke
#define KARA_CH1 15     // Chanel 1 for karaoke

files songs[MAX_FILE_SBK]=
    {   "A1",  4,48,48,192,1,
        "-2",  4,48,48,192,0,
        "A3",  4,48,24,192,1,
        "A4",  4,48,24,192,1,
        "A5",  4,48,24,192,1,
        "A6",  4,48,24,192,1,
        "A7-2",4,48,48,120,2,
        "A8",  4,48,48,192,1,
        "-9",  4,48,24,192,0, };
