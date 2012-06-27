/*=========================================================================

 Program: MAF2
 Module: mafGUIPanel
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
