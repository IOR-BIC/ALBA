/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgViewWin.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:01:24 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

//----------------------------------------------------------------------------
// Include: - include the class being defined first
//----------------------------------------------------------------------------
#include "mmgViewWin.h"

#include "mafView.h"
//----------------------------------------------------------------------------
// mmgViewWin EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgViewWin,wxPanel)
  EVT_SIZE(mmgViewWin::OnSize)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
mmgViewWin::mmgViewWin(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size,long style)
:wxPanel(parent,id,pos,size,wxTAB_TRAVERSAL)//SIL. 10-11-2003: added wxTAB_TRAVERSAL to intercept 'Enter' in the text widgets
//----------------------------------------------------------------------------
{
  m_owner = NULL;
}
//----------------------------------------------------------------------------
void mmgViewWin::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  if(m_owner) m_owner->OnSize(event);
}
