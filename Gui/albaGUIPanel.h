/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIPanel
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIPanel_H__
#define __albaGUIPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

/** 
  class name: albaGUIPanel
   Inherits directly from wxPanel and adds the pointer to the next panel
*/
class ALBA_EXPORT albaGUIPanel: public wxPanel
{
public:
  /** constructor */
  albaGUIPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");

  albaGUIPanel *m_NextPanel;

protected:
/** Event table declaration macro  */
DECLARE_EVENT_TABLE()
};
#endif
