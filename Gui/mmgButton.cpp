/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgButton.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:20 $
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


#include "mmgButton.h"

 //SIL. 23-3-2005:  -- hack to be removed
#define BN_CLICKED 0
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgButton,wxButton)
     EVT_SET_FOCUS(mmgButton::OnSetFocus) 
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgButton::mmgButton(wxWindow* parent, wxWindowID id, const wxString& label, 
                     const wxPoint& pos, const wxSize& size, long style)
: wxButton(parent,id,label,pos,size,style)
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_id = id;
};
//----------------------------------------------------------------------------
void mmgButton::Command(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  wxLogMessage("mmgButton::cmd");
}
#ifdef __WIN32__
    //----------------------------------------------------------------------------
    bool mmgButton::MSWCommand(WXUINT param, WXWORD id)
    //----------------------------------------------------------------------------
    {
      bool processed = FALSE;
      switch ( param )
      {
        case 1:                     // message came from an accelerator
        case BN_CLICKED:            // normal buttons send this
        //SIL. 23-3-2005: case BN_DOUBLECLICKED:      // owner-drawn ones also send this 
          if(m_Listener) 
            m_Listener->OnEvent(mafEvent(this, m_id));
          else
            processed = SendClickEvent();
        break;
      }
      return processed;
    }
#endif
