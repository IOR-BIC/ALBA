/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIHolder.h,v $
  Language:  C++
  Date:      $Date: 2009-12-01 14:36:33 $
  Version:   $Revision: 1.2.2.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
