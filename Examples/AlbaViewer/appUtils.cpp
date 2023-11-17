/*=========================================================================
Program:   AlbaViewer
Module:    appUtils.cpp
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

#include "albaDecl.h"
#include "appUtils.h"
#include "defines.h"

/*#include "albaDefines.h" */
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

// #include "appUtils.h"
// 
// #include "albaDecl.h"

//----------------------------------------------------------------------------
wxString appUtils::GetApplicationDirectory()
{
#ifndef _DEBUG
	// RELEASE 
	wxString applicationDirectory = albaGetApplicationDirectory().ToAscii();
	return applicationDirectory;

#else
	// DEBUG
	wxString sourceDir = ALBA_SOURCE_DIR;

	for (unsigned int i = 0; i < sourceDir.Length(); i++)
	{
		if (sourceDir[i] == '/')
			sourceDir[i] = '\\';
	}

	return sourceDir;

#endif
}

//----------------------------------------------------------------------------
wxString appUtils::GetConfigDirectory()
{
	//getting the Config directory
	wxString config_dir = albaGetApplicationDirectory().ToAscii();

#ifdef _DEBUG
	config_dir = ALBA_SOURCE_DIR;
	config_dir += "\\Installer";
#endif
	config_dir += "\\Config";

	for (unsigned int i = 0; i < config_dir.Length(); i++)
	{
		if (config_dir[i] == '/')
			config_dir[i] = '\\';
	}

	return config_dir;
}