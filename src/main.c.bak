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
//#define _CONTROLLER_INCLD

#include <ultra64.h>

#include "main.h"
#include "static.h"
#include "cont.h"
#include "gfx.h"
#include "playseq.h"
//#include "debug.c"
//#include "debug.h"

#include "config.h"

#define KICKIN 150
#define DELAY 30

extern kara_cfg kcnf;
extern s32 seqNo; //MIDI file number

#define  led_stop 4
#define  led_play 2
#define  led_REV 1
#define  led_FF 3
#define  led_pause 5



#define   M_FILE  	0
#define   M_FILES 	1
#define   M_VOICE 	2
#define   M_VOICES 	3
#define   M_ROYAL 	4
#define   M_ROYALS 	5
#define   M_KREST 	6
#define   M_KRESTS 	7
#define   M_KUB 	8
#define   M_KUBS	9
#define   M_K2	 	10
#define   M_K2S		11
#define   M_KL	 	12
#define   M_KLS		13
#define   M_KR	 	14
#define   M_KRS		15
#define   M_TEMP 	16
#define   M_TEMPS 	17
#define   M_TONE 	18
#define   M_TONES 	19
#define   M_NOP 	20

#define TOOLPOS		20
#define HELPPOSX	145	//160
#define K1POSX		40
#define K1POSXMAX	320-60
#define K1POSY		80	//60
//#define K2POSX		40
//#define K2POSY		140
#define KOFSET		32

#define P_STOP	1
#define P_PLAY  2
#define P_PAUSE 3
#define P_FF		4
#define P_REV		5

static int p_number=P_STOP;
static int p_ntmp=0;
static int p_file=0;
static int p_pitch=0;
static int kar_pitch=0;
static int p_tempo=0;
static int p_fix=0;
extern int f_tempo;
int i;


extern void drawSpriteCol(Gfx **glistp, Bitmap *bm, int x, int y, f32 scale_x, f32 scale_y, int alphabit ,u8 r,u8 g,u8 b);
extern void drawSprite(Gfx **glistp, Bitmap *bm, int x, int y, f32 scale_x, f32 scale_y, int alphabit);
extern void SpriteFinish(Gfx **glistp);
extern void SpriteInit(Gfx **glistp);
extern char text_menu[64] ;
extern karatxt kara[2000] ; // array by 2000 slogs 
extern ALCSeqMarker f_start,n_start,marks[1000];
extern u8 marksp[1000]; //pointer to marks
extern Sprite default_sprite;
extern ALCSeq *seq;
/*
 * external variables and definitions for rom mapping addresses
 * used for DMA'ing data from rom to ram
 *
 */

extern char _static_seg_org[],_static_seg_orgend[];
#define _staticSegmentRomStart (_static_seg_org)
#define _staticSegmentRomEnd (_static_seg_orgend)

extern char _cfb_obj[];
#define _cfbSegmentStart (_cfb_obj)

extern char _zbuf_obj[];
#define _zbufSegmentStart (_zbuf_obj)

extern char _code_bss_objend[];
#define _codeSegmentEnd (_code_bss_objend)

/* Give static segment a place in RAM */
char   *staticSegment = _codeSegmentEnd;


/*
 * external variables for thread structures and messages queues
 * all created and defined in BOOT.C
 *
 */

extern OSMesgQueue      retraceMessageQ;
extern OSMesg           retraceMessageBuf;

extern OSMesgQueue      dmaMessageQ;
extern OSMesg           dmaMessageBuf[];
extern OSIoMesg         dmaIOMessageBuf;

extern OSMesgQueue      rdpMessageQ;
extern OSMesg           rdpMessageBuf;

extern OSMesgQueue      rspMessageQ;
extern OSMesg           rspMessageBuf;

extern OSMesgQueue      n_siMessageQ;

extern OSMesg           dummyMessage;

extern OSContStatus     statusdata[MAXCONTROLLERS];
extern OSContPad        controllerdata[MAXCONTROLLERS];
OSPfs                   pfs[MAXCONTROLLERS];

/*
 * Double-buffered data structures
 *
 */

dynamic_stuff   dynamic[2];
OSTask          task[2];

/*
 * Stack for SP task
 *
 */

u64 dram_stack[SP_DRAM_STACK_SIZE64];	/* used for matrix stack */


