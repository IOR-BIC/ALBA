/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIMutexPanel.h,v $
  Language:  C++
  Date:      $Date: 2009-06-17 13:24:52 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIMutexPanel_H__
#define __mafGUIMutexPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/scrolwin.h>
#include "mafGUINamedPanel.h"
#include "mafGUIScrolledPanel.h"
#include "mafGUI.h"

/**
  Class Name: mafGUIMutexPanel.
  Represents a Panel that can plug on it only one panel at time. When put the current gui, it remove the previous one.
*/
class mafGUIMutexPanel: public mafGUIPanel
{
public:
  /** constructor. */
  mafGUIMutexPanel(wxWindow* parent,wxWindowID id );
  /** destructor. */
  virtual ~mafGUIMutexPanel();
  
	/** Put the gui onto the panel removing the previous GUI. */
  bool Put(wxWindow *win);

	/** Remove the current gui. */
  bool RemoveCurrentClient();

  /** retrieve the current gui on panel. */
  wxWindow *GetCurrentClient();

protected:
  //wxWindow *m_Client;
  wxBoxSizer *m_Sizer;
  //Event Table Declaration
  DECLARE_EVENT_TABLE()
};
#endif
