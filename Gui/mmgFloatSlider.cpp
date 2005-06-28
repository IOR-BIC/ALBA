/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFloatSlider.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-28 09:51:05 $
  Version:   $Revision: 1.4 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mmgFloatSlider.h"
#include "math.h"

//#ifdef __WIN32__
//  IMPLEMENT_DYNAMIC_CLASS(mmgFloatSlider, wxSlider95)
//#else
  IMPLEMENT_DYNAMIC_CLASS(mmgFloatSlider, wxSlider)
//#endif	


//----------------------------------------------------------------------------
// mmgFloatSlider
//----------------------------------------------------------------------------
mmgFloatSlider::mmgFloatSlider( wxWindow *parent, 
															 wxWindowID id, 
															 const double &value, 
															 const double &minvalue, 
															 const double &maxvalue, 
															 const wxPoint &position, 
															 const wxSize &size, 
															 long style ):
FLOAT_SLIDER_PARENT( parent, id, 0, 0, 1, position, size, style )
{
	this->DesiredTicks = 50;
  SetRange(minvalue,maxvalue,value);
}
//----------------------------------------------------------------------------
mmgFloatSlider::~mmgFloatSlider()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgFloatSlider::SetNumberOfSteps(int num)
//----------------------------------------------------------------------------
{
  this->DesiredTicks = num;
	SetRange(this->Min,this->Max);
}
//----------------------------------------------------------------------------
double mmgFloatSlider::GetValue()
//----------------------------------------------------------------------------
{
  int pos = wxSlider::GetValue();
  assert (pos >=0 && pos <=this->Ticks );
  this->Value = this->Min + pos*this->Step;
  return this->Value;
}
//----------------------------------------------------------------------------
void mmgFloatSlider::SetValue(double value)
//----------------------------------------------------------------------------
{
  this->Value = value;
	if(this->Value < this->Min) this->Value = this->Min;
	if(this->Value > this->Max) this->Value = this->Max;

	//value has to be discretized
	this->Value = Step*(int)floor(Value/Step+0.5);

	wxSlider::SetValue(ceil((value-this->Min)/this->Step));
}
//----------------------------------------------------------------------------
void mmgFloatSlider::SetMin(double min)
//----------------------------------------------------------------------------
{
  SetRange(min,this->Max);
}
//----------------------------------------------------------------------------
void mmgFloatSlider::SetMax(double max)
//----------------------------------------------------------------------------
{
  SetRange(this->Min,max);
}
//----------------------------------------------------------------------------
void mmgFloatSlider::SetRange(double min, double max, double value)
//----------------------------------------------------------------------------
{
	SetRange(min,max);
  SetValue(value);
}
//----------------------------------------------------------------------------
void mmgFloatSlider::SetRange(double min, double max)
//----------------------------------------------------------------------------
{
  //if(abs(min)<1 || abs(max)<1 || (abs(max)-abs(min))<2 )
		DiscretizeRangeRange(min, max, this->DesiredTicks, &this->Min, &this->Max, &this->Step);
  //else
	//{
    //Max = max;
    //Min = min;
    //Step = (max-min)/this->DesiredTicks;
	//}
	this->Ticks=ceil((this->Max-this->Min)/this->Step);
  wxSlider::SetRange(0,this->Ticks);
}
//----------------------------------------------------------------------------
double	mmgFloatSlider::Pow10(double exponent)
//----------------------------------------------------------------------------
{
   //pow dont work with negative exponent -- 
   if(exponent == 0)
     return 1;
   if(exponent > 0)
     return pow((double)10.0,exponent);
   else
     return 1/pow((double)10.0,-exponent);
}
//----------------------------------------------------------------------------
double	mmgFloatSlider::Round(double val)
//----------------------------------------------------------------------------
{
	if (val == 0) return 1;

	int sign = (val > 0) ? 1 : -1;
	val = fabs(val);

  // strange: (int)(-1f) = 0 --- anyway,it is fixed later
  int   exponent = (int)log10(val);
  double mantissa = val/Pow10(exponent); 

	if(mantissa <1) // fix for val< 1
	{
     mantissa *= 10;
		 exponent --;
	}
  // discretize the mantissa
  if(mantissa<2) 
		mantissa=1;
  else if(mantissa<5)
	  mantissa=2;
  else 
	  mantissa=5;
	
  // rebuild the number
  return sign*mantissa*Pow10(exponent);
}
//----------------------------------------------------------------------------
void mmgFloatSlider::DiscretizeRangeRange(double min,double max,double n, double *ra,double *rb,double *rs)
//----------------------------------------------------------------------------
{
  double step = Round((max-min)/n);

	double fmin = floor(min/step+0.5);
	int   imin = (int)(fmin);
	double rmin = step*imin;
	if(rmin < min) rmin += step;

	double fmax = floor(max/step+0.5);
	int   imax = (int)(fmax);
	double rmax = step*imax;
	if(rmax > max) rmax -= step;

  *rs = step;
	*ra = rmin;
	*rb = rmax;
}
