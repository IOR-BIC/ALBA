/*=========================================================================
Program:   AlbaMaster
Module:    exAppUtils.h
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

#include "albaDefines.h"

#ifndef __exAppUtils_H__
#define __exAppUtils_H__

class exAppUtils
{
public:

  /** In DEBUG mode return LHPBuilder source code directory in WIN32 path format. 
  In RELEASE mode it returns the Application installation directory in WIN32 path format (same as albaGetApplicationDirectory)*/

  static wxString GetApplicationDirectory();
	static wxString GetConfigDirectory();
};
#endif