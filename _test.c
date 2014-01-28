
/* Gmineur.48 a tribute to the hp48 minehunt */
// (C) christmas 2k4 r043v

/*/ powered by marie line /*/ // yet this time it's not marie jeanne :]

#include <windows.h>
#include "tinyptc.h"

// gfx
#include "chfont.h"
#include "boom.h"
#include "array.h"
#include "mine.h"
#include "greenTk.h"
#include "bullet.h"
#include "letter.h"
#include "title.h"
#include "tribute.h"
#include "cprt.h"

#include <alloc.h>
#include <stdio.h>
#include <conio_mingw.h>

int WIDTH=320, HEIGHT=256+18, fSIZE=WIDTH*HEIGHT, *pixel, change = 0 ;

extern unsigned char key[256] ;
//int * pcx2Gfm(const char *path, int trColor=0, int fitColor=0) ;
void drawGfm(int *Gfm, int x, int y) ;
//void gfm2h(int *Gfm, char * path, char * name) ;
//int *loadPcx(const char *path, int *sx=0, int *sy=0) ;
//int* scanImg(int *img, int sx, int sy, int *trClr) ;

int * chf[]   = { ch0 , ch1 , ch2 , ch3 , ch4 , ch5 , ch6 , ch7 , ch8 , ch9 } ; // Gfm of all digits
int * boom[]  = { bm0 , bm1 , bm2 , bm3 , bm4 , bm5 , bm6 , bm7 , bm8 } ;  // Gfm of boom animations
int * _case[] = { case0, case1 }  ; // blocks of array
int * grnTk[] = { greenTk0, greenTk1, greenTk2, greenTk3 } ; // green tank
int * bullet[]= { bullet0, bullet1, bullet2, bullet3 } ; // tank bullet
int * letter[]= { lt0,lt1,lt2,lt3,lt4,lt5,lt6,lt7,lt8,lt9,lt10,lt11,lt12,lt13,lt14,lt15,lt16,lt17,lt18,lt19,lt20,lt21,lt22,lt23,lt24,lt25,lt26 };
int * letterI[26] ; int * chfI[10] ;

void showInt(int value, int px, int py, int way=0, int **fnt=chf)
{    char str[8], *ptr=str ;
     if(value < 10 && way) sprintf(str, "0%i", value) ;
     else sprintf(str, "%i", value) ;
     int sx = ((*fnt)[2])>>16 ; // get font sx
         while(*ptr)
         {  drawGfm(fnt[(*ptr++)-'0'],px,py) ; px+=sx ;
         };
     /*switch(way)
     { case 0 : while(*ptr)
                {  drawGfm(chf[(*ptr++)-'0'],px,py) ; px+=sx ;
                };
       break ;  case 1 : break ;
     };*/
}

void drawText(const char *txt, int x, int y, int **fnt=letter, int limit=42)
{int n=0, px=x, sx=(((letter[0])[2])>>16) - 3 ;
 while(txt[n]!='\0' && n < limit)
 { if(txt[n] >= 'a' && txt[n] <= 'z')
        drawGfm(fnt[txt[n++]-'a'],px,y) ;
   else n++ ;
   px+=sx ; if(px > WIDTH) return ;
 };
}

int mapSx=20, mapSy=16, mapSz=12, mineNb=48 ;
char *map = 0 ;

#define Map(x,y) (map[(x) + (y)*mapSx])
#define tmpArray(x,y) (tempArray[(x) + (y)*mapSx])
#define WalkArray(x,y) (walkArray[(x) + (y)*mapSx])

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
char *walkArray=0 ;

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
   printf("\nset uncheck at %i %i",x,y) ;

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
  static int lastTime = GetTickCount(), frm=0 ;
  if(lastTime + 66 < GetTickCount())
  {   lastTime = GetTickCount() ;
      if(++frm >= 9) { boomEnabled=0 ; frm=-1 ; return ; }
  }
  if(frm<0) return ;
  drawGfm(boom[frm],x,y) ;
}

int cx,cy,px,py,zx,zy,mineAround,lx,ly,tankWay ;
int opt[] = {1,1,1,0,0,0,0,0,0,0,0,0,1,0} ;
#define searchZero    (opt[0])
#define searchZeroEnd (opt[1])
#define tagArray      (opt[2])
#define autoScroll    (opt[3])
#define revArColor    (opt[4])
#define revFtColor    (opt[5])
#define revBmColor    (opt[6])
#define revTkColor    (opt[7])
#define revMnColor    (opt[8])
#define revIfColor    (opt[9])
#define autoTag       (opt[12])
#define autoUnmasq    (opt[13])
#define optNb 10

