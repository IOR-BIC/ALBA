/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafZipUtility.h,v $
Language:  C++
Date:      $Date: 2010-11-29 16:53:42 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafZipUtility_h
#define __mafZipUtility_h

#include "mafDefines.h"
#include <vector>

class mafString;

/** extract the file archived in the zipFile in the directory where is placed the zip file.
Return the list of the files extracted. */
MAF_EXPORT std::vector<mafString> ZIPOpen(mafString zipFile);

#endif