/*
 * The work horse of this demo.
 *
 */

u8 current_bit = 0;		// Curren bit of music
u8 len_bit[LINES];				// размерность - количество строк в караоке

void get_lenbit(void)
{
int j;
for(j=0;j<LINES;j++)
{
	if (marksp[current_bit+j+1] != 0xFF) 
	{
		len_bit[j]=marksp[current_bit+j+1] - marksp[current_bit+j];
	}
	else
	{	
		len_bit[j]=0;
	}
}
}


void get_curbit (void)
{
s32 p;
u8  bp;
int i;
	p = getpos_midi();
	bp = 0;
	while ( p > kara[marksp[bp+=LINES]].pos );
	if ((bp>0) && (p != kara[marksp[bp]].pos )) bp-=LINES;
	current_bit=bp;
	get_lenbit();
	//for (i=0;i<LINES;i++) len_bit[i]=marksp[bp+i+1] - marksp[bp+i];
}

  u8		panelp[8] = {0,2,4,6,8,10,12,12};
  u8		panelplight = led_stop;

void stop_stop (void)
{
	p_number=P_STOP;
	panelplight=led_stop;
	stop_midi();
	alCSeqSetLoc(seq,&n_start);
	get_curbit();
	//current_bit=0;
}
			

u8 next_voicem(u8 m)
{
	switch (m)
	{
	case M_VOICE: 
		return M_KREST;
	case M_ROYAL: 
		return M_VOICE;
	case M_KREST: 
		return M_ROYAL;
	}
}

u8 next_akom(u8 m)
{
	switch (m)
	{
	case M_KUB:
		return M_KREST;
	case M_KREST:
		return M_KUB;
	}
}

u8 next_karam(u8 m)
{
	switch (m)
	{
	case M_KL: 
		if (songs[p_file].chanels == 2)  
			return M_KR;
		else
			return M_KREST;
	case M_KR: 
		return M_KREST;
	case M_KREST: 
		return M_KL;
	}
}

void kara_gen(void)
{
  kcnf.precision = 2; //Допустимый сдвиг в тиках относительно кванта при вычислении большой буквы
	if((kcnf.chanel&1) == 0) kcnf.base_note = songs[p_file].base1-kar_pitch;
		else kcnf.base_note = songs[p_file].base2-kar_pitch;
  kcnf.base_kara = songs[p_file].ticks; //192;//96;//тиков на 1 четверть (квант)
  kcnf.size = songs[p_file].size; //4; через сколько квантов идет большая буква
  make_karaoke();
  get_curbit ();
}

