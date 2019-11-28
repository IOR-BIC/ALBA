/*=========================================================================
Program:   AlbaViewer
Module:    AlbaViewer.h
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __AlbaViewer_H__
#define __AlbaViewer_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "appLogic.h" 

//----------------------------------------------------------------------------
// Class Name: AlbaViewer
//----------------------------------------------------------------------------
class AlbaViewer : public wxApp
{
public:
	
	bool OnInit();	

	int  OnExit();

	/** This function filters wxEvents and is used to control global hotKeys */
	int FilterEvent(wxEvent& event);

protected:

	/** Include icon files */
	void InitializeIcons();

	/** Manage Fatal Exception - Try to save project */
	void OnFatalException();

	appLogic *m_Logic;
};
DECLARE_APP(AlbaViewer)
#endif 
