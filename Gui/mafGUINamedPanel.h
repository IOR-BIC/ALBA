/*=========================================================================

 Program: MAF2
 Module: mafGUINamedPanel
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUINamedPanel_H__
#define __mafGUINamedPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUILab.h"
#include "mafGUIPanel.h"
#include "mafDefines.h"

/**
 Class Name: mafGUINamedPanel.
- mafGUINamedPanel is a wxPanel with a title bar.
- the title bar has a title and an optional close button.
- mafGUINamedPanel provide a default BoxSizer making easy to place widgets on it,
  just call Add(widget) or Remove(widget).

\todo
decide the ID to associate with the close button
*/
class MAF_EXPORT mafGUINamedPanel: public mafGUIPanel
{
public:
  /** constructor.  set CloseButton to show the close button */
  mafGUINamedPanel (wxWindow* parent,wxWindowID id = -1,bool CloseButton = false,bool HideTitle = false); 
  /** destructor. */
  virtual ~mafGUINamedPanel();
  
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

protected:
  wxPanel         *m_Top;
  mafGUILab          *m_Label;
  wxBoxSizer      *m_Sizer;
  wxBoxSizer      *m_TopSizer;
  mafGUINamedPanel   *m_NextPanel;
  wxColour         m_Color; 
  
  /** Event Table Declaration*/
  DECLARE_EVENT_TABLE()
};
#endif
