/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafColor.h,v $
  Language:  C++
  Date:      $Date: 2007-09-04 16:22:15 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafColor_H__
#define __mafColor_H__

//----------------------------------------------------------------------------
/** 
mafColor is a class representing an RGBA color:

- Provides conversion among RGB and HSV as static member functions.

- Contain both RGBA and HSV representation of a colour.
  RGB,SV in range [0..255], H in range [0..360].
  If updated through the various Set methods, then the two representation 
  are kept in sync. This provide a kind of implicit conversion.

- Provide conversion among the usual color format ( rgb on [0..255] ),
  the vtk format ( rgba on [0..1] ) and wxWindows ( wxColour ). These are accepted
  from all the Set,Get and Constructors.

- Provide weighted interpolation among two colors. Both in RGB and HSV space. 
  
- Provide a visual representation of a transparent color. 
  Filling the pixels (x,y) of a bitmap with mafColor::CheckeredColor(color, x,y) 
  will draw an image with a checkered pattern.
  The pattern visibility is proportional with the alpha value.
*/
class mafColor
//----------------------------------------------------------------------------
{
  public:
  /** default ctor*/
  mafColor();
  /** ctor accepting an integer RGBA */
  mafColor( int r, int g, int b, int a=0 ); 
  /** ctor accepting an vtk color */
  mafColor( double *vtkRGBA ); 
  /** ctor accepting an wxColour */
  mafColor( wxColour col ); 

  /** Set using wxColour */ 
  void     Set( wxColour col );
  /** Get returning wxColour */ 
  wxColour Get();
  
  /** Set using integer RGBA - components in [0..255] */
  void SetRGB( int r, int g, int b, int a=-1 );
  /** Get returning integer RGBA - components in [0..255] */
  void GetRGB( int *r, int *g, int *b, int *a= NULL );

  /** Set using integer HSV - H in [0..360] ,S,V in [0..255] */
  void SetHSV(int h, int s, int v);
  /** Set returning integer HSV - H in [0..360] ,S,V in [0..255] */
  void GetHSV(int *h, int *s, int *v);

  /** Set using double RGBA - components normalized on [0,1] 
  - o is the opacity == 1-alpha */
  void SetFloatRGB( double r, double g, double b, double o=-1 );  
  /** Get returning double RGBA - components normalized on [0,1] 
  - o is the opacity == 1-alpha*/
  void GetFloatRGB( double *r, double *g, double *b, double *o ); 

  /** Force updating the RGB representation. 
  - useful if the HSV member variable are set directly */
  void HSVToRGB(); 
  /** Force updating the HSV representation. 
  - useful if the RGB member variable are set directly */
  void RGBToHSV();

  /** static function - conversion from RGB to HSV, all represented on Integer */ 
  static void RGBToHSV(int r, int g, int b, int *h, int *s, int *v);
  /** static function - conversion from HSV to RGB, all represented on Integer */ 
  static void HSVToRGB(int h, int s, int v, int *r, int *g, int *b);

  /** static function - Interpolate two color using a weight t in HSV space. t=0 return c1, t=1 return c2 */ 
  static mafColor InterpolateHSV(mafColor c1, mafColor c2, double t);
  /** static function - Interpolate two color using a weight t in RGB space. t=0 return c1, t=1 return c2 */ 
  static mafColor InterpolateRGB(mafColor c1, mafColor c2, double t);

  /** static function - used to visually render a transparent color.
  The resulting color depend on the position, and form a checkered pattern */ 
  static mafColor CheckeredColor(mafColor c, int x, int y)
  {
    static mafColor check0 = mafColor(128,128,128);
    static mafColor check1 = mafColor(255,255,255);
  
    int a = x%16 >= 8;
    int b = y%16 >= 8;
    if( a+b == 1 )
      return mafColor::InterpolateRGB(c,check0,c.m_Alpha/255.0);
    else
      return mafColor::InterpolateRGB(c,check1,c.m_Alpha/255.0);
  }  

  // member variables 
  int m_Red,m_Green,m_Blue; ///<  rgb color representation
  int m_Hue,m_Saturation,m_Value; ///<  hsv color representation
  int m_Alpha;         ///<  alpha value
};
#endif
