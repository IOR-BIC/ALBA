/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPanel.h,v $
  Language:  C++
  Date:      $Date: 2007-09-05 08:26:02 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgPanel_H__
#define __mmgPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mmgPanel :
//----------------------------------------------------------------------------
/** */
class mmgPanel: public wxPanel
{
public:
  mmgPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");

  mmgPanel *m_NextPanel;

protected:

DECLARE_EVENT_TABLE()
};
#endif
