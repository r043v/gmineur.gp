/** Gmineur.48 - a tribute to the hp48 minehunt **/

#include	"./lib/Gdl/Gdl.h"				// Gdl²
#include	"./lib/modPlay/gpmodplay.h"	// chn modlib

// all gfx, in 16 bit rotated Gfm
#include "Gmineur.h"
#include "sp.h"
#include "hurry.h"

// 2 mod writed by rush
#include "tic-tac.mod.h"
#include "renegade.mod.h"

ushort * boom[9]	= { bm0 , bm1 , bm2 , bm3 , bm4 , bm5 , bm6 , bm7 , bm8 };
ushort * _case[2]	= { case0, case1 }  ;
ushort * grnTk[4]	= { greenTk0, greenTk1, greenTk2, greenTk3 } ;
ushort * bullet[4]	= { bullet0, bullet1, bullet2, bullet3 } ;
ushort * letter[27]	= { lt0,lt1,lt2,lt3,lt4,lt5,lt6,lt7,lt8,lt9,lt10,lt11,lt12,lt13,lt14,lt15,lt16,lt17,lt18,lt19,lt20,lt21,lt22,lt23,lt24,lt25,lt26 };
ushort * chf[10]		= { ch0 , ch1 , ch2 , ch3 , ch4 , ch5 , ch6 , ch7 , ch8 , ch9 } ;
ushort * letterI[26]	= { ltI0,ltI1,ltI2,ltI3,ltI4,ltI5,ltI6,ltI7,ltI8,ltI9,ltI10,ltI11,ltI12,ltI13,ltI14,ltI15,ltI16,ltI17,ltI18,ltI19,ltI20,ltI21,ltI22,ltI23,ltI24,ltI25 };
ushort * chfI[10]	= { chI0 , chI1 , chI2 , chI3 , chI4 , chI5 , chI6 , chI7 , chI8 , chI9 } ;
ushort * sp[5]		= { sp0,sp1,sp2,sp3,sp4 } ;

uchar * msk[2]		= { tictac, renegade } ;
MODPlay zIk ;

short *pixel ; int WIDTH=240, HEIGHT=320, fSIZE=WIDTH*HEIGHT, change = 0 ;
#include	"./nGfm.cpp" // 16bit Gfm blit routines

unsigned char key[256] ;

void showInt(int value, int px, int py, int way=0, ushort **fnt=chf)
{    char str[8], *ptr=str ;
     if(value < 10 && way) sprintf(str, "0%i", value) ;
     else sprintf(str, "%i", value) ;
     int sx = ((*fnt)[4])-6 ;
         while(*ptr)
         {  drawGfm(fnt[(*ptr++)-'0'],px,py) ; px+=sx ;
         };
}

void drawInt(int value, int x, int y)
{	showInt(value,x,y) ;		}

void drawText(const char *txt, int x, int y, ushort **fnt=letter, int limit=42)
{int n=0, px=x, sx=(((letter[0])[4])) - 6 ;
 while(txt[n]!='\0' && n < limit)
 { if(txt[n] >= 'a' && txt[n] <= 'z')
        drawGfm(fnt[txt[n++]-'a'],px,y) ;
   else n++ ;
   px+=sx ; if(px > 320) return ;
 };
}

int mapSx=20, mapSy=15, mapSz=12, mineNb=48 ;
char *map = 0 ;

#define Map(x,y) (map[(x) + (y)*mapSx])
#define tmpArray(x,y) (tempArray[(x) + (y)*mapSx])
#define WalkArray(x,y) (walkArray[(x) + (y)*mapSx])
#define GetTickCount GpTickCountGet

int ok=0 ;
char *tempArray ; // for recursive scanning

void testArray(int x, int y)
{  if(x<0 || y<0 || x>mapSx-1 || y>mapSy-1 || ok || Map(x,y)) return ;
   if((x == mapSx-1) && (y == mapSy-1)){ ok=1 ; return ; }
   if(tmpArray(x,y)) return ; tmpArray(x,y)=1 ;
   testArray(x-1,y) ;
   testArray(x,y-1) ;
   testArray(x+1,y) ;
   testArray(x,y+1) ;
}

int validKey[4] = {0,0,0,0} ;
signed char *walkArray=0 ;

void hDrawLine(int x, int y, int size, int color) ;

