/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-05 08:26:02 $
  Version:   $Revision: 1.3 $
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


#include "mmgPanel.h"
//----------------------------------------------------------------------------
// mmgPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgPanel,wxPanel)
END_EVENT_TABLE()
//----------------------------------------------------------------------------

mmgPanel::mmgPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
:wxPanel(parent,id,pos,size,style,name)         
//----------------------------------------------------------------------------
{
  m_NextPanel = NULL;
}
