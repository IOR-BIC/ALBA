/*=========================================================================
Program:   AlbaViewer
Module:    appUtils.h
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

#include "albaDefines.h"

#ifndef __appUtils_H__
#define __appUtils_H__

//----------------------------------------------------------------------------
// Class Name: appUtils
//----------------------------------------------------------------------------
class appUtils
{
public:

  /** In DEBUG mode return Application source code directory in WIN32 path format. 
  In RELEASE mode it returns the Application installation directory in WIN32 path format (same as albaGetApplicationDirectory)*/

  static wxString GetApplicationDirectory();
	static wxString GetConfigDirectory();
};
#endif