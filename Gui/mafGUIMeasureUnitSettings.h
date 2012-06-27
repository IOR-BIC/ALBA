/*=========================================================================

 Program: MAF2
 Module: mafGUIMeasureUnitSettings
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIMeasureUnitSettings_H__
#define __mafGUIMeasureUnitSettings_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: mafGUIMeasureUnitSettings.
  Represent a gui setting (reachable from option menu) in which data unit parameters can be customized.
*/
class mafGUIMeasureUnitSettings : public mafGUISettings
{
public:
  /** constructor. */
	mafGUIMeasureUnitSettings(mafObserver *Listener, const mafString &label = _("Measure Unit"));
  /** destructor. */
	~mafGUIMeasureUnitSettings(); 

  /** Measure Ids */
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
