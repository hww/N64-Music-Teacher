/*****************************************************************************
* @project N64 Music Teacher
* @info The game for learning music.
* @platform Nintendo 64
* @autor Valery P. (https://github.com/hww)
*****************************************************************************/

/* Nintendo 64 Karaoke system configuration */
typedef struct {
		char name[16];	//название композиции
		int size;	//Количество четвертей в строке
		int base1;	//базовая нота по каналу 1
		int base2;	//базовая нота по каналу 2
		int ticks;	//количество тиков на четверть
		int chanels;	//количество треков
				}	files;

#define MAX_PITCH_U 7	//Максимальное смещение тональности вверх
#define MAX_PITCH_D -7	//Максимальное смещение тональности вниз
#define MAX_TEMPO 250	//Максимальный темп
#define MIN_TEMPO 40	//Минимальный темп
#define STP_TEMPO 5	//Шаг изменения темпа
#define MAX_FILE_SBK 9  //Количество MIDI файлов в рабочем SBK
#define LINES 4		//Количество строк в выводе караоке
#define KARA_CH0 14 	//Канал 0 для караоке
#define KARA_CH1 15 	//Канал 1 для караоке

files songs[MAX_FILE_SBK]=
	{	"A1",  4,48,48,192,1,
		"-2",  4,48,48,192,0,
		"A3",  4,48,24,192,1,		
		"A4",  4,48,24,192,1,
		"A5",  4,48,24,192,1,
		"A6",  4,48,24,192,1,
		"A7-2",4,48,48,120,2,
		"A8",  4,48,48,192,1,
		"-9",  4,48,24,192,0, };
		           