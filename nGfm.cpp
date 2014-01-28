// transparent 16bit blit routine for Gdl²  - yet not optimised, i don't got a gp ... :/
// (C) 2k4 r043v
// take care, this Gfm format is'nt the same used for 8 bit mode.
// code to convert an 32 bit Gfm to a rotated 16 bit Gfm is in the pc release.

void udrawGfm(ushort *Gfm, short * scr)  // draw a Gfm unclipped on x
{   short sy = Gfm[3] ;    Gfm+=5 ;
    short * scrEnd = pixel + WIDTH*HEIGHT - 1 ;
    int lnb, sze ;
    if(scr > scrEnd) return ; // out of screen at up
    int upClip = (scr + sy*WIDTH > scrEnd) ; // is clipped at up ?
    
    if(scr < pixel) // is clipped at down ?
    {  if(scr + sy*WIDTH < pixel) return ; // out of screen at down
       do{    lnb = *Gfm++ ;
              while(lnb--) { Gfm++ ; Gfm += *Gfm ; Gfm++ ; };
              scr += WIDTH ; sy-- ;
       } while(scr < pixel) ;
    }

    short *screen = scr ; int c = 1 ;

    while(c < sy)
    {  lnb = *Gfm++ ;
       while(lnb--) {	scr += *Gfm++ ; sze = *Gfm++ ;
                      		memcpy(scr,Gfm,sze*2) ;
                      		Gfm += sze ;    scr += sze ;
       };  			scr = screen + WIDTH*(c++) ;
       if(upClip) if(scr > scrEnd) return ;
    };
}

void drawGfm(ushort *Gfm, int y, int x) // first is x, second is y, due to gp screen is 90° rotated
{    int sx = Gfm[4]; int sy = Gfm[3];
     short * scrEnd = pixel + WIDTH*HEIGHT - 1 ;
     if(x >= 0 && x + sx < WIDTH){ udrawGfm((ushort*)Gfm,pixel + y*WIDTH + x) ; return ; }

     if(x < -sx || x > WIDTH) return ;// out of screen on x
     if(x >= 0) // clip right only
     {   int max = WIDTH-x ;  int lnb, sze ;
         short * scr = pixel + y*WIDTH + x ;
         if(scr > scrEnd) return ; // out of screen at up
         int upClip = (scr + sy*WIDTH > scrEnd) ; // is clipped at up ?
         Gfm += 5 ;
         if(scr < pixel) // is clipped at down ?
         {  if(scr + sy*WIDTH < pixel) return ; // out of screen at down
            do{ lnb = *Gfm++ ;
                while(lnb--) { Gfm++ ; Gfm += *Gfm ; Gfm++ ; };
                scr += WIDTH ; sy-- ;
            } while(scr < pixel) ;
         }

         short *screen = scr ; int c = 1 ; short * lend ;
         while(c < sy)
         {  lnb = *Gfm++ ; lend = scr + max ;
            while(lnb--) { scr += *Gfm++ ; sze = *Gfm++ ;
                           if(scr + sze < lend) memcpy(scr,Gfm,sze*pixelSize) ;
                           else if(scr < lend)  memcpy(scr,Gfm,(lend-scr)*pixelSize) ;
                           Gfm += sze ;    scr+=sze ;
            };  scr = screen + WIDTH*(c++) ;
            if(upClip) if(scr > scrEnd) return ;
         };
     } else if(x+sx < WIDTH) { // clip left only
         int lnb, sze ; short *s = pixel + y*WIDTH ; short *scr = s+x ;
         if(s > scrEnd) return ; // out of screen at up
         int upClip = (s + sy*WIDTH > scrEnd) ; // is clipped at up ?
         Gfm += 5 ;
         if(s < pixel) // is clipped at down ?
         {  if(s + sy*WIDTH < pixel) return ; // out of screen at down
            do{ lnb = *Gfm++ ;
                while(lnb--) { Gfm++ ; Gfm += *Gfm ; Gfm++ ; };
                s += WIDTH ; sy-- ;
            } while(s < pixel) ;
         }
         scr = s+x ; short *screen = scr ; int c=1 ; int size ;
         while(c < sy)
         {  lnb = *Gfm++ ;
            while(lnb--) { scr += *Gfm++ ; sze = *Gfm++ ;
                           if(scr >= s)   memcpy(scr,Gfm,sze*pixelSize) ;
                           else if(scr + sze > s) { size = (scr + sze)-s ;
                                                    memcpy(s,Gfm+(sze-size),size*pixelSize) ;
                                                  }
                           Gfm += sze ;    scr+=sze ;
            };  scr = screen + WIDTH*(c++) ; s = scr-x ;
            if(upClip) if(s > scrEnd) return ;
         };
     } else return ;
}

void rotate90r(int *s, int *d, int sx, int sy)
{	int cptx=sx ; int * dd = d ; d+=(sx-1) ;
	while(cptx--)
	{	for(int cpty=0;cpty<sy;cpty++)
		{	*d = *s++ ; d+=sx ;	};
		d = dd+(cptx-1) ;
	};
}

void reverseGfmColor(ushort *Gfm)
{ int sy = Gfm[3] ;
  int cnt=0, c, size ; ushort *gPtr = Gfm + 5 ;
  while(cnt<sy)
  {    c = *gPtr++ ;  ++cnt ;
       while(c--) { gPtr++ ; size = *gPtr++ ;
       			if(size > 42) return ;
                    for(int i=0;i<size;i++)
                    { int r,g,b,color=*gPtr ;
                      r = 31-((color>>11)&31) ;
                      g = 31-((color>>6)&31) ;
                      b = 31-((color>>1)&31) ;
                      *gPtr++ = (r<<11)|(g<<6)|(b<<1)|1 ;
                    };
       };
  };
}

#ifdef USE_PCX
#endif
/*
void vDrawLine(int x, int y, int size, short color)
{ if(x < 0 || x > WIDTH) return ;
  if(y < 0 || y+size > HEIGHT) return ;
  short *s = pixel + WIDTH*y+x ;
  while(size--) { *s = color ; s+=WIDTH ; }
}

void hDrawLine(int x, int y, int size, short color)
{ if(x < 0 || x+size > WIDTH) return ;
  if(y < 0 || y > HEIGHT) return ;
  short *s = pixel + WIDTH*y+x ;
  while(size--) *s++ = color ;
}
*/

//0494633092

/*
*  necromancide
*  aristacrat
*  amiga lort
*  the product/the core
*  introfronty
*  perhaps2-dia
*  miko sampler
*  a shotgun named paul
*  crystal summer/hammer
*  fuck it all
  left behind
  giana sister
*  class installer
*  close tte compovers
  fairlight
  enjoy the silence/remix
*  mm?
*  sonic invasion
*  sugar melody
*  wicked surfah
*  reggadelia -smile-
*  the song
*  tubularbells
*  the alleycat
*  maudit chrono
*  hit the deck
*  6th.toonbck2
*  kikka-waltz
*  the last kuusnepa
  longing for techno 2
*  lullaby
*/

