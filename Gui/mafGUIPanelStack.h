/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIPanelStack.h,v $
  Language:  C++
  Date:      $Date: 2009-12-01 14:36:33 $
  Version:   $Revision: 1.1.2.1 $
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
/**
  class name:  mafGUIPanelStack
   Panel with the same behaviour of a stack container (push and pop methods)
*/
class mafGUIPanelStack: public mafGUIPanel
{
public:
  /** constructor */
  mafGUIPanelStack(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");
  /** destructor */
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
/** Event table declaration macro  */
DECLARE_EVENT_TABLE()
};
#endif
