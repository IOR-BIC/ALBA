/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFloatSlider.h,v $
  Language:  C++
  Date:      $Date: 2005-04-04 15:02:42 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgFloatSlider_H__
#define __mmgFloatSlider_H__

#include "mafDefines.h" //important: mafDefines should always be included as first

//#ifdef __WIN32__
//  #define FLOAT_SLIDER_PARENT wxSlider95
//#else
  #define FLOAT_SLIDER_PARENT wxSlider
//#endif	

//----------------------------------------------------------------------------
// mmgFloatSlider
/**  
a Slider that can be used to set float values,
mmgFloatSlider send usual wxWindows Events;
Use mmgFloatSlider in combination with a mmgValidator
to receive notification as mafEvents.
*/
//----------------------------------------------------------------------------
class mmgFloatSlider: public FLOAT_SLIDER_PARENT
{
    DECLARE_DYNAMIC_CLASS(mmgFloatSlider)
public:
	
  mmgFloatSlider::mmgFloatSlider( wxWindow *parent=NULL, 
																	wxWindowID id=-1, 
																	const float &value=0, 
																	const float &minvalue=0, 
																	const float &maxvalue=1,
																	const wxPoint &position=wxDefaultPosition, 
																	const wxSize& size=wxDefaultSize, 
																	long style=wxSL_HORIZONTAL );
  ~mmgFloatSlider();
  
  /** Set the slider value. */
	void SetValue(float value);
  
  /** Get the slider value. */
  float GetValue();
  
  /** Set the slider range and value. */
	void SetRange(float min,float max,float value);
  
  /** Set the slider range. */
	void SetRange(float min,float max);
  
  /** Set the number of slider's steps. */
	void SetNumberOfSteps(int num);
  
  /** Get the number of slider's steps. */
	int GetNumberOfSteps() {return this->Ticks;};
  
  /** Get the slider's step value. */
  float GetStep() {return this->Step;};
  
  /** Set the slider's max value. */
	void SetMax(float max);
  
  /** Get the slider's max value. */
  float GetMax() {return this->Max;};
  
  /** Set the slider's min value. */
	void SetMin(float min);
  
  /** Get the slider's min value. */
  float GetMin() {return this->Min;};

protected:
	float	Round(float val);
  float	Pow10(float exponent);
	void DiscretizeRangeRange(float min,float max,float n, float *ra,float *rb,float *rs);

	int DesiredTicks;
	int Ticks;
	
	float Value;
	float Step;
	float Max;
	float Min;
};
#endif
