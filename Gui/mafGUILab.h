/*=========================================================================

 Program: MAF2
 Module: mafGUILab
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUILab_H__
#define __mafGUILab_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGUILab :
/**
mafGUILab is a Label that notify user-click using the normal wxEvents.
It is used on the titlebar of mafGUINamedPanel to popup a menu.
*/
//----------------------------------------------------------------------------
class mafGUILab : public wxStaticText
{
DECLARE_DYNAMIC_CLASS(mafGUILab)

public:
  mafGUILab() { };

  mafGUILab(wxWindow *parent, wxWindowID id,
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
#endif // __mafGUILab_H__
