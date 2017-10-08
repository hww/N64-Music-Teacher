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
#include "textbar.h"

extern void drawSprite(Gfx **glistp, Bitmap *bm, int x, int y, f32 scale_x, f32 scale_y, int alphabit);
extern void drawSpriteCol(Gfx **glistp, Bitmap *bm, int x, int y, f32 scale_x, f32 scale_y, int alphabit,u8 r,u8 g,u8 b);
char *drawNextLine(Gfx **glistp, u32 bar, u16 posx, u16 posy ,u8 r, u8 g, u8 b);
char *drawCurrentLine(Gfx **glistp, u32 bar, u16 posx, u16 posy ,u8 r, u8 g, u8 b);
char *my_strchr(char *str, char target);

void initTextbar(u32 bar, u16 delay, u8 alphastep, u16 kickin)
{
  textBar[bar].txtcurrent = textBar[bar].txtstart;
  textBar[bar].txtnext = NULL;
  textBar[bar].curlen = 0;
  textBar[bar].nextlen = 0;
  textBar[bar].delayval = delay;
  textBar[bar].delaycur = 0;
  textBar[bar].alphastep = alphastep;
  textBar[bar].alphacurrent = 0;
  textBar[bar].alphacurrent_dir = INCREASE;
  textBar[bar].alphanext = 0;
  textBar[bar].alphanext_dir = INCREASE;
  textBar[bar].alphanext_kickin = kickin;
}

u16 drawTextbar(Gfx **glistp, u32 bar, u16 posx, u16 posy, u8 r , u8 g, u8 b)
{
  Gfx *gxp;
  char *nextp1,*nextp2;

  gxp = *glistp;

  nextp2 = drawCurrentLine(&gxp, bar, posx, posy , r, g, b);

  if (textBar[bar].txtnext == NULL) {
        textBar[bar].txtnext = nextp2;
        textBar[bar].nextlen = 0;
        textBar[bar].alphanext = 0;
        textBar[bar].alphanext_dir = INCREASE;
  }

  if (textBar[bar].delaycur > 0) {
        textBar[bar].delaycur--;
  } else {

    if (textBar[bar].alphacurrent_dir == INCREASE) {
        textBar[bar].alphacurrent += textBar[bar].alphastep;
        if (textBar[bar].alphacurrent > 255) {
                textBar[bar].alphacurrent = 255;
                textBar[bar].alphacurrent_dir = DECREASE;
                textBar[bar].delaycur = textBar[bar].delayval;
        }
    } else {
        textBar[bar].alphacurrent -= textBar[bar].alphastep;

        if (textBar[bar].alphacurrent < textBar[bar].alphanext_kickin) {
          nextp1 = drawNextLine(&gxp, bar, posx, posy, r, g, b);
          textBar[bar].alphanext += textBar[bar].alphastep;
          if (textBar[bar].alphanext > 255)
              textBar[bar].alphanext = 255;
        }

        if (textBar[bar].alphacurrent < 0) {
           textBar[bar].txtcurrent = textBar[bar].txtnext;
           textBar[bar].curlen = textBar[bar].nextlen;
           textBar[bar].alphacurrent = textBar[bar].alphanext;
           textBar[bar].alphacurrent_dir = textBar[bar].alphanext_dir;

           textBar[bar].txtnext = nextp1;
           textBar[bar].nextlen = 0;
           textBar[bar].alphanext = 0;
           textBar[bar].alphanext_dir = INCREASE;
        }
    }
  }

  *glistp = gxp;
  return(textBar[bar].curlen);

}

char *drawCurrentLine(Gfx **glistp, u32 bar, u16 posx, u16 posy, u8 r, u8 g, u8 b)
{
     char *p = textBar[bar].txtcurrent;
     Bitmap *charbm;
     char *Index;
     u16 dx = posx;
     u16 len = 0;
     Gfx *gxp;

     gxp = *glistp;
     if (textBar[bar].curlen)
        dx += (140-textBar[bar].curlen)/2;

     while((*p - SHIFT) != '\n') {
         if ((*p - SHIFT) != ' ') {

            if ((Index = my_strchr(letters_str, *p - SHIFT)) != NULL) {
               charbm = &letters_bms[Index - letters_str];
               drawSpriteCol(&gxp, charbm, dx+len, posy, 1, 0.8, textBar[bar].alphacurrent, r , g, b);
               len += charbm->width;
            }

         } else {
            len += SPACE;
         }

         p++;

     }

  if (textBar[bar].curlen==0)
     textBar[bar].curlen = len;

  *glistp = gxp;
  return(*++p?p:textBar[bar].txtstart);

}

char *drawNextLine(Gfx **glistp, u32 bar, u16 posx, u16 posy ,u8 r, u8 g,u8 b)
{
     char *p = textBar[bar].txtnext;
     Bitmap *charbm;
     char *Index;
     u16 dx = posx;
     u16 len = 0;
     Gfx *gxp;

     gxp = *glistp;
     if (textBar[bar].nextlen)
        dx += (140-textBar[bar].nextlen)/2;

     while((*p - SHIFT) != '\n') {
         if ((*p - SHIFT) != ' ') {

            if ((Index = my_strchr(letters_str, *p - SHIFT)) != NULL) {
               charbm = &letters_bms[Index - letters_str];
               drawSpriteCol(&gxp, charbm, dx+len, posy, 1, 0.8, textBar[bar].alphanext, r, g, b);
               len += charbm->width;
            }

         } else {
            len += SPACE;
         }

         p++;

     }

  if (textBar[bar].nextlen==0)
     textBar[bar].nextlen = len;

  *glistp = gxp;
  return(*++p?p:textBar[bar].txtstart);

}


char *my_strchr(char *str, char target)
{
  while(*str && (*str != target))
      str++;
  if (*str)
      return(str);
  return(NULL);
}