int getNb(int px, int py)
{   int nb=0 ;
    if(px>0)                         if(Map(px-1,py))    nb++ ;
    if(px>0 && py>0)                 if(Map(px-1,py-1))  nb++ ;
    if(px>0 && py < mapSy-1)         if(Map(px-1,py+1))  nb++ ;

    if(py>0)                         if(Map(px,py-1))    nb++ ;
    if(py < mapSy-1)                 if(Map(px,py+1))    nb++ ;

    if(py>0 && px < mapSx-1)         if(Map(px+1,py-1))  nb++ ;
    if(px < mapSx-1)                 if(Map(px+1,py))    nb++ ;
    if(px < mapSx-1 && py < mapSy-1) if(Map(px+1,py+1))  nb++ ;
    return nb ;
}

int getChecked(int px, int py)
{ int nb=8 ;
  if(px>0)                         if(WalkArray(px-1,py)==0)    --nb ;
  if(px>0 && py>0)                 if(WalkArray(px-1,py-1)==0)  --nb ;
  if(px>0 && py < mapSy-1)         if(WalkArray(px-1,py+1)==0)  --nb ;
  if(py>0)                         if(WalkArray(px,py-1)==0)    --nb ;
  if(py < mapSy-1)                 if(WalkArray(px,py+1)==0)    --nb ;
  if(py>0 && px < mapSx-1)         if(WalkArray(px+1,py-1)==0)  --nb ;
  if(px < mapSx-1)                 if(WalkArray(px+1,py)==0)    --nb ;
  if(px < mapSx-1 && py < mapSy-1) if(WalkArray(px+1,py+1)==0)  --nb ;
  return 8-nb ;
}

inline int getwalkValue(int px, int py)
{ int nb = getNb(px,py) ;
  if(nb) return nb ;  return -1 ;
}

inline int getnbValue(int px, int py)
{ int nb = WalkArray(px,py) ;
  if(nb>0) return nb ;  return 0 ;
}

void showArray(int x, int y)
{  if(x<0 || y<0 || x>=mapSx || y>=mapSy) return ;
   if(getChecked(x,y) != getnbValue(x,y)) return ;
   if(WalkArray(x,y) < 0) return ;
   WalkArray(x,y) = -1 ;

   if(x < mapSx-1)      WalkArray(x+1,y)  = -1 ;
   if(x)                WalkArray(x-1,y)  = -1 ;
   if(y < mapSy-1)      WalkArray(x,y+1)  = -1 ;
   if(y)                WalkArray(x,y-1)  = -1 ;
   if(x < mapSx-1 && y < mapSy-1) WalkArray(x+1,y+1)= -1 ;
   if(y && x < mapSx-1) WalkArray(x+1,y-1)= -1 ;
   if(y < mapSy-1 && x) WalkArray(x-1,y+1)= -1 ;
   if(x && y)           WalkArray(x-1,y-1)= -1 ;
}

int boomEnabled=0 ;

void showBoom(int x, int y)
{ if(!boomEnabled) return ;
  static long lastTime = GetTickCount(), frm=0 ;
  if(lastTime + 66 < GetTickCount())
  {   lastTime = GetTickCount() ;
      if(++frm >= 9) { boomEnabled=0 ; frm=-1 ; return ; }
  }
  if(frm<0) return ;
  drawGfm(boom[frm],x,y) ;
}

int cx,cy,px,py,zx,zy,mineAround,lx,ly,tankWay ;
int opt[] = {1,1,1,0,0,0,0,0,0,0,0,0,1,0} ;
#define searchZero		(opt[0])
#define searchZeroEnd	(opt[1])
#define tagArray		(opt[2])
#define autoScroll			1
#define revArColor		(opt[3])
#define revFtColor		(opt[4])
#define revBmColor		(opt[5])
#define revTkColor		(opt[6])
#define revMnColor		(opt[7])
#define revIfColor		(opt[8])
#define autoTag		(opt[12])
#define autoUnmasq		(opt[13])
#define optNb				9

char quit=0, dead=0, win=0, fire=0, bulletNb=1  ;
int gamePos=0, gameTime, lastTime, invincible=0, showPt=1 ;

