/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUISettingsAdvanced.cpp,v $
Language:  C++
Date:      $Date: 2009-02-25 16:43:09 $
Version:   $Revision: 1.1.2.1 $
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

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medGUISettingsAdvanced.h"
#include "medDecl.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
medGUISettingsAdvanced::medGUISettingsAdvanced(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  m_ConversionUnits = NONE;

  InitializeSettings();
}
//----------------------------------------------------------------------------
void medGUISettingsAdvanced::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  wxString choices_conversion[2] = {"NONE","mm2m"};
  m_Gui->Label(_("Convert Data in SI units"));
  m_Gui->Combo(ID_CONVERSION_UNITS,"",&m_ConversionUnits,2,choices_conversion);
  
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
medGUISettingsAdvanced::~medGUISettingsAdvanced() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medGUISettingsAdvanced::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
  case ID_CONVERSION_UNITS:
    {
      m_Config->Write("ConversionUnits",m_ConversionUnits);
    }
    break;
  default:
    mafEventMacro(*maf_event);
    break; 
  }

  m_Config->Flush();
}
//----------------------------------------------------------------------------
void medGUISettingsAdvanced::InitializeSettings()
//----------------------------------------------------------------------------
{
  long long_item;

  if(m_Config->Read("ConversionUnits", &long_item))
  {
    m_ConversionUnits=long_item;
  }
  else
  {
    m_Config->Write("ConversionUnits",m_ConversionUnits);
  }

  m_Config->Flush();
}
