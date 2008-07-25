/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUINamedPanel.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUINamedPanel_H__
#define __mafGUINamedPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUILab.h"
#include "mafGUIPanel.h"
//----------------------------------------------------------------------------
// mafGUINamedPanel :
/**
- mafGUINamedPanel is a wxPanel with a title bar.
- the title bar has a title and an optional close button.
- mafGUINamedPanel provide a default BoxSizer making easy to place widgets on it,
  just call Add(widget) or Remove(widget).

\todo
decide the ID to associate with the close button
*/
//----------------------------------------------------------------------------
class mafGUINamedPanel: public mafGUIPanel
{
public:
  /** set CloseButton to show the close button */
  mafGUINamedPanel (wxWindow* parent,wxWindowID id = -1,bool CloseButton = false,bool HideTitle = false); 
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

DECLARE_EVENT_TABLE()
};
#endif
