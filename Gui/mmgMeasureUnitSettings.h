/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgMeasureUnitSettings.h,v $
Language:  C++
Date:      $Date: 2007-09-11 10:19:17 $
Version:   $Revision: 1.7 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgMeasureUnitSettings_H__
#define __mmgMeasureUnitSettings_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mmgMeasureUnitSettings :
//----------------------------------------------------------------------------
/**
*/
class mmgMeasureUnitSettings : public mafGUISettings
{
public:
	mmgMeasureUnitSettings(mafObserver *Listener);
	~mmgMeasureUnitSettings(); 

  enum MEASURE_UNIT_WIDGET_ID
  {
    MEASURE_DATA_STRING_ID = MINID,
    MEASURE_VISUAL_STRING_ID,
    MEASURE_SCALE_FACTOR_ID,
    MEASURE_DEFAULT_DATA_UNIT_ID,
    MEASURE_DEFAULT_VISUAL_UNIT_ID,
    MEASURE_UNIT_UPDATED
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Return the Scale factor to map mm into new unit.*/
  double GetScaleFactor();

  /** Return measure unit name.*/
  mafString GetUnitName();

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize measure unit used into the application.*/
  void InitializeSettings();

  double       m_ScaleFactor;
  mafString    m_DataUnitName;
  mafString    m_VisualUnitName;
  wxString     m_DefaultUnits[5];
  double       m_DefaultFactors[5];
  int          m_ChoosedDataUnit;
  int          m_ChoosedVisualUnit;
};
#endif
