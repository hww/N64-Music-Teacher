/*****************************************************************************
* @project N64 Music Teacher
* @info The game for learning music.
* @platform Nintendo 64
* @autor Valery P. (https://github.com/hww)
*****************************************************************************/

#include <ultra64.h>

 /* Font elements */
#include "textbar/font/part1.h"
#include "textbar/font/part2.h"
#include "textbar/font/part3.h"
#include "textbar/font/part4.h"
#include "textbar/font/part5.h"

static char letters_str[] = "abcdefghijklmn+-opqrstuvwxyz[]:;,./ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?'&()";

#define SPACE                   4
#define BORDER_ALPHA            255
#define BACKGROUND_ALPHA        140
#define INCREASE                1
#define DECREASE                0
#define SHIFT                   0

typedef struct {
	char *txtcurrent;
	char *txtnext;
	char *txtstart;
	u16  curlen;
	u16  nextlen;
	s16  delayval;
	s16  delaycur;
	u8   alphastep;
	s16  alphacurrent;
	u8   alphacurrent_dir;
	s16  alphanext;
	u8   alphanext_dir;
	u16  alphanext_kickin;
} txtBar_t;

static char text_3d[] = "\
'Remis' '1999' entry\n\
Name : Belecky \n\
Date : 28th feb 1998\n\
Size : 16 Mbits\n\
Format : V64/Z64\n\
Country : Europe\n\
Drink : Coca Cola\n\
Sleep : Not enough\n\
....\n\
We included 3 hidden parts\n\
Try to find them all\n\
....\n\
Credits for this demo,\n\
Coding + 3d models : Immortal & Widget\n\
Modplayer : LaC\n\
Intro background : Newt\n\
Testing : Twindianopolis Inc.\n\
Additional help : Locke\n\
....\n\
 \n";

char text_menu[64] = "File\n";


static char text_motor[] =
{
};

static char text_skull[] =
{
};

static char text_pom[] =
{
};


txtBar_t textBar[] = {
		{ 0,0, text_menu,  0,0,0,0,0,0,0,0,0,0 }, /* TXTBAR_PART_MENU */
		{ 0,0, text_3d,    0,0,0,0,0,0,0,0,0,0 }, /* TXTBAR_PART_3D */
		{ 0,0, text_motor, 0,0,0,0,0,0,0,0,0,0 }, /* TXTBAR_PART_MOTOR */
		{ 0,0, text_skull, 0,0,0,0,0,0,0,0,0,0 }, /* TXTBAR_PART_SKULL */
		{ 0,0, text_pom,   0,0,0,0,0,0,0,0,0,0 }  /* TXTBAR_PART_POM */
};

