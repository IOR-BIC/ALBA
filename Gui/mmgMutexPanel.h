/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMutexPanel.h,v $
  Language:  C++
  Date:      $Date: 2007-09-05 08:26:02 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgMutexPanel_H__
#define __mmgMutexPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/scrolwin.h>
#include "mmgNamedPanel.h"
#include "mmgScrolledPanel.h"
#include "mmgGui.h"

//----------------------------------------------------------------------------
// mmgMutexPanel :
//----------------------------------------------------------------------------
class mmgMutexPanel: public mmgPanel
{
public:
           mmgMutexPanel(wxWindow* parent,wxWindowID id );
  virtual ~mmgMutexPanel();
  
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
