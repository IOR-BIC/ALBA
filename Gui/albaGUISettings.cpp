/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISettings
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGUISettings.h"

#include "albaDecl.h"
#include "albaGUI.h"

//#include "albaMemDbg.h"

//----------------------------------------------------------------------------
albaGUISettings::albaGUISettings(albaObserver *Listener, const albaString &label)
//----------------------------------------------------------------------------
{
  m_Listener = Listener;
  m_Label    = label;

  m_Gui = NULL;

  m_Config = new wxConfig(wxEmptyString);
}
//----------------------------------------------------------------------------
albaGUISettings::~albaGUISettings()
//----------------------------------------------------------------------------
{
  cppDEL(m_Config);
}
//----------------------------------------------------------------------------
void albaGUISettings::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
albaGUI* albaGUISettings::GetGui()
//----------------------------------------------------------------------------
{
  if (m_Gui == NULL)
  {
    CreateGui();
  }
  assert(m_Gui);
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaGUISettings::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  albaEventMacro(*alba_event);
  m_Config->Flush();
}
//----------------------------------------------------------------------------
void albaGUISettings::InitializeSettings()
//----------------------------------------------------------------------------
{
/*
  wxString string_item;
  long long_item;
  if(m_Config->Read("KeyString", &string_item))
  {
    //Update your variable according to the value read from the registry
  }
  else
  {
    // Write default value to the registry; the key doesn't exists
  }
  m_Config->Flush();
*/
}