void showBullet(int start=0)
{ if(!fire) return ; if(!bulletNb){ fire=0 ; return ; }
  static int sze=0 ;
  if(start){ sze=0 ;  return ; }
  int x=px+(sze>>4),y=py+(sze>>4) ;
  switch(tankWay)
  { case 2 : x=px ;         break ; // down
    case 1 : y=py ;         break ; // right
    case 0 : x=px ; y=py-(sze>>4) ; break ; // up
    case 3 : x=px-(sze>>4) ; y=py ; break ; // left
  }; if(x<0 || x==mapSx || y<0 || y==mapSy)
             { fire=0 ; change+=2 ; return ; }
    if(WalkArray(x,y)==0)
    { fire=0 ; change+=2 ;
        if(Map(x,y)) { WalkArray(x,y)=-1 ; bulletNb++ ; }
        else { WalkArray(x,y)=getwalkValue(x,y) ; bulletNb=0 ; }
    }
    sze+=2 ;
  switch(tankWay)
  { case 0 : drawGfm(bullet[tankWay],zx,zy+sze-8) ; break ;
    case 1 : drawGfm(bullet[tankWay],zx+sze-8,zy) ; break ;
    case 2 : drawGfm(bullet[tankWay],zx,zy-sze+8) ; break ;
    case 3 : drawGfm(bullet[tankWay],zx-sze+8,zy) ; break ;
  };
}
// lolinda14@yahoo.fr lorianne
int zeroLimit ;
void searchForZero(int x, int y, int n=0)
{  if(!n) zeroLimit=0 ;
   if(x<0 || y<0 || x>mapSx-1 || y>mapSy-1) return ;
   if(WalkArray(x,y)!=0 || Map(x,y)!=0) return ;
   if(WalkArray(x,y) == 0) WalkArray(x,y) = -1 ;
   int nb = getNb(x,y) ;
   if(nb){ if(autoTag) WalkArray(x,y) = nb ; return ; }
   if(++n > 11) zeroLimit++ ;
   searchForZero(x-1,y,n)   ; searchForZero(x-1,y+1,n) ;
   searchForZero(x-1,y-1,n) ; searchForZero(x+1,y,n)   ;
   searchForZero(x+1,y+1,n) ; searchForZero(x+1,y-1,n) ;
   searchForZero(x,y+1,n)   ; searchForZero(x,y-1,n)   ;
}

void generateMap(void)
{ int x,y,n ; ok=0 ;
  while(!ok) { n=0 ;
    memset(map,0,mapSx*mapSy) ;
    memset(walkArray,0,mapSx*mapSy) ;
    // put mines..
    while(n<mineNb)
    { x=rand()%(mapSx-1) ;
      y=rand()%(mapSy-1) ;
      if(x+y) // not start
      	if(x != mapSx-1 || y != mapSy-1) // and not end..
	  if(!Map(x,y)) { Map(x,y)=1 ; ++n ; }
    };
    // is it possible ? .. recursive way
    tempArray = (char*)malloc(mapSx*mapSy) ;
    memset(tempArray,0,mapSx*mapSy) ;
    testArray(0,0) ; free(tempArray) ;
    
    // scan for zero the first and last block, if too easy, regenerate
    if(searchZeroEnd){ searchForZero(mapSx-1,mapSy-1);
                       if(zeroLimit) ok=0 ;
    } else walkArray[(mapSx*mapSy)-1] = getwalkValue(mapSx-1,mapSy-1) ;
    if(searchZero){ searchForZero(0,0) ;
                    if(zeroLimit) ok=0 ;
    }
  };
  px=py=0  ; mineAround=getNb(px,py) ;
  lx=ly=42 ; tankWay=1 ;
}

void drawYesNo(int value,int x,int y, ushort**fnt=letter)
{ if(value)	drawText("yes",x,y,fnt) ;
  else		drawText("no",x,y,fnt) ;
}

short	*saveScr ;
ushort **chfnt=chf, **infoFnt, inOption=0 ;

void dye(void)
{ if(invincible) return ;
  for(int i=0;i<mapSx*mapSy;i++)
    if(map[i]) walkArray[i] = -1 ;
  dead=1 ;
}

