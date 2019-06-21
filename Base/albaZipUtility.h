/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaZipUtility
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaZipUtility_h
#define __albaZipUtility_h

#include "albaDefines.h"
#include <vector>

class albaString;

/** extract the file archived in the zipFile in the directory where is placed the zip file.
Return the list of the files extracted. */
ALBA_EXPORT std::vector<albaString> ZIPOpen(albaString zipFile);

#endif