/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIPanel.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIPanel_H__
#define __mafGUIPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGUIPanel :
//----------------------------------------------------------------------------
/** */
class mafGUIPanel: public wxPanel
{
public:
  mafGUIPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");

  mafGUIPanel *m_NextPanel;

protected:

DECLARE_EVENT_TABLE()
};
#endif