DemoLoop(void)
{
  dynamic_stuff *generate;
  OSTask        *gentask;
  Gfx           *glistp0;
  Gfx           *glistp;

  f32           alphacnt=0;
  f32           spmove=0;
  int           j,i;


  u32           moveCnt  = 0;
  u16           perspnorm;      // perspective settings

  int           draw_buffer=0;
  unsigned short tempbuf[32];
  unsigned short tempbuf2[40];
  int           logorowcnt=0;
  int           logocolcnt=0;    

  u8		glow;
  u8		panbutton =2;
  u16		old_button;
  u8		panelmode = 0;
  u8		panelm[9] = {M_NOP,M_FILE,M_ROYAL,M_ROYAL,M_KUB,M_KL,M_TEMP,M_TONE,M_NOP};
  u8 		bgnumber = 0;
  u8 		n,c,l,lnum,ypos;
  s32		cptr,nptr;
  
  Bitmap *charbm;
 
  osViBlack(0);

  initTextbar(TXTBAR_PART_MENU, DELAY, 6, KICKIN);

  if (songs[p_file].chanels == 2) 
	panelm[3]=M_ROYAL;
  else 
	panelm[3]=M_KREST; 


  while (1) {    
    alphacnt++;
    moveCnt++;
    spmove++;

    generate = &(dynamic[draw_buffer]);
    glistp0  = &(generate->glist[0]);
    glistp   = glistp0;

    /* Tell RCP where each segment is */
    gSPSegment(glistp++, 0, 0x0);
    gSPSegment(glistp++, STATIC_SEGMENT, osVirtualToPhysical(staticSegment));

    /* Clear zbuffer */
    gSPDisplayList(glistp++, clearzbuffer_dl);

    /* Clear frame buffer */
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD,
      OS_K0_TO_PHYSICAL(_cfb[draw_buffer]));
//    gDPSetFillColor(glistp++, GPACK_RGBA5551(128,0,128,1) << 16 |
//      GPACK_RGBA5551(128,0,128,1));
    gDPSetFillColor(glistp++, 0x000000ff);

    gSPDisplayList(glistp++, clearcfb_dl);    
    
    /* Initialize RSP and RDP state */
    gSPDisplayList(glistp++, init_dl);
/*
    guPerspective(&(generate->projection),
                  &perspnorm,55, 320.0/240.0,
                  1.0, 2500.0, 25.0);

    guLookAt(&generate->viewing,
             0,0,0,
             0,0,-2,
             0,1,0);

    // Set pointers in drawing list to right entity in dynamic data to use right matrices
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(generate->projection)),
      G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(generate->viewing)),
      G_MTX_PROJECTION|G_MTX_MUL|G_MTX_NOPUSH);
    gSPPerspNormalize(glistp++, perspnorm);
*/
    SpriteInit(&glistp);
    
    logorowcnt=0;
    logocolcnt=0;    

//    default_sprite.bmsiz = G_IM_SIZ_8b;
//    default_sprite.bmfmt = G_IM_FMT_IA; 
    
//       default_sprite.bmfmt = G_IM_FMT_RGBA; 
//       default_sprite.bmsiz = G_IM_SIZ_16b;

    default_sprite.bmfmt = G_IM_FMT_IA;
/*
    for(j=0;j<16;j++) 
    {
      	drawSpriteCol(&glistp,backg_bitmap[0],j*20,0,1,1,128,0,0,255);
    }
*/

    for(j=0;j<48;j++) 
    {
	if (logorowcnt==8) 
	{
	    logorowcnt=0;
	    logocolcnt++;
	}
      	drawSpriteCol(&glistp,backg_bitmap[bgnumber],40*logorowcnt++,40*logocolcnt,1,1,255,76,76,150);
    }

       ypos=K1POSY;
       cptr = getpos_midi();

//* Вывод караоке
	 if(panelm[5]!=M_KREST)
	 {
	        i = K1POSX;

		for (lnum=0;lnum<LINES;lnum++)		// <4
		{
		    if (marksp[current_bit]==0xFF) continue;
		    l=len_bit[lnum];
		    for(j=0;j<l;j++) 
		    {
		    	n =(u8) kara[marksp[current_bit+lnum]+j].note;
  		        if (n == KEOF)break;

		    	c =(u8) kara[marksp[current_bit+lnum]+j].com;
		    	nptr = kara[marksp[current_bit+lnum]+j].pos;
		   	if((n>35) && (n<72))i+=4;
		   	if (cptr<nptr)
			{
				drawSpriteCol(&glistp,&kara_bms[n],i,ypos+kara_bms[n].t,1,1,255,0,0,0);
				i+=kara_bms[n].width;
			}
			else
			{
				drawSpriteCol(&glistp,&kara_bms[n],i,ypos+kara_bms[n].t,1,1,255,255,255,0);
				i+=kara_bms[n].width;
			}
			while(c>1)
			{
				nptr+=kara[marksp[current_bit+lnum]+j].dur;

			  	if (cptr<nptr)
			  	{
					drawSpriteCol(&glistp,&kara_bms[KCOMON],i,ypos+kara_bms[KCOMON].t,1,1,255,0,0,0);
				}
				else
				{
					drawSpriteCol(&glistp,&kara_bms[KCOMON],i,ypos+kara_bms[KCOMON].t,1,1,255,255,255,0);
				}		   		
				i+=kara_bms[KCOMON].width;
				c--;
			} //end while c>1
			// perevod na sleduyuchuyu stroku odnako

			if ( i>K1POSXMAX )
			{
			 i = K1POSX; ypos+=KOFSET;
                        }
		    }	//end for j
		} //end for lnum

         //!!! sprintf(text_menu, "%d %d %d\n",current_bit,marksp[current_bit+1] ,marksp[current_bit+2] );	
	  nptr += ((kara[marksp[current_bit+LINES]].pos - nptr)>>1);
	  if ((cptr>nptr) && (marksp[current_bit+LINES] != 0xFF))
	  {
			current_bit+=LINES;
			//len_bit[0]=marksp[current_bit+1] - marksp[current_bit];
			get_lenbit();
	  }
	 }	
// конец вывода караоке */
    default_sprite.bmfmt = G_IM_FMT_RGBA;

    i=0;
    for(j=0;j<8;j++) {
	if (j==panelplight) 
	{
		drawSprite(&glistp,copyr_bitmap[panelp[j]+1],i,TOOLPOS ,1,1,255);
	}
	else
	{

 	  	drawSprite(&glistp,copyr_bitmap[panelp[j]],i,TOOLPOS ,1,1,255);
	}
	i+=copyr_bitmap[panelp[j]]->width;
    }
    i=0;
    if (panelmode) {
	    for(j=0;j<9;j++) {
	    	drawSprite(&glistp,logo_bitmap[panelm[j]],i,TOOLPOS+12,1,1,255);
		if (j==panbutton) drawSprite(&glistp,logo_bitmap[panelm[j]+1],i,TOOLPOS+12,1,1,glow);
	    	i+=logo_bitmap[panelm[j]]->width;
	    }
	    if (glow <255) glow+=15;
    }

    drawTextbar(&glistp, TXTBAR_PART_MENU, HELPPOSX, TOOLPOS + 1,60,60,60);
    
    SpriteFinish(&glistp);

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);

    /* Build graphics task to execute the display list we just built */
    gentask = &(task[draw_buffer ? 0 : 1]);
    gentask->t.type            = M_GFXTASK;
    gentask->t.flags           = 0x0;
    gentask->t.ucode_boot      = (u64*) rspbootTextStart;
    gentask->t.ucode_boot_size = ((int)rspbootTextEnd - (int)rspbootTextStart);
    gentask->t.ucode           = (u64*) gspFast3DTextStart; /* use XBUS */
    gentask->t.ucode_data      = (u64*) gspFast3DDataStart;
    gentask->t.ucode_size      = 4096;
    gentask->t.ucode_data_size = 2048;
    gentask->t.dram_stack      = (u64*) dram_stack;
    gentask->t.dram_stack_size = 1024;
    gentask->t.output_buff     = (u64*) NULL;
    gentask->t.output_buff_size= (u64*) NULL;
    gentask->t.yield_data_ptr  = (u64*) NULL; /* Graphics only - no yielding */
    gentask->t.yield_data_size = 0x0;
    gentask->t.data_ptr        = (u64*) glistp0;
    gentask->t.data_size       = ((int) glistp - (int) glistp0);
    
    /* Write back dirty cache lines that need to be read by the RCP */
    osWritebackDCache(generate, sizeof(dynamic_stuff));

    /* Fire up SP task */
    osSpTaskStart(gentask);

    /* wait for SP completion */
    (void)osRecvMesg(&rspMessageQ, NULL, OS_MESG_BLOCK);
    (void)osRecvMesg(&rdpMessageQ, NULL, OS_MESG_BLOCK);
          
    /* setup to swap buffers */
    osViSwapBuffer(_cfb[draw_buffer]);

    /* Make sure there isn't an old retrace in queue */
    //if (MQ_IS_FULL(&retraceMessageQ))
    //  (void)osRecvMesg(&retraceMessageQ, NULL, OS_MESG_BLOCK);
    /* Wait for Vertical retrace to finish swap buffers */
    (void)osRecvMesg(&retraceMessageQ, NULL, OS_MESG_BLOCK);
    
    draw_buffer ^= 1;
    //audio_update();
		if(panelmode&&(panbutton==1))
			sprintf(text_menu, "%15s\n",songs[p_file].name);			//вывод в строку состояния имени файла
		else
			sprintf(text_menu, "TEMPO::%-3d PITCH::%+02d\n",p_tempo,p_pitch);			//вывод в строку состояния
		
