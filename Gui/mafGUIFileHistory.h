/*=========================================================================

 Program: MAF2
 Module: mafGUIFileHistory
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIFileHistory_H__
#define __mafGUIFileHistory_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include <wx/docview.h>
#include <wx/confbase.h>

//----------------------------------------------------------------------------
// mafGUIFileHistory :
//----------------------------------------------------------------------------
/**
  class name: mafGUIFileHistory
  class that handle a list of file called history.  Tipically it is used by an application
  as an history for last opened files.
*/
class MAF_EXPORT mafGUIFileHistory : public wxFileHistory
{
public:
  /** constructor */
 	mafGUIFileHistory();

  /** Add filename to history. */
	virtual void AddFileToHistory(const wxString& file);

  /** Remove filename to history. */
  virtual void RemoveFileFromHistory(int i);

  /** Add filename item to men. */
  virtual void AddFilesToMenu();

  /** Add filename item to a given men. */
  virtual void AddFilesToMenu(wxMenu* menu);
};
#endif
