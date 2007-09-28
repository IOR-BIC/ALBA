/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUISettings.cpp,v $
Language:  C++
Date:      $Date: 2007-09-28 15:04:12 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUISettings.h"

#include "mafDecl.h"
#include "mmgGui.h"

//----------------------------------------------------------------------------
mafGUISettings::mafGUISettings(mafObserver *Listener, const mafString &label)
//----------------------------------------------------------------------------
{
	m_Listener = Listener;
  m_Label    = label;

  m_Gui = NULL;

  m_Config = new wxConfig(wxEmptyString);
}
//----------------------------------------------------------------------------
mafGUISettings::~mafGUISettings()
//----------------------------------------------------------------------------
{
  cppDEL(m_Config);
}
//----------------------------------------------------------------------------
void mafGUISettings::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
mmgGui* mafGUISettings::GetGui()
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
void mafGUISettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*maf_event);
  m_Config->Flush();
}
//----------------------------------------------------------------------------
void mafGUISettings::InitializeSettings()
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
