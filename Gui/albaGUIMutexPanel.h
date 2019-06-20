/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMutexPanel
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIMutexPanel_H__
#define __albaGUIMutexPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/scrolwin.h>
#include "albaGUINamedPanel.h"
#include "albaGUIScrolledPanel.h"
#include "albaGUI.h"

/**
  Class Name: albaGUIMutexPanel.
  Represents a Panel that can plug on it only one panel at time. When put the current gui, it remove the previous one.
*/
class albaGUIMutexPanel: public albaGUIPanel
{
public:
  /** constructor. */
  albaGUIMutexPanel(wxWindow* parent,wxWindowID id );
  /** destructor. */
  virtual ~albaGUIMutexPanel();
  
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
