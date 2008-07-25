/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIPanelStack.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIPanelStack_H__
#define __mafGUIPanelStack_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include "mafGUIPanel.h"
//----------------------------------------------------------------------------
// mafGUIPanelStack :
//----------------------------------------------------------------------------
class mafGUIPanelStack: public mafGUIPanel
{
public:
  mafGUIPanelStack(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");
  virtual ~mafGUIPanelStack(); 

  /** Put the panel on the panel stack removing the previous panel. */
	bool Put(mafGUIPanel* p);
  
	/** Remove the panel from the stack. */
	bool Remove(mafGUIPanel* p);
  
	/** Insert the panel into the stack. */
	void Push(mafGUIPanel *p);
  
	/** Retrieve the last pushed panel. */
	void Pop ();

protected:
  /** Call DoLayout. */
	void OnSize(wxSizeEvent& event);
  
	/** Adjust the size calling wxPanel::OnSize. */
	void DoLayout();

  wxBoxSizer   *m_Sizer;
  mafGUIPanel     *m_CurrentPanel;

DECLARE_EVENT_TABLE()
};
#endif
