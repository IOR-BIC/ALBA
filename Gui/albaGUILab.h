/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILab
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUILab_H__
#define __albaGUILab_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// albaGUILab :
/**
albaGUILab is a Label that notify user-click using the normal wxEvents.
It is used on the titlebar of albaGUINamedPanel to popup a menu.
*/
//----------------------------------------------------------------------------
class albaGUILab : public wxStaticText
{
DECLARE_DYNAMIC_CLASS(albaGUILab)

public:
  albaGUILab() { };

  albaGUILab(wxWindow *parent, wxWindowID id,
      const wxString& label,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0,
      const wxString& name = wxStaticTextNameStr)
  {
    Create(parent, id, label, pos, size, style, name);
  };

protected:  
  /** Notify the mouse click. */
	void OnLeftMouseButtonDown(wxMouseEvent &event);
DECLARE_EVENT_TABLE()
};
#endif // __albaGUILab_H__
