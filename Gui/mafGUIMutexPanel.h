/*=========================================================================

 Program: MAF2
 Module: mafGUIMutexPanel
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
