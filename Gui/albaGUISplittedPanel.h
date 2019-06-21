/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISplittedPanel
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUISplittedPanel_H__
#define __albaGUISplittedPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include "albaDefines.h"
#include "albaGUIPanel.h"

//----------------------------------------------------------------------------
// albaGUISplittedPanel :
/**
albaGUISplittedPanel is a wxPanel with a user-draggable splitter
that divide it in two sections vertically. Call SetTop and SetBottom
to place things on it.
*/
//----------------------------------------------------------------------------
class ALBA_EXPORT albaGUISplittedPanel: public albaGUIPanel
{
public:
  albaGUISplittedPanel(wxWindow* parent,wxWindowID id = -1,int size = 100);
  virtual ~albaGUISplittedPanel();

  /** Put on top the window. */
	void PutOnTop(wxWindow *w);

  /** Put on bottom the window. */
  void PutOnBottom(wxWindow *w);

protected:
  wxSashLayoutWindow   *m_Bottom;
  wxWindow             *m_TopPanel;
  wxWindow             *m_BottomPanel;

  /** Call Dolayout. */
  void OnSize(wxSizeEvent& event);

  /** Readraw the Sash panel. */
  void OnSashDrag(wxSashEvent& event);

  /** Readistribute the interface according to the new size. */
  void DoLayout();
	
DECLARE_EVENT_TABLE()
};
#endif
