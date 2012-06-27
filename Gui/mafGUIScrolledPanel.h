/*=========================================================================

 Program: MAF2
 Module: mafGUIScrolledPanel
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafGUIScrolledPanel: public wxScrolledWindow
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
