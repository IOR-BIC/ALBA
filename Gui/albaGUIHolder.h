/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIHolder
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIHolder_H__
#define __albaGUIHolder_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/scrolwin.h>
#include "albaGUINamedPanel.h"
#include "albaGUIScrolledPanel.h"
#include "albaGUI.h"


/**
  class name: albaGUIHolder
  Secialized panel in which a developer can put a albaGUI.
*/
class ALBA_EXPORT albaGUIHolder: public albaGUINamedPanel
{
public:
  /** constructor */
           albaGUIHolder(wxWindow* parent,wxWindowID id, bool CloseButton=false, bool HideTitle = false );
  /** destructor */
  virtual ~albaGUIHolder();
  
	/** Return the panel that held the gui. */
	wxPanel* GetPanel() {return (wxPanel*)m_Panel;};

	/** Put the gui onto the panel removing the previous gui. */
  bool Put(albaGUI *gui);

	/** Remove the current gui. */
  bool RemoveCurrentGui();

  /** retrieve the current gui */
  wxWindow *GetCurrentGui();

protected:
  albaGUIScrolledPanel *m_Panel;

/** Event table declaration macro  */
DECLARE_EVENT_TABLE()
};
#endif
