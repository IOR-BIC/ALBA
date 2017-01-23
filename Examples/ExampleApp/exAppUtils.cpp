/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: exAppUtils.cpp,v $
Language:  C++
Date:      $Date: 2009-05-19 14:29:52 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni, Nicola Vanella
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDecl.h"
#include "exAppUtils.h"
#include "defines.h"

//----------------------------------------------------------------------------
wxString exAppUtils::GetApplicationDirectory()
{
#ifndef _DEBUG
	// RELEASE 
	wxString applicationDirectory = mafGetApplicationDirectory().c_str();
	return applicationDirectory;

#else
	// DEBUG
	wxString sourceDir = MAF_SOURCE_DIR;

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
	wxString config_dir = mafGetApplicationDirectory().c_str();

#ifdef _DEBUG
	config_dir = MAF_SOURCE_DIR;
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