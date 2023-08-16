/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIFloatSlider
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


#include "albaGUIFloatSlider.h"
#include "math.h"

//#ifdef __WIN32__
//  IMPLEMENT_DYNAMIC_CLASS(albaGUIFloatSlider, wxSlider95)
//#else
  IMPLEMENT_DYNAMIC_CLASS(albaGUIFloatSlider, wxSlider)
//#endif	


//----------------------------------------------------------------------------
// albaGUIFloatSlider
//----------------------------------------------------------------------------
albaGUIFloatSlider::albaGUIFloatSlider( wxWindow *parent, 
															 wxWindowID id, 
															 const double &value, 
															 const double &minvalue, 
															 const double &maxvalue, 
															 const wxPoint &position, 
															 const wxSize &size, 
															 long style ):
FLOAT_SLIDER_PARENT( parent, id, 0, 0, 1, position, size, style )
{
	this->m_DesiredTicks = 50;
  SetRange(minvalue,maxvalue,value);
}
//----------------------------------------------------------------------------
albaGUIFloatSlider::~albaGUIFloatSlider()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIFloatSlider::SetNumberOfSteps(int num)
//----------------------------------------------------------------------------
{
  this->m_DesiredTicks = num;
	SetRange(this->m_Min,this->m_Max);
}
//----------------------------------------------------------------------------
double albaGUIFloatSlider::GetValue()
//----------------------------------------------------------------------------
{
   return this->m_Value;
}

//----------------------------------------------------------------------------
double albaGUIFloatSlider::GetSliderValue()
//----------------------------------------------------------------------------
{
	int pos = wxSlider::GetValue();
	assert(pos >= 0 && pos <= this->m_Ticks);
	if (pos == 0)
		m_Value = m_Min;
	else if (pos == m_Ticks)
		m_Value = m_Max;
	else
		this->m_Value = this->m_Min + pos*this->m_Step;
	return this->m_Value;
}
//----------------------------------------------------------------------------
void albaGUIFloatSlider::SetValue(double value)
//----------------------------------------------------------------------------
{
  this->m_Value = value;
	if(this->m_Value < this->m_Min) this->m_Value = this->m_Min;
	if(this->m_Value > this->m_Max) this->m_Value = this->m_Max;

	//value has to be discretized
	//this->m_Value = m_Step*(int)floor(m_Value/m_Step+0.5);

	wxSlider::SetValue(floor((value-this->m_Min)/this->m_Step+0.5));
}
//----------------------------------------------------------------------------
void albaGUIFloatSlider::SetMin(double min)
//----------------------------------------------------------------------------
{
  SetRange(min,this->m_Max);
}
//----------------------------------------------------------------------------
void albaGUIFloatSlider::SetMax(double max)
//----------------------------------------------------------------------------
{
  SetRange(this->m_Min,max);
}
//----------------------------------------------------------------------------
void albaGUIFloatSlider::SetRange(double min, double max, double value)
//----------------------------------------------------------------------------
{
	SetRange(min,max);
  SetValue(value);
}
//----------------------------------------------------------------------------
void albaGUIFloatSlider::SetRange(double min, double max)
//----------------------------------------------------------------------------
{
  //if(abs(min)<1 || abs(max)<1 || (abs(max)-abs(min))<2 )
		DiscretizeRangeRange(min, max, this->m_DesiredTicks, &this->m_Min, &this->m_Max, &this->m_Step);
  //else
	//{
    //m_Max = max;
    //m_Min = min;
    //m_Step = (max-min)/this->m_DesiredTicks;
	//}
	this->m_Ticks=ceil((this->m_Max-this->m_Min)/this->m_Step);
  wxSlider::SetRange(0,this->m_Ticks);
}
//----------------------------------------------------------------------------
double	albaGUIFloatSlider::Pow10(double exponent)
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
double	albaGUIFloatSlider::Round(double val)
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
void albaGUIFloatSlider::DiscretizeRangeRange(double min,double max,double n, double *ra,double *rb,double *rs)
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
	*ra = min;
	*rb = max;
}
