/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIFileHistory.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
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
/** */
class mafGUIFileHistory : public wxFileHistory
{
public:
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
