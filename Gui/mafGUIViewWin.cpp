/*=========================================================================

 Program: MAF2
 Module: mafGUIViewWin
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUIViewWin.h"
#include "mafView.h"

//----------------------------------------------------------------------------
// mafGUIViewWin EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIViewWin,wxPanel)
  EVT_SIZE(mafGUIViewWin::OnSize)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
mafGUIViewWin::mafGUIViewWin(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size,long style)
:wxPanel(parent,id,pos,size,wxTAB_TRAVERSAL)//SIL. 10-11-2003: added wxTAB_TRAVERSAL to intercept 'Enter' in the text widgets
//----------------------------------------------------------------------------
{
  m_Owner = NULL;
}
//----------------------------------------------------------------------------
void mafGUIViewWin::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  if(m_Owner) 
    m_Owner->OnSize(event);
}
