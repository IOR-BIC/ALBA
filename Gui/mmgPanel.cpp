/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:02 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

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
  m_next = NULL;
}
