/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFloatSlider.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:00 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

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
															 const float &value, 
															 const float &minvalue, 
															 const float &maxvalue, 
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
float mmgFloatSlider::GetValue()
//----------------------------------------------------------------------------
{
  int pos = wxSlider::GetValue();
  assert (pos >=0 && pos <=this->Ticks );
  this->Value = this->Min + pos*this->Step;
  return this->Value;
}
//----------------------------------------------------------------------------
void mmgFloatSlider::SetValue(float value)
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
void mmgFloatSlider::SetMin(float min)
//----------------------------------------------------------------------------
{
  SetRange(min,this->Max);
}
//----------------------------------------------------------------------------
void mmgFloatSlider::SetMax(float max)
//----------------------------------------------------------------------------
{
  SetRange(this->Min,max);
}
//----------------------------------------------------------------------------
void mmgFloatSlider::SetRange(float min, float max, float value)
//----------------------------------------------------------------------------
{
	SetRange(min,max);
  SetValue(value);
}
//----------------------------------------------------------------------------
void mmgFloatSlider::SetRange(float min, float max)
//----------------------------------------------------------------------------
{
  if(abs(min)<1 || abs(max)<1 || (abs(max)-abs(min))<2 )
		DiscretizeRangeRange(min, max, this->DesiredTicks, &this->Min, &this->Max, &this->Step);
  else
	{
		DiscretizeRangeRange(min, max, this->DesiredTicks, &this->Min, &this->Max, &this->Step);
	}
	this->Ticks=ceil((this->Max-this->Min)/this->Step);
  wxSlider::SetRange(0,this->Ticks);
}
//----------------------------------------------------------------------------
float	mmgFloatSlider::Round(float val)
//----------------------------------------------------------------------------
{
	if (val == 0) return 1;

	int sign = (val > 0) ? 1 : -1;
	val = fabs(val);

	int   exponent = (int)(log10(val));
	float mantissa = val/pow(10,exponent); 
	if(mantissa <1) // fix for val< 1
	{
     mantissa *= 10;
		 exponent --;
	}
	if(mantissa<2) 
		mantissa=1;
  else if(mantissa<5)
	  mantissa=2;
  else 
	  mantissa=5;
	return sign*mantissa*pow(10,exponent);
}
//----------------------------------------------------------------------------
void mmgFloatSlider::DiscretizeRangeRange(float min,float max,float n, float *ra,float *rb,float *rs)
//----------------------------------------------------------------------------
{
  float step = Round((max-min)/n);

	float fmin = floor(min/step+0.5);
	int   imin = (int)(fmin);
	float rmin = step*imin;
	if(rmin < min) rmin += step;

	float fmax = floor(max/step+0.5);
	int   imax = (int)(fmax);
	float rmax = step*imax;
	if(rmax >= max) rmax -= step;

  *rs = step;
	*ra = rmin;
	*rb = rmax;
}
