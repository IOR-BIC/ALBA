/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLab.h,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:01 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mmgLab_H_
#define _mmgLab_H_

#include "mafDefines.h" //important: mafDefines should always be included as first

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
#endif // _mmgLab_H_
