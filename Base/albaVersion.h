/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVersion
 Authors: based on albaVersion (www.vtk.org), adapted by Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVersion_h
#define __albaVersion_h


#include "albaBase.h" 
#include "albaConfigure.h"

#define ALBA_SOURCE_VERSION "alba version " ALBA_VERSION ", alba source $Revision: 1.4 $, $Date: 2005-02-20 23:33:19 $ (GMT)"

/** albaVersion - Versioning class for ALBA.
  Holds methods for defining/determining the current ALBA version
  (major, minor, build).

  @beware
  This file will change frequently to update the ALBA_SOURCE_VERSION which
  timestamps a particular source release.
*/
class ALBA_EXPORT albaVersion : public albaBase
{
public:
  albaVersion() {};
  ~albaVersion() {};

  /**
    Return the version of alba this object is a part of.
    A variety of methods are included. GetALBASourceVersion returns a string
    with an identifier which timestamps a particular source tree.*/
  static const char *GetALBAVersion() { return ALBA_VERSION; }
  static int GetALBAMajorVersion() { return ALBA_MAJOR_VERSION; }
  static int GetALBAMinorVersion() { return ALBA_MINOR_VERSION; }
  static const char *GetALBASourceVersion() { return ALBA_SOURCE_VERSION; }

private:
  albaVersion(const albaVersion&);  // Not implemented.
  void operator=(const albaVersion&);  // Not implemented.
};

#endif 

