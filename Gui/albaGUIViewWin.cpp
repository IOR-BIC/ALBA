/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIViewWin
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

#include "albaGUIViewWin.h"
#include "albaView.h"

//----------------------------------------------------------------------------
// albaGUIViewWin EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIViewWin,wxPanel)
  EVT_SIZE(albaGUIViewWin::OnSize)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
albaGUIViewWin::albaGUIViewWin(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size,long style)
:wxPanel(parent,id,pos,size,wxTAB_TRAVERSAL)//SIL. 10-11-2003: added wxTAB_TRAVERSAL to intercept 'Enter' in the text widgets
//----------------------------------------------------------------------------
{
  m_Owner = NULL;
}
//----------------------------------------------------------------------------
void albaGUIViewWin::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  if(m_Owner) 
    m_Owner->OnSize(event);
}
