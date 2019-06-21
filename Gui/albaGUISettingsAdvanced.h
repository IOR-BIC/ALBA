/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISettingsAdvanced
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUISettingsAdvanced_H__
#define __albaGUISettingsAdvanced_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// albaGUISettingsAdvanced :
//----------------------------------------------------------------------------
/**
  Class Name: albaGUISettingsAdvanced.
  Represents advanced settings which can be modified accessing to option menu.
  - conversion units 
*/
class ALBA_EXPORT albaGUISettingsAdvanced : public albaGUISettings
{
public:
  /** constructor.  */
  albaGUISettingsAdvanced(albaObserver *Listener, const albaString &label = _("Advanced"));
  /** destructor.  */
  ~albaGUISettingsAdvanced(); 

  /** GUI IDs*/
  enum MEASURE_UNIT_WIDGET_ID
  {
    ID_CONVERSION_UNITS = MINID,
  };

  /** Conversion IDs*/
  enum CONVERSION_UNITS
  {
    NONE = 0,
    mm2m,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(albaEventBase *alba_event);

  /** Retrieve conversion unit parameter */
  int GetConversionType(){return m_ConversionUnits;};

  /** Set conversion unit parameter */
  void SetConversionType(int conversion);

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize measure unit used into the application.*/
  void InitializeSettings();

  int m_ConversionUnits;

};
#endif
