/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUICrossIncremental.h,v $
  Language:  C++
  Date:      $Date: 2009-01-19 12:00:05 $
  Version:   $Revision: 1.4.2.3 $
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
#include "mafGUIPanel.h"
#include "mafObserver.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class mafGUIButton;
class mafGUIComboBox;

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
class mafGUICrossIncremental: public mafGUIPanel , public mafObserver
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
                  long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN,
                  bool comboStep = false);

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
    ID_COMBO_ENTRY,
  };

	
	void EnableStep(bool value);

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent(mafEventBase *maf_event);

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

  mafGUIComboBox *GetComboBox(){return m_StepComboBox;}

  void SetComboValue(int index);

private:
  void CreateWidgetTopBottom();
  void CreateWidgetLeftRight();
  void CreateWidgetTextEntry(double min, double max);
  void CreateWidgetComboBox();
  void ConvertStepComboIntoStepVariable();

  mafGUIButton    *m_ButtonTop;
  mafGUIButton    *m_ButtonBottom;
  mafGUIButton    *m_ButtonLeft;
  mafGUIButton    *m_ButtonRight;
  wxTextCtrl   *m_StepText;
  mafGUIComboBox *m_StepComboBox;

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

  bool m_IsComboStep;
  int m_Digits;

  DECLARE_EVENT_TABLE()
};
#endif