void showOption(void)
{	inOption=1 ;
	saveScr = (short*)malloc(320*240*2) ;
	for(int l=0;l<319;l++) // rotate and save screen
	memcpy(saveScr+l*240,pixel+(319-l)*240,240*2) ;
	//memcpy(saveScr,pixel,320*240*2) ;

 const char *str[] = {	"auto search zero",		"search zero at goal",	"tag array",
		                "reverse array color",		"reverse  font color",
                		"reverse  boom color",	"reverse  tank color",
		                "reverse  mine color",		"reverse  info color"
                	};

  int it=-3,way=1,s=32,e=320-32 ;
  long ztme=0 ;
  static int selected=3 ;
	while(keyPush(kR)) refreshKey() ;
  while(!keyPush(kR)) // wait for enter or escape
  {  	pixel = (short*)screen[nflip] ;
  	memcpy(pixel,saveScr,320*240*2) ; // draw rotated screen
	
     // show unselected option
     for(int i=0;i<optNb;i++)
     if(selected != i) {	drawText(str[i],36,212-(20+s)-16*i,letterI) ;
                         	drawYesNo(opt[i],250,212-(20+s)-16*i,letterI) ;
                       }

       { static int skip=42, x2, cnt=-3, wy=1 ;
         if(skip++ >= 12)
         {     x2 = 80+cnt ; skip=0 ;
               if(wy) { if(++cnt > -1)    wy=0 ; }
               else   { if(--cnt < -5)    wy=1 ; }
         }
        drawGfm(title,x2,e-30+(cnt>>1)) ;
       }
     drawGfm(cprt,70,e-65) ;
     // fade a centered square
     int st, ed ;
     for(int l=s;l<e;l++)
     {  if(l<=20+s){ st=18+((20+s)-l) ; ed=240-18-((20+s)-l) ; }
        else if(l>=e-20){ st=18+(l-(e-20)) ; ed=240-18-(l-(e-20)) ; }
        else { st=18 ; ed=240-18 ; } //ed=320-42 ;
        for(int c=st;c<ed;c++)
        { int color = pixel[c+l*240] ;
          int r,g,b ;
          r = 31-((color>>11)&31) ; r-=it ; if(r<0)r=0 ; if(r>31)r=31 ;
          g = 31-((color>>6)&31)  ; g-=it ; if(g<0)g=0 ; if(g>31)g=31 ;
          b = 31-((color>>1)&31)  ; b-=it ; if(b<0)b=0 ; if(b>31)b=31 ;
          pixel[c+l*240] = (r<<11)|(g<<6)|(b<<1)|1 ;
        };
     };

     // timer for fade value
     if(ztme + 102 < GetTickCount())
     { ztme = GetTickCount() ;
       if(way){ if(++it > 16) way=0 ; }
       else   { if(--it < 4) way=1 ; }
     }

     drawGfm(title,80,ed-7) ;
     drawGfm(tribute,56,ed-22) ;
     drawText(str[selected],42,212-(20+s)-16*selected) ;
     drawYesNo(opt[selected],244,212-(20+s)-16*selected) ;

     //showInt(getFreeRam(),122,2) ;
     // test key and change option
       // left or right
	if(key[37]||key[39])
	{   if(validKey[0])
		{  validKey[0]=0 ; opt[selected]^=1 ;
		   if(selected>2 && selected<9) {
		   for(int j=0;j<9;j++)
		    { if(selected==3) { if(j<2) reverseGfmColor(_case[j]) ; } else
		      if(selected==5) { if(j<9) reverseGfmColor(boom[j]) ; } else
		      if(selected==6) { if(j<4) reverseGfmColor(grnTk[j]) ; }
		    }; if(selected==4){  if(chfnt==chf) chfnt=chfI ;
		                         	else                 chfnt=chf ;
		       } else if(selected==8) {  if(infoFnt==chf) infoFnt=chfI ;
							  else             	   infoFnt=chf ;
		       } else if(selected==7) reverseGfmColor(mine) ;
		   }
		}
	}
      else { validKey[0]=1 ;
             // up
            if(key[38]){ if(validKey[1]) {validKey[1]=0 ; if(selected) selected-- ; } }
            else { validKey[1]=1 ;
                    // down
                   if(key[40]){ if(validKey[3]) {validKey[3]=0 ; if(selected<optNb-1) selected++ ; } }
                   else validKey[3]=1 ;
                 }
           }
     flipScreen(); refreshKey();
  } ; free(saveScr) ; inOption=0 ;
  memcpy(pixel,saveScr,320*240*2) ; // draw rotated screen
  flipScreen(); change=2 ;
  while(keyPush(kR)) refreshKey() ;
}

