/*=========================================================================
Program:   AlbaMaster
Module:    AlbaMaster.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __AlbaApp_H__
#define __AlbaApp_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaLogicWithManagers.h" 

// ============================================================================
class AlbaMaster : public wxApp
// ============================================================================
{
public:
  bool OnInit();
  int  OnExit();

	/** This function filters wxEvents and is used to control global hot keys*/
	int FilterEvent(wxEvent& event);

protected:
  albaLogicWithManagers *m_Logic;
	bool m_LogicInitialized;

	void OnFatalException();
};
DECLARE_APP(AlbaMaster)
#endif //__AlbaApp_H__
