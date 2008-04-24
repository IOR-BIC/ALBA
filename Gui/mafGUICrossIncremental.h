/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUICrossIncremental.h,v $
  Language:  C++
  Date:      $Date: 2008-04-24 08:39:51 $
  Version:   $Revision: 1.3 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2008
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUICrossIncremental_H__
#define __mafGUICrossIncremental_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmgPanel.h"
#include "mafObserver.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class mmgButton;

//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------

#define MININT    -32000
#define MAXINT     32000
#define MINFLOAT  -2000000000 
#define MAXFLOAT   2000000000
#define MINDOUBLE -1.0e+299
#define MAXDOUBLE 1.0e+299


//----------------------------------------------------------------------------
// mafGUICrossIncremental :
/**
Class that represent a widget with 4 buttons and a text array disposed like a cross
It can use two accumulator variable that are updated. It needed absolutely step variable pointer.
After modification, it returns to listener an event with in Arg the sign of increment.
*/
//----------------------------------------------------------------------------
class mafGUICrossIncremental: public mmgPanel , public mafObserver
{
public:
  mafGUICrossIncremental(wxWindow* parent, 
                  wxWindowID id,
                  const char * label,
                  double *stepVariable,
									double *topBottomVariable,
									double *leftRightVariable,
                  bool boldLabel = true,
									int modality = ID_COMPLETE_LAYOUT,
                  const wxPoint& pos = wxDefaultPosition, 
                  const wxSize& size = wxDefaultSize,
                  double min = MINFLOAT, double max = MAXFLOAT, int decimal_digit = -1,
                  long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN);

  virtual ~mafGUICrossIncremental();

  
  enum MODALITY_CROSS_INCREMENTAL_ID
  {
    ID_TOP_BOTTOM_LAYOUT = 0,
    ID_LEFT_RIGHT_LAYOUT,
    ID_COMPLETE_LAYOUT,
  };

  enum MODALITY_OPERATION_ID
  {
    ID_DECREMENT = -1,
    ID_CHANGE_STEP = 0,
    ID_INCREMENT = 1
  };
  
  enum CROSS_INCREMENTAL_MODE_ID
  {
    ID_BUTTON_TOP = MINID,
    ID_BUTTON_LEFT,
    ID_BUTTON_RIGHT,
    ID_BUTTON_BOTTOM,
    ID_STEP_ENTRY,
  };

  void CreateWidgetTopBottom();
	void CreateWidgetLeftRight();
	void CreateWidgetTextEntry(double min, double max, int decimal_digit);
	
	void EnableStep(bool value);

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent(mafEventBase *maf_event);

  int IsIncrement(){return m_Increment;};
  double GetTopBottomVariation(){return m_TopBottomVariation;};
  double GetLeftRightVariation(){return m_LeftRightVariation;};


  void SetStepVariable(double step){if(m_StepText) *m_StepVariable = step;m_StepText->SetValue(wxString::Format("%.2f", *m_StepVariable));}

  void LayoutStyle(const char *label);

private:

  mmgButton    *m_ButtonTop;
  mmgButton    *m_ButtonBottom;
  mmgButton    *m_ButtonLeft;
  mmgButton    *m_ButtonRight;
  wxTextCtrl   *m_StepText;

	wxBoxSizer      *m_Sizer;

  double            *m_TopBottomVariable; ///< Flag used to distinguish top-bottom variable
  double            *m_LeftRightVariable; ///< Flag used to distinguish left-right variable

  double            m_TopBottomVariation;
  double            m_LeftRightVariation;

  bool m_Bold;
  int m_Increment;

  double            *m_StepVariable;
  mafObserver*      m_Listener;
	int               m_IdLayout;

  DECLARE_EVENT_TABLE()
};
#endif
