/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIMutexPanel.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
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

//----------------------------------------------------------------------------
// mafGUIMutexPanel :
//----------------------------------------------------------------------------
class mafGUIMutexPanel: public mafGUIPanel
{
public:
           mafGUIMutexPanel(wxWindow* parent,wxWindowID id );
  virtual ~mafGUIMutexPanel();
  
	/** Put the gui onto the panel removing the previous GUI. */
  bool Put(wxWindow *win);

	/** Remove the current gui. */
  bool RemoveCurrentClient();

  wxWindow *GetCurrentClient();

protected:
  //wxWindow *m_Client;
  wxBoxSizer *m_Sizer;
DECLARE_EVENT_TABLE()
};
#endif
