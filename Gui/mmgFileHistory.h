/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFileHistory.h,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:00 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgFileHistory_H__
#define __mmgFileHistory_H__

#include "mafDefines.h" //important: mafDefines should always be included as first
#include <wx/docview.h>
#include <wx/confbase.h>

#include "mafEvent.h"
//----------------------------------------------------------------------------
// mmgFileHistory :
//----------------------------------------------------------------------------
/** */
class mmgFileHistory : public wxFileHistory
{
public:
 	mmgFileHistory();

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
