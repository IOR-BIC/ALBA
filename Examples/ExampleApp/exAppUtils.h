/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: exAppUtils.h,v $
Language:  C++
Date:      $Date: 2009-05-19 14:29:52 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni, Nicola Vanella
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafDefines.h"

#ifndef __exAppUtils_H__
#define __exAppUtils_H__

class exAppUtils
{
public:

  /** In DEBUG mode return LHPBuilder source code directory in WIN32 path format. 
  In RELEASE mode it returns the Application installation directory in WIN32 path format (same as mafGetApplicationDirectory)*/

  static wxString GetApplicationDirectory();
	static wxString GetConfigDirectory();
};
#endif