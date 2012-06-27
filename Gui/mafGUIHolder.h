/*=========================================================================

 Program: MAF2
 Module: mafGUIHolder
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIHolder_H__
#define __mafGUIHolder_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/scrolwin.h>
#include "mafGUINamedPanel.h"
#include "mafGUIScrolledPanel.h"
#include "mafGUI.h"


/**
  class name: mafGUIHolder
  Secialized panel in which a developer can put a mafGUI.
*/
class MAF_EXPORT mafGUIHolder: public mafGUINamedPanel
{
public:
  /** constructor */
           mafGUIHolder(wxWindow* parent,wxWindowID id, bool CloseButton=false, bool HideTitle = false );
  /** destructor */
  virtual ~mafGUIHolder();
  
	/** Return the panel that held the gui. */
	wxPanel* GetPanel() {return (wxPanel*)m_Panel;};

	/** Put the gui onto the panel removing the previous gui. */
  bool Put(mafGUI *gui);

	/** Remove the current gui. */
  bool RemoveCurrentGui();

  /** retrieve the current gui */
  wxWindow *GetCurrentGui();

protected:
  mafGUIScrolledPanel *m_Panel;

/** Event table declaration macro  */
DECLARE_EVENT_TABLE()
};
#endif
