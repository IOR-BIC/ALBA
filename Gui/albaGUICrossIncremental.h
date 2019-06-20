/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUICrossIncremental
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUICrossIncremental_H__
#define __albaGUICrossIncremental_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIPanel.h"
#include "albaObserver.h"
#include "albaString.h"

//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class albaGUIButton;
class albaGUIComboBox;

//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------

#define MININT    -2147483647-1
#define MAXINT     2147483647
#define MINFLOAT  -1.0e+38F 
#define MAXFLOAT   1.0e+38F
#define MINDOUBLE -1.0e+299
#define MAXDOUBLE 1.0e+299


//----------------------------------------------------------------------------
// albaGUICrossIncremental :
/**
Class that represent a widget with 4 buttons and a text array disposed like a cross
It can use two accumulator variable that are updated. It needed absolutely step variable pointer.
After modification, it returns to listener an event with in Arg the sign of increment.
*/
//----------------------------------------------------------------------------
class ALBA_EXPORT albaGUICrossIncremental: public albaGUIPanel , public albaObserver
{
public:
  albaGUICrossIncremental(wxWindow* parent, 
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
                  long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN,
                  bool comboStep = false,
                  albaString *buttonUpDown_text = NULL,
                  albaString *buttonLeftRight_text = NULL);

  virtual ~albaGUICrossIncremental();

  
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
    ID_COMBO_ENTRY,
  };

	
	void EnableStep(bool value);

  void SetListener(albaObserver *Listener) {m_Listener = Listener;};
  void OnEvent(albaEventBase *alba_event);

  int IsIncrement(){return m_Increment;};
  double GetTopBottomVariation(){return m_TopBottomVariation;};
  double GetLeftRightVariation(){return m_LeftRightVariation;};


  void SetStepVariable(double step);
  void SetComboBoxItems(wxArrayString &array, int selected = 0);

  void LayoutStyle(const char *label);

  void SetTextButtonTop(const char *text);
  void SetTextButtonBottom(const char *text);
  void SetTextButtonLeft(const char *text);
  void SetTextButtonRight(const char *text);

  albaGUIComboBox *GetComboBox(){return m_StepComboBox;}

  void SetComboValue(int index);

private:
  void CreateWidgetTopBottom(albaString *button_text);
  void CreateWidgetLeftRight(albaString *button_text);
  void CreateWidgetTextEntry(double min, double max);
  void CreateWidgetComboBox();
  void ConvertStepComboIntoStepVariable();

  albaGUIButton    *m_ButtonTop;
  albaGUIButton    *m_ButtonBottom;
  albaGUIButton    *m_ButtonLeft;
  albaGUIButton    *m_ButtonRight;
  wxTextCtrl   *m_StepText;
  albaGUIComboBox *m_StepComboBox;

	wxBoxSizer      *m_Sizer;

  double            *m_TopBottomVariable; ///< Flag used to distinguish top-bottom variable
  double            *m_LeftRightVariable; ///< Flag used to distinguish left-right variable

  double            m_TopBottomVariation;
  double            m_LeftRightVariation;

  bool m_Bold;
  int m_Increment;

  double            *m_StepVariable;
  albaObserver*      m_Listener;
	int               m_IdLayout;

  bool m_IsComboStep;
  int m_Digits;

  DECLARE_EVENT_TABLE()
};
#endif
