/*=========================================================================

 Program: MAF2
 Module: mafGUISettingsAdvanced
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUISettingsAdvanced_H__
#define __mafGUISettingsAdvanced_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGUISettingsAdvanced :
//----------------------------------------------------------------------------
/**
  Class Name: mafGUISettingsAdvanced.
  Represents advanced settings which can be modified accessing to option menu.
  - conversion units 
*/
class MAF_EXPORT mafGUISettingsAdvanced : public mafGUISettings
{
public:
  /** constructor.  */
  mafGUISettingsAdvanced(mafObserver *Listener, const mafString &label = _("Advanced"));
  /** destructor.  */
  ~mafGUISettingsAdvanced(); 

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
  void OnEvent(mafEventBase *maf_event);

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
