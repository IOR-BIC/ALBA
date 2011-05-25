/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIPanel.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 10:09:29 $
  Version:   $Revision: 1.1.2.2 $
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

/** 
  class name: mafGUIPanel
   Inherits directly from wxPanel and adds the pointer to the next panel
*/
class MAF_EXPORT mafGUIPanel: public wxPanel
{
public:
  /** constructor */
  mafGUIPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");

  mafGUIPanel *m_NextPanel;

protected:
/** Event table declaration macro  */
DECLARE_EVENT_TABLE()
};
#endif
