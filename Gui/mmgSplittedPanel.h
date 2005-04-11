/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgSplittedPanel.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:28 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgSplittedPanel_H__
#define __mmgSplittedPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include "mmgPanel.h"

//----------------------------------------------------------------------------
// mmgSplittedPanel :
/**
mmgSplittedPanel is a wxPanel with a user-draggable splitter
that divide it in two sections vertically. Call SetTop and SetBottom
to place things on it.
*/
//----------------------------------------------------------------------------
class mmgSplittedPanel: public mmgPanel
{
public:
  mmgSplittedPanel(wxWindow* parent,wxWindowID id = -1,int size = 100);
  virtual ~mmgSplittedPanel();

  /** Put on top the window. */
	void PutOnTop(wxWindow *w);

  /** Put on bottom the window. */
  void PutOnBottom(wxWindow *w);

protected:
  wxSashLayoutWindow   *m_Bottom;
  wxWindow             *m_topw;
  wxWindow             *m_bottomw;

  /** Call Dolayout. */
  void OnSize(wxSizeEvent& event);

  /** Readraw the Sash panel. */
  void OnSashDrag(wxSashEvent& event);

  /** Readistribute the interface according to the new size. */
  void DoLayout();
	
DECLARE_EVENT_TABLE()
};
#endif
