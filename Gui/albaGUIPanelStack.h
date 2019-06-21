/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIPanelStack
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIPanelStack_H__
#define __albaGUIPanelStack_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include "albaGUIPanel.h"
/**
  class name:  albaGUIPanelStack
   Panel with the same behaviour of a stack container (push and pop methods)
*/
class albaGUIPanelStack: public albaGUIPanel
{
public:
  /** constructor */
  albaGUIPanelStack(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");
  /** destructor */
  virtual ~albaGUIPanelStack(); 

  /** Put the panel on the panel stack removing the previous panel. */
	bool Put(albaGUIPanel* p);
  
	/** Remove the panel from the stack. */
	bool Remove(albaGUIPanel* p);
  
	/** Insert the panel into the stack. */
	void Push(albaGUIPanel *p);
  
	/** Retrieve the last pushed panel. */
	void Pop ();

protected:
  /** Call DoLayout. */
	void OnSize(wxSizeEvent& event);
  
	/** Adjust the size calling wxPanel::OnSize. */
	void DoLayout();

  wxBoxSizer   *m_Sizer;
  albaGUIPanel     *m_CurrentPanel;
/** Event table declaration macro  */
DECLARE_EVENT_TABLE()
};
#endif
