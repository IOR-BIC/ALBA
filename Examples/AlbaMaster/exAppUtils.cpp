/*=========================================================================
Program:   AlbaMaster
Module:    exAppUtils.cpp
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

#include "albaDecl.h"
#include "exAppUtils.h"
#include "defines.h"

//----------------------------------------------------------------------------
wxString exAppUtils::GetApplicationDirectory()
{
#ifndef _DEBUG
	// RELEASE 
	wxString applicationDirectory = albaGetApplicationDirectory().char_str();
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
wxString exAppUtils::GetConfigDirectory()
{
	//getting the Config directory
	wxString config_dir = albaGetApplicationDirectory().char_str();

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