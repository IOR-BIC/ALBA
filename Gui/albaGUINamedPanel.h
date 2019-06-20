/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUINamedPanel
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUINamedPanel_H__
#define __albaGUINamedPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUILab.h"
#include "albaGUIPanel.h"
#include "albaDefines.h"

/**
 Class Name: albaGUINamedPanel.
- albaGUINamedPanel is a wxPanel with a title bar.
- the title bar has a title and an optional close button.
- albaGUINamedPanel provide a default BoxSizer making easy to place widgets on it,
  just call Add(widget) or Remove(widget).

\todo
decide the ID to associate with the close button
*/
class ALBA_EXPORT albaGUINamedPanel: public albaGUIPanel
{
public:
  /** constructor.  set CloseButton to show the close button */
  albaGUINamedPanel (wxWindow* parent,wxWindowID id = -1,bool CloseButton = false,bool HideTitle = false); 
  /** destructor. */
  virtual ~albaGUINamedPanel();
  
  /** Add a widget to the panel. */
  void Add(wxWindow* window,int option = 0, int flag = wxEXPAND, int border = 0)  {window->Reparent(this); m_Sizer->Add(window,option,flag,border);};
  
	/** Add a sizer to the panel. */
  void Add(wxSizer*  sizer, int option = 0, int flag = wxEXPAND, int border = 0)  {m_Sizer->Add(sizer, option,flag,border);};
  
	/** Remove a widget from the panel. */
  bool Remove(wxWindow* window) {return m_Sizer->Detach(window);};
  
	/** Remove a sizer from the panel. */
  bool Remove(wxSizer*  sizer ) {return m_Sizer->Detach(sizer);};

  /** Set the text shown on the title-bar. */
  void SetTitle(wxString label) {if(m_Label) m_Label->SetLabel(label);};

  /** Change the background color of the title. */
  void SetTitleColor(wxColour *color = NULL);

	/** Returns TopSizer */
	wxBoxSizer * GetTopSizer() { return m_TopSizer; }

	/** Returns Top Panel */
	wxPanel * GetTopPanel()  { return m_Top; }

protected:
  wxPanel         *m_Top;
  albaGUILab          *m_Label;
  wxBoxSizer      *m_Sizer;
  wxBoxSizer      *m_TopSizer;
  albaGUINamedPanel   *m_NextPanel;
  wxColour         m_Color; 
  
  /** Event Table Declaration*/
  DECLARE_EVENT_TABLE()
};
#endif