void zmain() // this game was write in tribute of the great hp48 minehunt
{   map = (char*)malloc(mapSx*mapSy) ;
	//short * tscr = (short*)malloc(320*240*2) ;
    walkArray = (signed char *)malloc(mapSx*mapSy) ;
    pixel     = (short*)screen[nflip];
    srand(GetTickCount());
	memset(key,0,256) ;
	
	int hurryUp = 0 ;
	
  while(!quit)
  { generateMap() ; hurryUp = 0 ;
    dead=win=fire=boomEnabled=0 ;  gameTime=4<<16|48 ; lastTime=GetTickCount() ; bulletNb=1 ; change=2 ;
    while (invincible|boomEnabled|fire || !(dead|win))
    { if(GetTickCount() > lastTime + 984) // ml.brdy 4.1.984
      { int m=gameTime>>16 ,s=gameTime&0xFFFF ;
        if(!s){ if(m) { --m ; s=59 ; } else { dye(); } } else --s ;
        gameTime = (m<<16)|s ; lastTime=GetTickCount() ; change+=2 ;
        showPt ^= 1 ; if(!m) hurryUp=1 ;
      }

	refreshKey() ;

     if(!(boomEnabled||fire)) {
      if(key[32]){ fire=1 ; showBullet(1) ; } // A
      if(keyPush(kSelect)) GpAppExit() ;
/*      if(!autoScroll) { static int release=0 ;
       if(key[16] && !key[17]) // "shift"
       { if(release) { release=0 ; gamePos^=1 ; change+=2 ; lx=ly=42 ; }
       } else release=1 ;
      }*/

      if(keyPush(kR)) // start or R
        showOption() ;
      else if(keyPush(kL)) {	static int musik=0 ;
      					musik ^= 1 ;
      					MODPlay_Unload(&zIk) ;
					MODPlay_SetMOD(&zIk,msk[musik]) ;
					MODPlay_Start(&zIk) ;
					while(keyPush(kL)) refreshKey() ;
      				}


//      if(key[17])
//            if(key[72]) { invincible^=1 ; while(key[72]) refreshKey() ; }

      // left
      if(key[37]){ if(validKey[0]) {validKey[0]=0 ; tankWay=3 ; if(px) px-- ; } }
      else { validKey[0]=1 ;
             // up
            if(key[38]){ if(validKey[1]) {validKey[1]=0 ; tankWay=0 ; if(py) py-- ; } }
            else { validKey[1]=1 ;
                   // right
                   if(key[39]){ if(validKey[2]) {validKey[2]=0 ; tankWay=1 ; if(px < mapSx-1) px++ ; } }
                   else { validKey[2]=1 ;
                          // down
                          if(key[40]){ if(validKey[3]) {validKey[3]=0 ; tankWay=2 ; if(py < mapSy-1) py++ ; } }
                          else validKey[3]=1 ;
                        }
                       }
                      }
      }

        if(lx != px || ly != py)
        { if(Map(px,py)||key[17])
          {     if(WalkArray(px,py)==0 && !(key[17])) { boomEnabled=1 ; WalkArray(px,py) = -1 ; dye(); }
                else { switch(tankWay)
                       { case 0 : py++ ; break ;
                         case 1 : px-- ; break ;
                         case 2 : py-- ; break ;
                         case 3 : px++ ; break ;
                       };   change+=2 ;
                     }
          }

          if(autoScroll) gamePos = (py < mapSy>>1) ;
          lx=px ; ly=py ;
          zx = px<<4 ; zy = 240-((py+1)<<4) ;
          mineAround=getNb(px,py) ; change+=2 ;
          if(mineAround){  if(WalkArray(px,py) != -1) WalkArray(px,py) = mineAround ; }
          else    { if(searchZero) searchForZero(px,py) ;
                    else           WalkArray(px,py) = -1;
                  }
          if(px==mapSx-1 && py==mapSy-1) ++win ;
        }
        
       //int fs = countfps() ;
        
       if(change||boomEnabled||fire|| hurryUp){
          memset(pixel,0,320*240*2) ;
          //for(int ey=0;ey<(320/16)-1;ey++) memset(pixel+(240*16*ey),0x11,240*2) ;
          for(cx=0;cx<20;cx++)
           for(cy=0;cy<mapSy;cy++)
            if(WalkArray(cx,cy)!=0)
            {  drawGfm(_case[1],cx<<4,240-((cy+1)<<4)) ;
               if(Map(cx,cy))
                  drawGfm(mine,(cx<<4)+1,240-((cy+1)<<4)-1) ;
               else if(WalkArray(cx,cy)>0)
                      if(tagArray) if(px != cx || py != cy)
                       drawGfm(chfnt[WalkArray(cx,cy)],(cx<<4)+4,(240-((cy+1)<<4))-3) ;
            }  else drawGfm(_case[0],cx<<4,240-((cy+1)<<4)) ;
          showBullet() ;
          drawGfm(grnTk[tankWay],zx+1,zy-1) ;
          showBoom(zx-12,zy-12) ;
          //showInt(fs,5,2,0,chfI);
          if(change) change-- ;
          //showInt(boomEnabled,100,2,0,chfI) ;
          //showInt(fire,150,2,0,chfI) ;
          if(hurryUp) drawGfm(hurry,274+(rand()%2),196+(rand()%2)) ;
	{	int r,g,b ;	int ppy = (!gamePos*175) + 17 ; int cpx=44,cpy ; int ix=zx - 32*(px>16) ;
		unsigned short * s =(unsigned short*)(screen[nflip] + (ix-1)*240 +ppy);
		while(cpx--)
		{ cpy=28 ;
		  while(cpy--)
		  { 	r=(*s>>11)>>2; g=((*s>>6)&31)>>2 ; b=((*s>>1)&31)>>2 ; 
		  	*s++ = r<<11|g<<6|b<<1|1 ;
		  };	s+=(240-28) ;
		};
	          infoFnt = chf ;
	          if(revIfColor) infoFnt = chfI ;
	          showInt(mineAround,ix+4, ppy+14,0,infoFnt);
	          showInt(gameTime>>16,ix+5,ppy-2,0,infoFnt) ;
	          showInt(gameTime&0xFFFF,ix+24,ppy-2,1,infoFnt) ;
	          if(showPt) drawGfm(letter[26],ix+16,ppy-2) ;
	          if(!fire && bulletNb) drawGfm(bullet[0],ix+32,ppy+13) ;
	 }
	  //if(!(boomEnabled||fire||hurryUp) && change==1) memcpy(tscr,pixel,320*240*2) ;
          flipScreen() ; pixel = (short*)screen[nflip];
       } else { change=0 ;flipScreen() ; pixel = (short*)screen[nflip]; }
}
//     long t=GetTickCount() ; inOption=1 ; // loriane
//     while(!key[13] && !key[17] && !key[27] && !key[32] && !key[37] && !key[38] && !key[39] && !key[40] && t + 2420 > GetTickCount()) refreshKey();
//     while(key[13] || key[17] || key[27] || key[32] || key[37] && key[38] && key[39] && key[40])  refreshKey() ;
     inOption=0 ;
  };
}