//*************************************************************************
//        u16     button;
//        s8      stick_x;                /* -80 <= stick_x <= 80 */
//        s8      stick_y;                /* -80 <= stick_y <= 80 */
//        u8      errno;
//*************************************************************************

// ОБРАБОТКА НАЖАТИЙ НА КНОПКИ
    ReadController();
    if (controllerdata[0].button != old_button) //Нажата новая кнопка
    {
    	if ( controllerdata[0].button & START_BUTTON)  
    	{
    		panelmode ^= 1;
    		panelp[0] = panelmode; 
				glow=0;
			}
	if ( controllerdata[0].button &  L_TRIG )  //Смена фона
	{
//	    	bgnumber++;
//	    	if (bgnumber>13) bgnumber=0; 
		if ( (p_tempo-=STP_TEMPO)<MIN_TEMPO ) p_tempo=MIN_TEMPO;
		set_tempo(p_tempo);
	}
	if ( controllerdata[0].button &  R_TRIG )  //Смена фона
	{
		if ( (p_tempo+=STP_TEMPO)>MAX_TEMPO ) p_tempo=MAX_TEMPO;
		set_tempo(p_tempo);
	}
    	if ( controllerdata[0].button & R_JPAD && p_fix==0)  
    	{
				if (++panbutton>7) panbutton=7;
				glow=0;
			}
    	if ( controllerdata[0].button & L_JPAD && p_fix==0)  
    	{
				if (--panbutton<1) panbutton=1;
				glow=0;
			}

	if (panelmode) //Разбор полетов в меню
	{
	    	if ( controllerdata[0].button & A_BUTTON)  //Enter
		{
					switch (panbutton) //Разбор полетов по кнопке A
					{
					case 1:
						stop_stop();
						break;
					case 7:
						stop_stop();
						break;
					case 2: //Voice mode ch1
						panelm[2]=next_voicem(panelm[2]);
						ins_ch(0,panelm[2]);
						break;
					case 3: //Voice mode ch2
						if ( songs[p_file].chanels == 2)
						{
							panelm[3]=next_voicem(panelm[3]);
							ins_ch(1,panelm[3]);
						}
						break;
					case 4: //Accomp mode
						panelm[4]=next_akom(panelm[4]);
						if(panelm[4]==M_KUB) accom_on(panelm); else accom_off(panelm);
						break;
					case 5: //Karaoke mode
						stop_stop();
        					panelm[5]=next_karam(panelm[5]);

						if(panelm[5]==M_KR) kcnf.chanel=KARA_CH1;
						else kcnf.chanel=KARA_CH0;
						if(panelm[5]!=M_KREST) kara_gen();
						glow=0;
						break;
					default:
						break;
					}	//end switch	
		}		//endif A_BUTTON
				//---------------------------------------------------
					switch (panbutton) //Разбор полетов не по кнопке A
					{
					case 1:	//file
						if (p_number==P_STOP)
						{
	    						if ( controllerdata[0].button & U_JPAD ) //Up
							{
								if ( ++p_file>MAX_FILE_SBK-1) p_file=MAX_FILE_SBK-1;
				  		        	p_fix=0;
								if (songs[p_file].chanels > 0) p_fix=1;
								glow=0;
								break;
							}
	    						if ( controllerdata[0].button & D_JPAD ) //Down
							{
								if ( --p_file<0 ) p_file=0;
								p_fix = 0;
				  		        	if (songs[p_file].chanels > 0) p_fix=1;
								glow=0;
								break;
							}
			    				if ( controllerdata[0].button & A_BUTTON ) //Enter
							{
								glow=0;
								if (p_fix)
								{
									seqNo=p_file;
									panelm[2]=M_ROYAL;
									if (songs[p_file].chanels == 2) 
										panelm[3]=M_ROYAL;
									else 
										panelm[3]=M_KREST; 
									panelm[4]=M_KUB;
									panelm[5]=M_KL;
									p_pitch=kar_pitch=0;
									solf_shift(0);
									set_file();
									set_chvol(14,0);
									set_chvol(15,0);
											//здесь нужно сгенерить караоке
  									kcnf.chanel = KARA_CH0;
									kara_gen();
									p_tempo=f_tempo;
									p_fix=0;
								}
								break;
							}
	    						if ( controllerdata[0].button & B_BUTTON ) //Cancel
							{
							  glow=0;
							  if (p_fix)
							  {
								p_file=seqNo;
								p_fix=0;
							  }
							  break;
							}
						}
						break;
					case 6: //Change tempo
	    			if ( controllerdata[0].button & U_JPAD ) //Up
						{
							if ( (p_tempo+=STP_TEMPO)>MAX_TEMPO ) p_tempo=MAX_TEMPO;
							set_tempo(p_tempo);
							glow=0;
						}
	    			if ( controllerdata[0].button & D_JPAD ) //Down
						{
							if ( (p_tempo-=STP_TEMPO)<MIN_TEMPO ) p_tempo=MIN_TEMPO;
							set_tempo(p_tempo);
							glow=0;
						}
						break;
					case 7: //Change pitch
						if ( p_number==P_STOP )
						{
			    				if ( controllerdata[0].button & U_JPAD ) //Up
							{
								if ( ++p_pitch>MAX_PITCH_U ) p_pitch=MAX_PITCH_U;
								p_fix=1;
								glow=0;
							}
	    						if ( controllerdata[0].button & D_JPAD ) //Down
							{
								if ( --p_pitch<MAX_PITCH_D ) p_pitch=MAX_PITCH_D;
								p_fix=1;
								glow=0;
							} 
	    						if ( controllerdata[0].button & A_BUTTON ) //Enter
							{
								glow=0;
								if (p_fix)
								{
									kar_pitch=p_pitch;
									//здесь нужно сгенерить караоке
									kara_gen();
									solf_shift(kar_pitch);
									p_fix=0;
								}
								break;
							}
	    						if ( controllerdata[0].button & B_BUTTON ) //Cancel
							{
							  glow=0;
								if (p_fix)
								{
									p_pitch=kar_pitch;
									p_fix=0;
								}
								break;
							}
						}
						break;
					default:
						break;
					}	//end switch	

	} 				//endif panelmode
	//else			//not panelmode
	//{					//Управление кнопками проигрывателя
	  if ( controllerdata[0].button & D_CBUTTONS)
		  if ( p_number==P_PLAY )
			{
				p_number=P_PAUSE;
				panelplight=led_pause;
				stop_midi();
			}
			else
			{
				p_number=P_PLAY;
				panelplight=led_play;
				play_midi();
			}
	  if ( controllerdata[0].button & U_CBUTTONS)
		{
			stop_stop();	// STOP PLAYER 
		}
	  if ( controllerdata[0].button & R_CBUTTONS && !p_ntmp) { p_ntmp=p_number; p_number=P_FF; }
	  if ( controllerdata[0].button & L_CBUTTONS && !p_ntmp) { p_ntmp=p_number; p_number=P_REV; }
	  if ( !controllerdata[0].button && p_ntmp) { p_number=p_ntmp; p_ntmp=0; }
		if (p_ntmp)
			{
				if (p_number==P_FF && marksp[current_bit+LINES]!=0xFF)
					{
						panelplight=led_FF;
						current_bit+=LINES;
						get_lenbit();
						alCSeqSetLoc(seq,&marks[marksp[current_bit]]);
						//get_curbit();
					}
				if (p_number==P_REV && current_bit)
					{
						panelplight=led_REV;
                				current_bit-=LINES;
						get_lenbit();
						alCSeqSetLoc(seq,&marks[marksp[current_bit]]);
						//get_curbit();
					}
			}
	//}					//end else panelmode
    }				//endif old_button
    old_button=controllerdata[0].button;
		if(!playing())	//переход в режим STOP по окончании композиции
		{
			p_number=P_STOP;
			panelplight=led_stop;
			stop_midi();
			alCSeqSetLoc(seq,&n_start);
			get_curbit();
		}
  audio_update();
  }					//end while(1)

}						//end DemoLoop()


/*
 * The main thread, copies static segment to RAM and fires up DemoLoop
 *
 */
// Отсюда мы стартуем
void mainproc(void *arg)
{
  //int i;
  initControllers();

  /* DMA static segment to ram */
  osPiStartDma(&dmaIOMessageBuf, OS_MESG_PRI_NORMAL, OS_READ,
         (u32)_staticSegmentRomStart, staticSegment,
	       (u32)_staticSegmentRomEnd - (u32)_staticSegmentRomStart,
	       &dmaMessageQ);

  /* Wait for DMA complete */
  (void)osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);
  /* Start rocking */
	// Здесь должен быть выбор midi файла
	seqNo=0;
  open_midi();
	set_chvol(14,0);
	set_chvol(15,0);
	//здесь нужно сгенерить караоке
  kcnf.chanel = KARA_CH0;
	p_tempo=f_tempo;
	kara_gen();

//  EPrint ("Beg %d \n", current_bit);
//  EPrint ("Ln1 %d \n", len_bit[0] );
//  EPrint ("Ln2 %d \n", len_bit[1] );

  DemoLoop();

}