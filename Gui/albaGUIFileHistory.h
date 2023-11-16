/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIFileHistory
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIFileHistory_H__
#define __albaGUIFileHistory_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include <wx/docview.h>
#include <wx/confbase.h>

//----------------------------------------------------------------------------
// albaGUIFileHistory :
//----------------------------------------------------------------------------
/**
  class name: albaGUIFileHistory
  class that handle a list of file called history.  Tipically it is used by an application
  as an history for last opened files.
*/
class ALBA_EXPORT albaGUIFileHistory : public wxFileHistory
{
public:
  /** constructor */
 	albaGUIFileHistory();

  /** Add filename to history. */
	virtual void AddFileToHistory(const wxString& file);

  /** Remove filename to history. */
  virtual void RemoveFileFromHistory(int i);

	void UpdateMenuLabels(wxMenu* menu);

  /** Add filename item to men. */
  virtual void AddFilesToMenu();

  /** Add filename item to a given men. */
  virtual void AddFilesToMenu(wxMenu* menu);
};
#endif
