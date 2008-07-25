/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGUIPanel.h"
//----------------------------------------------------------------------------
// mafGUIPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIPanel,wxPanel)
END_EVENT_TABLE()
//----------------------------------------------------------------------------

mafGUIPanel::mafGUIPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
:wxPanel(parent,id,pos,size,style,name)         
//----------------------------------------------------------------------------
{
  m_NextPanel = NULL;
}
