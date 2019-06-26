/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaColor
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaColor.h" 

//----------------------------------------------------------------------------
albaColor::albaColor()
//----------------------------------------------------------------------------
{
  m_Red = m_Green = m_Blue = m_Hue = m_Saturation = m_Value =0;
  m_Alpha = 0;
}
//----------------------------------------------------------------------------
albaColor::albaColor( int r, int g, int b, int a )
//----------------------------------------------------------------------------
{
  SetRGB(r,g,b,a);
}
//----------------------------------------------------------------------------
albaColor::albaColor( double *vtkRGBA )
//----------------------------------------------------------------------------
{
  assert(vtkRGBA);
  SetFloatRGB( vtkRGBA[0], vtkRGBA[1], vtkRGBA[2], vtkRGBA[3]);
}
//----------------------------------------------------------------------------
albaColor::albaColor( wxColour col )
//----------------------------------------------------------------------------
{
  m_Alpha = 0;
  Set( col );
}
//----------------------------------------------------------------------------
void albaColor::Set( wxColour col )
//----------------------------------------------------------------------------
{
  SetRGB( col.Red(), col.Green(), col.Blue() );
}
//----------------------------------------------------------------------------
void albaColor::SetRGB( int r, int g, int b, int a )
//----------------------------------------------------------------------------
{
  if( r<0 ) r=0; if( r>255 ) r=255; 
  if( g<0 ) g=0; if( g>255 ) g=255; 
  if( b<0 ) b=0; if( b>255 ) b=255; 
  
  m_Red = r; m_Green = g; m_Blue =b;
  if(a != -1) 
  {
    if( a<0 ) a=0; if( a>255 ) a=255; 
    m_Alpha = a;
  }
  RGBToHSV();
}
//----------------------------------------------------------------------------
void albaColor::GetRGB( int *r, int *g, int *b, int *a )
//----------------------------------------------------------------------------
{
  *r = m_Red; *g = m_Green;  *b =m_Blue;
  if( a != NULL ) *a = m_Alpha;
}
//----------------------------------------------------------------------------
void albaColor::SetHSV(int h, int s, int v)
//----------------------------------------------------------------------------
{
  if( s<0 ) s=0; if( s>255 ) s=255; 
  if( v<0 ) v=0; if( v>255 ) v=255; 
  while(h<0)   h+= 360;
  while(h>360) h-= 360;
  
  m_Hue = h; m_Saturation = s; m_Value =v;
  HSVToRGB();
}
//----------------------------------------------------------------------------
void albaColor::GetHSV(int *h, int *s, int *v)
//----------------------------------------------------------------------------
{
  *h = m_Hue; *s = m_Saturation;  *v =m_Value;
}
//----------------------------------------------------------------------------
void albaColor::SetFloatRGB( double r, double g, double b, double o )
//----------------------------------------------------------------------------
{
  if(o==-1)
    SetRGB( r*255, g*255, b*255 );
  else
    SetRGB( r*255, g*255, b*255, (1-o)*255 );
}
//----------------------------------------------------------------------------
void albaColor::GetFloatRGB( double *r, double *g, double *b, double *o )
//----------------------------------------------------------------------------
{
   *r = m_Red / 255.0;
   *g = m_Green / 255.0;
   *b = m_Blue / 255.0;
   *o = 1 - m_Alpha / 255.0;
}
//----------------------------------------------------------------------------
// rgb,sv in range [0..255], h in range [0..360]
void albaColor::RGBToHSV(int r, int g, int b, int *h, int *s, int *v)
//----------------------------------------------------------------------------
{
   float max = r; 
   if (max < g ) max = g;
   if (max < b ) max = b;

   float min = r; 
   if (min > g ) min = g;
   if (min > b ) min = b;

   *h = 0;
   *s = max-min;
   *v = max;

   if(s==0) return; 

   float delta = max - min;
   float H;
			if (delta == 0) H = 0;
	 else if (max == r) H = 0 + (g-b)/delta;
   else if (max == g) H = 2 + (b-r)/delta;
   else if (max == b) H = 4 + (r-g)/delta;

   *h = H * 60;
   if (*h<0) *h+=360;
}
//----------------------------------------------------------------------------
// rgb,sv in range [0..255], h in range [0..360]
void albaColor::HSVToRGB(int h, int s, int v, int *r, int *g, int *b)
{
  *r = *g = *b = v;
  if (s == 0) return;
  
  if (h == 360) h=0;

  float H = h / 60.0;  // H is in [0..6)
  float S = s / 255.0; // S is in [0..1]

  int i=0;  // i is the largest integer <= H
  if( H>=1 ) i=1;
  if( H>=2 ) i=2;
  if( H>=3 ) i=3;
  if( H>=4 ) i=4;
  if( H>=5 ) i=5;
  
  float F = H-i; // f is the fractional part of fh;

  float p,q,t;
  p = v * (1.0 - S         );
  q = v * (1.0 - S * F     );
  t = v * (1.0 - S * (1-F) );

  switch(i) 
  {
    case 0: *r=v; *g=t; *b=p ; break;
    case 1: *r=q; *g=v; *b=p ; break;
    case 2: *r=p; *g=v; *b=t ; break;
    case 3: *r=p; *g=q; *b=v ; break;
    case 4: *r=t; *g=p; *b=v ; break;
    case 5: *r=v; *g=p; *b=q ; break;
  } 
}

