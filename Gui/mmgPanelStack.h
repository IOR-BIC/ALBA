/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPanelStack.h,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:02 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgPanelStack_H__
#define __mmgPanelStack_H__

#include "mafDefines.h" //important: mafDefines should always be included as first
#include <wx/laywin.h>

#include "mmgPanel.h"
//----------------------------------------------------------------------------
// mmgPanelStack :
//----------------------------------------------------------------------------
class mmgPanelStack: public mmgPanel
{
public:
  mmgPanelStack(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");
  virtual ~mmgPanelStack(); 

  /** Put the panel on the panel stack removing the previous panel. */
	bool Put(mmgPanel* p);
  
	/** Remove the panel from the stack. */
	bool Remove(mmgPanel* p);
  
	/** Insert the panel into the stack. */
	void Push(mmgPanel *p);
  
	/** Retrieve the last pushed panel. */
	void Pop ();

protected:
  /** Call DoLayout. */
	void OnSize(wxSizeEvent& event);
  
	/** Adjust the size calling wxPanel::OnSize. */
	void DoLayout();

  wxBoxSizer   *m_sizer;
  mmgPanel     *m_curr_panel;

DECLARE_EVENT_TABLE()
};
#endif
