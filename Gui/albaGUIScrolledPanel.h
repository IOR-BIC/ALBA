/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIScrolledPanel
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIScrolledPanel_H__
#define __albaGUIScrolledPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// albaGUIScrolledPanel :
/** 
albaGUIScrolledPanel is a Panel on which is possible to add child-widget.
A vertical scrollbar will automatically appear if 
child-widgets require more space then the avaiable.
*/
//----------------------------------------------------------------------------
class ALBA_EXPORT albaGUIScrolledPanel: public wxScrolledWindow
{
public:
  albaGUIScrolledPanel(wxWindow* parent,wxWindowID id = -1);
  virtual ~albaGUIScrolledPanel();
    
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
