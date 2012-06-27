/*=========================================================================

 Program: MAF2
 Module: mafZipUtility
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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