void GpMain(void * arg)
{	/*__asm {
              MRC p15, 0, R5, c1, c0, 0 // read control register
              BIC r5,r5,#2              // align control OFF
              MOV r6,0x1004
              ORR r5,r5,r6              // I/D cache ON
              MCR p15, 0, R5, c1, c0, 0 // write control register
         }*/

	initScreen();	GpFatInit() ; 
	int logoy=-90; short logox=216 ; int lgw=1, limit=0 ; int intro=1 ;
	pixel = (short*)screen[nflip] ;
	memset(pixel,0xFF,320*240) ;		
	for(int r=0;r<0xFFFF;r++)	*pixel++ = r ;
	flipScreen();
	waitKey(kR) ;
	
	while(intro!=2) { refreshKey() ;
		pixel = (short*)screen[nflip] ;
		//clrScr() ;
		//drawGfm(sp[4],logox,logoy);
		//memset(screen[nflip],0,400*240) ;
		//drawGfm(title,24,192) ;
		if(intro){
			if(lgw==1){	if(++++++logoy > 142-limit) { lgw=0 ; ++++++++++++limit; }}
			else		if(!lgw) if(------logoy < 42+limit){ if(++++++++++++limit>70) intro=0 ; //lgw=2 ;
											else lgw=1 ;
										      }
		}
		//drawGfm(sp[0],20,24);
		//drawGfm(sp[3],20,66);
		//drawGfm(sp[2],20,108);
		//drawGfm(sp[1],20,150);
			
		flipScreen() ;
		//if(lgw==2) if(keyPush(kStart) || keyPush(kA)) intro=0 ;
		if(!intro || limit>32) { if(----logox + 88 < 200) intro=2 ; }
	}; 
	MODPlay_Init (&zIk);
	MODPlay_SetMOD(&zIk,tictac) ;
	MODPlay_Start(&zIk) ;
	zmain() ;
}
