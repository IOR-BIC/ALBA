/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIScrolledPanel.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIScrolledPanel_H__
#define __mafGUIScrolledPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGUIScrolledPanel :
/** 
mafGUIScrolledPanel is a Panel on which is possible to add child-widget.
A vertical scrollbar will automatically appear if 
child-widgets require more space then the avaiable.
*/
//----------------------------------------------------------------------------
class mafGUIScrolledPanel: public wxScrolledWindow
{
public:
  mafGUIScrolledPanel(wxWindow* parent,wxWindowID id = -1);
  virtual ~mafGUIScrolledPanel();
    
	/** Add a window into the scrolled panel. */
  void Add(wxWindow* window,int option = 0, int flag = wxEXPAND, int border = 0);  

	/** Add a sizer into the scrolled panel. */
  void Add(wxSizer*  sizer, int option = 0, int flag = wxEXPAND, int border = 0); 

	/** Remove a window from the scrolled panel. */
  bool Remove(wxWindow* window);

	/** Remove a sizer from the scrolled panel. */
  bool Remove(wxSizer*  sizer );

  /** Adjust the ScrollBar Settings.
      Must be called explicitly if the size of the contents change at runtime.      
  */
  void FitInside();

  /** Redraw the scrolled window. */
  virtual bool Layout();


protected:
  wxBoxSizer *m_Sizer;

DECLARE_EVENT_TABLE()
};
#endif
