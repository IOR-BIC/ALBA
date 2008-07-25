/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIHolder.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIGuiHolder_H__
#define __mafGUIGuiHolder_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/scrolwin.h>
#include "mafGUINamedPanel.h"
#include "mafGUIScrolledPanel.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
// mafGUIHolder :
//----------------------------------------------------------------------------
class mafGUIHolder: public mafGUINamedPanel
{
public:
           mafGUIHolder(wxWindow* parent,wxWindowID id, bool CloseButton=false, bool HideTitle = false );
  virtual ~mafGUIHolder();
  
	/** Return the panel that held the gui. */
	wxPanel* GetPanel() {return (wxPanel*)m_Panel;};

	/** Put the gui onto the panel removing the previous gui. */
  bool Put(mafGUI *gui);

	/** Remove the current gui. */
  bool RemoveCurrentGui();

  wxWindow *GetCurrentGui();

protected:
  mafGUIScrolledPanel *m_Panel;
  
DECLARE_EVENT_TABLE()
};
#endif
