/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIFileHistory.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 10:08:10 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
