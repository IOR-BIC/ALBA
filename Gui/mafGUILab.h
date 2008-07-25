/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUILab.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
