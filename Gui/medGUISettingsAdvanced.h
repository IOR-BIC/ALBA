/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUISettingsAdvanced.h,v $
Language:  C++
Date:      $Date: 2011-05-26 07:43:15 $
Version:   $Revision: 1.1.2.4 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#ifndef __medGUISettingsAdvanced_H__
#define __medGUISettingsAdvanced_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// medGUISettingsAdvanced :
//----------------------------------------------------------------------------
/**
  Class Name: medGUISettingsAdvanced.
  Represents advanced settings which can be modified accessing to option menu.
  - conversion units 
*/
class MED_EXPORT medGUISettingsAdvanced : public mafGUISettings
{
public:
  /** constructor.  */
  medGUISettingsAdvanced(mafObserver *Listener, const mafString &label = _("Advanced"));
  /** destructor.  */
  ~medGUISettingsAdvanced(); 

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