char quit=0, dead=0, win=0, fire=0, bulletNb=1  ;
int gamePos=1, gameTime, lastTime, invincible=0, showPt=1 ; ;

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
             { fire=0 ; change++ ; return ; }
    if(WalkArray(x,y)==0)
    { fire=0 ; change++ ;
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

int countFps(void) ;

int zeroLimit ;
void searchForZero(int x, int y, int n=0)
{  if(!n) zeroLimit=0 ;
   if(x<0 || y<0 || x>=mapSx || y>=mapSy) return ;
   if(WalkArray(x,y)!=0 || Map(x,y)!=0) { if(autoUnmasq) showArray(x,y) ; return ; }
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
{ int x,y,n=0 ; ok=0 ;
  while(!ok) {
    memset(map,0,mapSx*mapSy) ;
    // generate .. yet, by a rand
    while(n<mineNb)
    { x=rand()%mapSx ;
      y=rand()%mapSy ;
      if(x+y)
       if(x < mapSx || y < mapSy)
        if(!Map(x,y)) { Map(x,y)=1 ; ++n ; }// printf("\nput mine at %i %i",x,y) ; }
    };
    // is it possible ? .. recursive way
    tempArray = (char*)malloc(mapSx*mapSy) ;
    memset(tempArray,0,mapSx*mapSy) ;
    testArray(0,0) ; free(tempArray) ;
    memset(walkArray,0,mapSx*mapSy) ;
    // scan for zero the first and last block, if too easy, regenerate
    char *easy = "\nmap too easy" ;
    if(searchZeroEnd){ searchForZero(mapSx-1,mapSy-1);
                       if(zeroLimit) { ok=0 ; printf(easy) ; }
    } else walkArray[mapSx*mapSy-1] = getwalkValue(mapSx-1,mapSy-1) ;
    if(searchZero){ searchForZero(0,0) ;
                    if(zeroLimit)
                    { ok=0 ; printf(easy) ; }
    }
    if(!ok) { printf("\nregenerate map") ; n=0 ; }
  };
  px=py=0  ; mineAround=getNb(px,py) ;
  lx=ly=42 ; tankWay=1 ;
}

void showMenu(void)
{


}

void drawYesNo(int value,int x,int y, int**fnt=letter)
{ if(value) drawText("yes",x,y,fnt) ;
  else      drawText("no",x,y,fnt) ;
}

int *saveScr ; void reverseGfmColor(int *Gfm);
int **chfnt=chf, **infoFnt, inOption=0 ;

void showOption(void)
{ inOption=1 ; saveScr = (int*)malloc(fSIZE*4) ;
  for(int l=0;l<HEIGHT;l++) // rotate and save screen
    memcpy(saveScr+l*320,pixel+(HEIGHT-l)*320,320*4) ;

  char *str[] = { "auto search zero",    "search zero at end",
                  "tag array",           "auto scrolling",
                  "reverse array color", "reverse  font color",
                  "reverse  boom color", "reverse  tank color",
                  "reverse  mine color", "reverse  info color"
                };
  int it=32,way=1,ztme=0,s=18+16*gamePos,e=18+224+16*gamePos ;
  static int selected=4 ;

  while(!(key[13] || key[27])) // wait for enter or escape
  {  memcpy(pixel,saveScr,fSIZE*4) ; // draw rotated screen

     // show unselected option
     for(int i=0;i<optNb;i++)
     if(selected != i) { drawText(str[i],60,e-79-16*i,letterI) ;
                         drawYesNo(opt[i],238,e-79-16*i,letterI) ;
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
     {  if(l<=20+s){ st=42+((20+s)-l) ; ed=278-((20+s)-l) ; }
        else if(l>=e-20){ st=42+(l-(e-20)) ; ed=278-(l-(e-20)) ; }
        else { st=42 ; ed=278 ; }
        for(int c=st;c<ed;c++)
        { int color = pixel[c+l*320] ;
          int r,g,b ;
          r = 255-((color>>16)&255) ; r-=it ; if(r<0)r=0 ; if(r>255)r=255 ;
          g = 255-((color>>8)&255) ;  g-=it ; if(g<0)g=0 ; if(g>255)g=255 ;
          b = 255-(color&255) ;       b-=it ; if(b<0)b=0 ; if(b>255)b=255 ;
          pixel[c+l*320] = (r<<16)|(g<<8)|b ;
        };
     };
     // timer for fade value
     if(ztme + 10 < GetTickCount())
     { ztme = GetTickCount() ;
       if(way){ if(++it > 144) way=0 ; }
       else   { if(--it < 64) way=1 ; }
     }

     drawGfm(title,80,e-28) ;
     drawGfm(tribute,56,e-48) ;
     drawText(str[selected],72,e-79-16*selected) ;
     drawYesNo(opt[selected],238,e-79-16*selected) ;
     // test key and change option
       // left or right
      if(key[37]||key[39]){ if(validKey[0])
                            {  validKey[0]=0 ; opt[selected]^=1 ;
                               if(selected>3 && selected<9) {
                                for(int j=0;j<10;j++)
                                { if(selected==4) { if(j<2) reverseGfmColor(_case[j]) ; } else
                                  //if(selected==5) reverseGfmColor(chf[j]) ; else
                                  if(selected==6) { if(j<9) reverseGfmColor(boom[j]) ; } else
                                  if(selected==7) { if(j<4) reverseGfmColor(grnTk[j]) ; }
                                }; if(selected==5){  if(chfnt==chf) chfnt=chfI ;
                                                     else           chfnt=chf ;
                                   } else if(selected==9) {  if(infoFnt==chf) infoFnt=chfI ;
                                                             else             infoFnt=chf ;
                                   } else if(selected==8) reverseGfmColor(mine) ;
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
     ptc_update(pixel);
  } ; free(saveScr) ; inOption=0 ;
}

int *rloadPcx(const char *path, int *sx=0, int *sy=0);
int *rldPcx(const char *path, int nb) ;

void dye(void)
{ if(invincible) return ;
  for(int i=0;i<mapSx*mapSy;i++)
    if(map[i]) walkArray[i] = -1 ;
  dead=1 ;
}

int main() // this game was write in tribute of the great hp48 minehunt
{   if (!ptc_open("Gmineur.48",WIDTH,HEIGHT)) return 1;
    map = (char*)malloc(mapSx*mapSy) ;
    walkArray = (char*)malloc(mapSx*mapSy) ;
    pixel     = (int*)malloc(fSIZE*4) ;
    printf("\nGdl say you hello\n") ;   srand(GetTickCount());
    reverseGfmColor(letter[26]) ;
    for(int i=0;i<26;i++)
    { letterI[i] = (int*)malloc((letter[i])[1]) ;
      memcpy(letterI[i],letter[i],(letter[i])[1]) ;
      reverseGfmColor(letterI[i]) ;
    }

    for(int i=0;i<10;i++) // need to be free !!
    { chfI[i] = (int*)malloc((chf[i])[1]) ;
      memcpy(chfI[i],chf[i],(chf[i])[1]) ;
      reverseGfmColor(chfI[i]) ;
    }

    //cprt = pcx2Gfm("c:\\cop.pcx",0,0) ;
    //gfm2h(cprt,"c:\\cprt.h","cprt") ;
  /*  int * ach[27] ; // pcx2h
    int * abm = loadPcx("c:\\font.pcx") ;
    char pth[42+21] ;
    int *aboom[27] ;      //int trClr = 0x42004242 ;
    sprintf(pth ,"c:\\letter.h") ;
    for(int n=0;n<27;n++)
    { aboom[n] = scanImg(abm+(11*16*n),11,16,abm) ;

      sprintf(pth+42,"lt%i",n) ;
      gfm2h(aboom[n],pth,pth+42) ;
    }
  */

  while(!quit)
  {
    generateMap() ;
    dead=win=fire=0 ;  gameTime=5<<16|48 ; lastTime=GetTickCount() ; bulletNb=1 ;

    while (invincible|boomEnabled|fire || !(dead|win))
    { if(GetTickCount() > lastTime + 984) // mlbrdy 4.1.984
      { int m=gameTime>>16 ,s=gameTime&0xFFFF ;
        if(!s){ if(m) { --m ; s=59 ; } else { dye(); } } else --s ;
        gameTime = (m<<16)|s ; lastTime=GetTickCount() ; change++ ;
        showPt ^= 1 ;
      }

     if(!(boomEnabled|fire)) {
      if(key[32]){ fire=1 ; showBullet(1) ; } // "space"
      if(!autoScroll) { static int release=0 ;
       if(key[16] && !key[17]) // "shift"
       { if(release) { release=0 ; gamePos^=1 ; change++ ; lx=ly=42 ; }
       } else release=1 ;
      }

      if(key[35] || key[79] || key[13]) // "fin" or "o" or "enter"
      { while(key[13]) ptc_update(pixel);
        showOption() ;
        while(key[13]) ptc_update(pixel);
      }

      if(key[17])
            if(key[72]) { invincible^=1 ; while(key[72]) ptc_update(pixel); ; }

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

          {     if(WalkArray(px,py)==0 && !(key[17])) { printf("\nBOOM !\nyou lose..") ; boomEnabled=1 ; WalkArray(px,py) = -1 ;  { dye(); } }
                else { switch(tankWay)
                       { case 0 : py++ ; break ;
                         case 1 : px-- ; break ;
                         case 2 : py-- ; break ;
                         case 3 : px++ ; break ;
                       };   ++change ;
                     }
          }
          if(autoScroll) gamePos = (py < mapSy>>1) ;
          lx=px ; ly=py ;
          zx = px<<4 ; zy = HEIGHT-((py+gamePos+1)<<4) ;
          mineAround=getNb(px,py) ; change++ ;
          if(mineAround){  if(WalkArray(px,py) != -1) WalkArray(px,py) = mineAround ; }
          else    { if(searchZero) searchForZero(px,py) ;
                    else           WalkArray(px,py) = -1;
                  }
          if(px==mapSx-1 && py==mapSy-1) ++win ;
        }
       if(change|boomEnabled|fire)
       {  memset(pixel+16*WIDTH*gamePos,0xFF,(fSIZE-(16*WIDTH))*4) ; // clrScr
          change=0 ;
          for(cx=0;cx<20;cx++)
           for(cy=0;cy<16;cy++)
            if(WalkArray(cx,cy)!=0)
            {  drawGfm(_case[1],cx<<4,HEIGHT-((cy+gamePos+1)<<4)) ;
               if(Map(cx,cy))
                  drawGfm(mine,(cx<<4)+1,HEIGHT-((cy+gamePos+1)<<4)-1) ;
               else if(WalkArray(cx,cy)>0)
                      if(tagArray) if(px != cx || py != cy)
                       drawGfm(chfnt[WalkArray(cx,cy)],(cx<<4)+4,(HEIGHT-((cy+gamePos+1)<<4))-3) ;
            }  else drawGfm(_case[0],cx<<4,HEIGHT-((cy+gamePos+1)<<4)) ;
          showBullet() ;
          drawGfm(grnTk[tankWay],zx+1,zy-1) ;
          showBoom(zx-12,zy-12) ;
       } else change=0 ;
          memset(pixel+gamePos*257*WIDTH,0xFF,15*WIDTH*4) ;
          memset(pixel+gamePos*258*WIDTH+1,0,WIDTH*4) ;
          memset(pixel+(16+gamePos*256)*WIDTH,0,WIDTH*4) ;
          infoFnt = chf ;
          if(revIfColor) infoFnt = chfI ;
          showInt(mineAround,zx,gamePos*256,0,infoFnt);
          showInt(gameTime>>16,(px < mapSx>>1)*268+16,gamePos*256,0,infoFnt) ;
          showInt(gameTime&0xFFFF,(px < mapSx>>1)*268+32,gamePos*256,1,infoFnt) ;
          if(showPt) drawGfm(letter[26],(px < mapSx>>1)*268+24,gamePos*256) ;
          if(!fire && bulletNb) drawGfm(bullet[0],(px < mapSx>>1)*214+52,gamePos*256) ;
          ptc_update(pixel);
    };
     int t=GetTickCount() ; inOption=1 ;
     while(!key[13] && !key[17] && !key[27] && !key[32] && !key[37] && !key[38] && !key[39] && !key[40] && t + 2420 > GetTickCount()) ptc_update(pixel);
     while(key[13] || key[17] || key[27] || key[32] || key[37] && key[38] && key[39] && key[40])  ptc_update(pixel);
     inOption=0 ;
  };
}

//0386864174

    /*for(int i=0;i<20;i++) { int nb = getNb(i,-1) ;
     if(nb) WalkArray(i,-1)= nb ; else WalkArray(i,-1)= -1 ;
     nb = getNb(i,mapSy) ;
     if(nb) WalkArray(i,mapSy)= nb ; else WalkArray(i,mapSy)= -1 ;
    } ;*/

    //int *mne = loadPcx("c:\\mine.pcx") ;
    //int *gmne = scanImg(mne,16,16,mne) ;
    //gfm2h(gmne,"c:\\mine.h","mine") ;

/*    int * ach[10] ; // pcx2h
    int * abm = loadPcx("c:\\bullet.pcx") ;
    char pth[42+21] ;
    int *aboom[9] ;      int trClr = 0x42004242 ;
    for(int n=0;n<4;n++)
    { aboom[n] = scanImg(abm+(16*16*n),16,16,abm) ;
      sprintf(pth   ,"c:\\bullet%i.h",n) ;
      sprintf(pth+42,"bullet%i",n) ;
      gfm2h(aboom[n],pth,pth+42) ;
    }
*/