//----------------------------------------------------------------------------
void albaColor::RGBToHSL(int r, int g, int b, int *h, int *s, int *l)
{
	double themin,themax,delta;
	double dr,dg,db,dh,ds,dl;

	dr=r/255.0;
	dg=g/255.0;
	db=b/255.0;


	themin = MIN(dr,MIN(dg,db));
	themax = MAX(dr,MAX(dg,db));
	delta = themax - themin;
	dl = (themin + themax) / 2;
	ds = 0;
	if (dl > 0 && dl < 1)
		ds = delta / (dl < 0.5 ? (2*dl) : (2-2*dl));
	dh = 0;
	if (delta > 0) {
		if (themax == dr && themax != dg)
			dh += (dg - db) / delta;
		if (themax == dg && themax != db)
			dh += (2 + (db - dr) / delta);
		if (themax == db && themax != dr)
			dh += (4 + (dr - dg) / delta);
		dh *= 60;
	}
	
	*h=dh;
	*s=ds*255;
	*l=dl*255;
}

//----------------------------------------------------------------------------
void albaColor::HSLToRGB(int h, int s, int l, int *r, int *g, int *b)
{
	double dr,dg,db,satr,satg,satb,ctmpr,ctmpg,ctmpb;
	double dh,ds,dl;
	dh=h;
	ds=s/255.0;
	dl=l/255.0;
	
	if (dh < 120) {
		satr = (120 - dh) / 60.0;
		satg = dh / 60.0;
		satb = 0;
	} else if (dh < 240) {
		satr = 0;
		satg = (240 - dh) / 60.0;
		satb = (dh - 120) / 60.0;
	} else {
		satr = (dh - 240) / 60.0;
		satg = 0;
		satb = (360 - dh) / 60.0;
	}
	satr = MIN(satr,1.0);
	satg = MIN(satg,1.0);
	satb = MIN(satb,1.0);

	ctmpr = 2 * ds * satr + (1 - ds);
	ctmpg = 2 * ds * satg + (1 - ds);
	ctmpb = 2 * ds * satb + (1 - ds);

	if (dl < 0.5) {
		dr = dl * ctmpr;
		dg = dl * ctmpg;
		db = dl * ctmpb;
	} else {
		dr = (1 - dl) * ctmpr + 2 * dl - 1;
		dg = (1 - dl) * ctmpg + 2 * dl - 1;
		db = (1 - dl) * ctmpb + 2 * dl - 1;
	}

	*r=dr*255;
	*g=dg*255;
	*b=db*255;
}



//----------------------------------------------------------------------------
void albaColor::HSVToRGB() 
//----------------------------------------------------------------------------
{
  HSVToRGB( m_Hue, m_Saturation, m_Value, &m_Red, &m_Green, &m_Blue );  
}
//----------------------------------------------------------------------------
void albaColor::RGBToHSV()
//----------------------------------------------------------------------------
{
  RGBToHSV( m_Red, m_Green, m_Blue, &m_Hue, &m_Saturation, &m_Value );  
}
//----------------------------------------------------------------------------
albaColor albaColor::InterpolateHSV(albaColor c1, albaColor c2, double t)
//----------------------------------------------------------------------------
{
  if( ( c2.m_Hue - c1.m_Hue )>180 )  c1.m_Hue += 360;
  if( ( c1.m_Hue - c2.m_Hue )>180 )  c2.m_Hue += 360;
  
  albaColor c;
  c.m_Hue = c1.m_Hue*(1-t) + c2.m_Hue*t; if(c.m_Hue > 360 ) c.m_Hue -= 360; 
  c.m_Saturation = c1.m_Saturation*(1-t) + c2.m_Saturation*t; 
  c.m_Value = c1.m_Value*(1-t) + c2.m_Value*t; 
  c.m_Alpha = c1.m_Alpha*(1-t) + c2.m_Alpha*t; 
  c.HSVToRGB();
  return c;    
}  
//----------------------------------------------------------------------------
albaColor albaColor::InterpolateRGB(albaColor c1, albaColor c2, double t)
//----------------------------------------------------------------------------
{
  albaColor c;
  c.m_Red = c1.m_Red*(1-t) + c2.m_Red*t;
  c.m_Green = c1.m_Green*(1-t) + c2.m_Green*t; 
  c.m_Blue = c1.m_Blue*(1-t) + c2.m_Blue*t; 
  c.m_Alpha = c1.m_Alpha*(1-t) + c2.m_Alpha*t; 
  c.RGBToHSV();
  return c;    
}  
/*
//----------------------------------------------------------------------------
albaColor albaColor::CheckeredColor(albaColor c, int x, int y)
//----------------------------------------------------------------------------
{
  static albaColor check0 = albaColor(128,128,128);
  static albaColor check1 = albaColor(255,255,255);
  
  int a = x%16 >= 8;
  int b = y%16 >= 8;
  if( a+b == 1 )
    return albaColor::InterpolateRGB(c,check0,c.m_Alpha/255.0);
  else
    return albaColor::InterpolateRGB(c,check1,c.m_Alpha/255.0);
}  
*/
