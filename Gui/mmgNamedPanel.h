/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgNamedPanel.h,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:02 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgNamedPanel_H__
#define __mmgNamedPanel_H__

#include "mafDefines.h" //important: mafDefines should always be included as first

#include "mmgLab.h"
#include "mmgPanel.h"
//----------------------------------------------------------------------------
// mmgNamedPanel :
/**
- mmgNamedPanel is a wxPanel with a title bar.
- the title bar has a title and an optional close button.
- mmgNamedPanel provide a default BoxSizer making easy to place widgets on it,
  just call Add(widget) or Remove(widget).

\todo
decide the ID to associate with the close button
*/
//----------------------------------------------------------------------------
class mmgNamedPanel: public mmgPanel
{
public:
  /** set CloseButton to show the close button */
  mmgNamedPanel (wxWindow* parent,wxWindowID id = -1,bool CloseButton = false,bool HideTitle = false); 
  virtual ~mmgNamedPanel();
  
	/** Add a widget to the panel. */
  void Add(wxWindow* window,int option = 0, int flag = wxEXPAND, int border = 0)  {window->Reparent(this); m_sizer->Add(window,option,flag,border);};
  
	/** Add a sizer to the panel. */
  void Add(wxSizer*  sizer, int option = 0, int flag = wxEXPAND, int border = 0)  {m_sizer->Add(sizer, option,flag,border);};
  
	/** Remove a widget from the panel. */
  bool Remove(wxWindow* window) {return m_sizer->Remove(window);};
  
	/** Remove a sizer from the panel. */
  bool Remove(wxSizer*  sizer ) {return m_sizer->Remove(sizer);};

  /** Set the text shown on the title-bar. */
  void SetTitle(wxString label) {if(m_lab) m_lab->SetLabel(label);};

protected:
  wxPanel      *m_top;
  mmgLab         *m_lab;
  wxBoxSizer   *m_sizer;
  wxBoxSizer   *m_topsizer;
  mmgNamedPanel  *m_next;

DECLARE_EVENT_TABLE()
};
#endif