Bitmap letters_bms[] = {
		{ 7, 92, 0, 0, font_part1, 13, 0 },
		{ 6, 92, 7, 0, font_part1, 13, 0 },
		{ 6, 92, 13, 0, font_part1, 13, 0 },
		{ 6, 92, 19, 0, font_part1, 13, 0 },
		{ 6, 92, 25, 0, font_part1, 13, 0 },
		{ 6, 92, 31, 0, font_part1, 13, 0 },
		{ 6, 92, 37, 0, font_part1, 13, 0 },
		{ 6, 92, 43, 0, font_part1, 13, 0 },
		{ 2, 92, 49, 0, font_part1, 13, 0 },
		{ 5, 92, 51, 0, font_part1, 13, 0 },
		{ 6, 92, 56, 0, font_part1, 13, 0 },
		{ 2, 92, 62, 0, font_part1, 13, 0 },
		{ 8, 92, 64, 0, font_part1, 13, 0 },
		{ 6, 92, 72, 0, font_part1, 13, 0 },
		{ 6, 92, 78, 0, font_part1, 13, 0 },
		{ 5, 92, 84, 0, font_part1, 13, 0 },
		{ 6, 100, 0, 0, font_part2, 13, 0 },
		{ 6, 100, 6, 0, font_part2, 13, 0 },
		{ 6, 100, 12, 0, font_part2, 13, 0 },
		{ 5, 100, 18, 0, font_part2, 13, 0 },
		{ 6, 100, 23, 0, font_part2, 13, 0 },
		{ 6, 100, 29, 0, font_part2, 13, 0 },
		{ 6, 100, 35, 0, font_part2, 13, 0 },
		{ 6, 100, 41, 0, font_part2, 13, 0 },
		{ 8, 100, 47, 0, font_part2, 13, 0 },
		{ 7, 100, 55, 0, font_part2, 13, 0 },
		{ 7, 100, 62, 0, font_part2, 13, 0 },
		{ 5, 100, 69, 0, font_part2, 13, 0 },
		{ 4, 100, 74, 0, font_part2, 13, 0 },
		{ 4, 100, 78, 0, font_part2, 13, 0 },
		{ 2, 100, 82, 0, font_part2, 13, 0 },
		{ 3, 100, 84, 0, font_part2, 13, 0 },
		{ 3, 100, 87, 0, font_part2, 13, 0 },
		{ 2, 100, 90, 0, font_part2, 13, 0 },
		{ 6, 100, 92, 0, font_part2, 13, 0 },
		{ 8, 104, 0, 0, font_part3, 13, 0 },
		{ 6, 104, 8, 0, font_part3, 13, 0 },
		{ 7, 104, 14, 0, font_part3, 13, 0 },
		{ 8, 104, 21, 0, font_part3, 13, 0 },
		{ 7, 104, 29, 0, font_part3, 13, 0 },
		{ 7, 104, 36, 0, font_part3, 13, 0 },
		{ 8, 104, 43, 0, font_part3, 13, 0 },
		{ 8, 104, 51, 0, font_part3, 13, 0 },
		{ 6, 104, 59, 0, font_part3, 13, 0 },
		{ 7, 104, 65, 0, font_part3, 13, 0 },
		{ 7, 104, 72, 0, font_part3, 13, 0 },
		{ 6, 104, 79, 0, font_part3, 13, 0 },
		{10, 104, 85, 0, font_part3, 13, 0 },
		{ 9, 104, 95, 0, font_part3, 13, 0 },
		{ 9, 100, 0, 0, font_part4, 13, 0 },
		{ 5, 100, 9, 0, font_part4, 13, 0 },
		{ 9, 100, 14, 0, font_part4, 13, 0 },
		{ 6, 100, 23, 0, font_part4, 13, 0 },
		{ 8, 100, 29, 0, font_part4, 13, 0 },
		{ 8, 100, 37, 0, font_part4, 13, 0 },
		{ 8, 100, 45, 0, font_part4, 13, 0 },
		{ 8, 100, 53, 0, font_part4, 13, 0 },
		{12, 100, 61, 0, font_part4, 13, 0 },
		{ 9, 100, 73, 0, font_part4, 13, 0 },
		{ 8, 100, 82, 0, font_part4, 13, 0 },
		{ 9, 100, 90, 0, font_part4, 13, 0 },
		{ 6, 100, 0, 0, font_part5, 13, 0 },
		{ 4, 100, 6, 0, font_part5, 13, 0 },
		{ 6, 100, 10, 0, font_part5, 13, 0 },
		{ 6, 100, 16, 0, font_part5, 13, 0 },
		{ 7, 100, 22, 0, font_part5, 13, 0 },
		{ 6, 100, 29, 0, font_part5, 13, 0 },
		{ 6, 100, 35, 0, font_part5, 13, 0 },
		{ 8, 100, 41, 0, font_part5, 13, 0 },
		{ 6, 100, 49, 0, font_part5, 13, 0 },
		{ 6, 100, 55, 0, font_part5, 13, 0 },
		{ 2, 100, 61, 0, font_part5, 13, 0 },
		{ 6, 100, 63, 0, font_part5, 13, 0 },
		{ 2, 100, 69, 0, font_part5, 13, 0 },
		{ 7, 100, 71, 0, font_part5, 13, 0 },
		{ 4, 100, 78, 0, font_part5, 13, 0 },
		{ 4, 100, 82, 0, font_part5, 13, 0 },
		{ 0, 0, 0, 0, NULL, 0, 0}
};

