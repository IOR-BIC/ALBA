/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFloatSlider.h,v $
  Language:  C++
  Date:      $Date: 2005-06-28 09:51:06 $
  Version:   $Revision: 1.4 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgFloatSlider_H__
#define __mmgFloatSlider_H__

//#ifdef __WIN32__
//  #define FLOAT_SLIDER_PARENT wxSlider95
//#else
  #define FLOAT_SLIDER_PARENT wxSlider
//#endif	
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// mmgFloatSlider
/**  
a Slider that can be used to set double values,
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
																	const double &value=0, 
																	const double &minvalue=0, 
																	const double &maxvalue=1,
																	const wxPoint &position=wxDefaultPosition, 
																	const wxSize& size=wxDefaultSize, 
																	long style=wxSL_HORIZONTAL );
  ~mmgFloatSlider();
  
  /** Set the slider value. */
	void SetValue(double value);
  
  /** Get the slider value. */
  double GetValue();
  
  /** Set the slider range and value. */
	void SetRange(double min,double max,double value);
  
  /** Set the slider range. */
	void SetRange(double min,double max);
  
  /** Set the number of slider's steps. */
	void SetNumberOfSteps(int num);
  
  /** Get the number of slider's steps. */
	int GetNumberOfSteps() {return this->Ticks;};
  
  /** Get the slider's step value. */
  double GetStep() {return this->Step;};
  
  /** Set the slider's max value. */
	void SetMax(double max);
  
  /** Get the slider's max value. */
  double GetMax() {return this->Max;};
  
  /** Set the slider's min value. */
	void SetMin(double min);
  
  /** Get the slider's min value. */
  double GetMin() {return this->Min;};

protected:
	double	Round(double val);
  double	Pow10(double exponent);
	void DiscretizeRangeRange(double min,double max,double n, double *ra,double *rb,double *rs);

	int DesiredTicks;
	int Ticks;
	
	double Value;
	double Step;
	double Max;
	double Min;
};
#endif
