/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgViewWin.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-04 14:48:13 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
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
  m_Owner = NULL;
}
//----------------------------------------------------------------------------
void mmgViewWin::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  if(m_Owner) 
    m_Owner->OnSize(event);
}
