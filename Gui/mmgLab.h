/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLab.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:23 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgLab_H__
#define __mmgLab_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mmgLab :
/**
mmgLab is a Label that notify user-click using the normal wxEvents.
It is used on the titlebar of mmgNamedPanel to popup a menu.
*/
//----------------------------------------------------------------------------
class mmgLab : public wxStaticText
{
DECLARE_DYNAMIC_CLASS(mmgLab)

public:
  mmgLab() { };

  mmgLab(wxWindow *parent, wxWindowID id,
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
#endif // __mmgLab_H__
