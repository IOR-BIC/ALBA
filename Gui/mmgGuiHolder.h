/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgGuiHolder.h,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:01 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgGuiHolder_H__
#define __mmgGuiHolder_H__

#include "mafDefines.h" //important: mafDefines should always be included as first

#include <wx/scrolwin.h>
#include "mmgNamedPanel.h"
#include "mmgScrolledPanel.h"
#include "mmgGui.h"

//----------------------------------------------------------------------------
// mmgGuiHolder :
//----------------------------------------------------------------------------
class mmgGuiHolder: public mmgNamedPanel
{
public:
           mmgGuiHolder(wxWindow* parent,wxWindowID id, bool CloseButton=false, bool HideTitle = false );
  virtual ~mmgGuiHolder();
  
	/** Return the panel that held the gui. */
	wxPanel* GetPanel() {return (wxPanel*)m_panel;};

	/** Put the gui onto the panel removing the previous gui. */
  bool Put(mmgGui *gui);

	/** Remove the current gui. */
  bool Remove(mmgGui *gui = NULL);

protected:
  mmgGui           *m_currgui;
  mmgScrolledPanel *m_panel;

DECLARE_EVENT_TABLE()
};
#endif
