/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVersion.h,v $
  Language:  C++
  Date:      $Date: 2005-01-11 17:35:03 $
  Version:   $Revision: 1.3 $
  Authors:   based on mafVersion (www.vtk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVersion_h
#define __mafVersion_h


#include "mafBase.h" 
#include "mafConfigure.h"

#define MAF_SOURCE_VERSION "maf version " MAF_VERSION ", maf source $Revision: 1.3 $, $Date: 2005-01-11 17:35:03 $ (GMT)"

/** mafVersion - Versioning class for MAF.
  Holds methods for defining/determining the current MAF version
  (major, minor, build).

  @beware
  This file will change frequently to update the MAF_SOURCE_VERSION which
  timestamps a particular source release.
*/
class MAF_EXPORT mafVersion : public mafBase
{
public:
  mafVersion() {};
  ~mafVersion() {};

  /**
    Return the version of maf this object is a part of.
    A variety of methods are included. GetMAFSourceVersion returns a string
    with an identifier which timestamps a particular source tree.*/
  static const char *GetMAFVersion() { return MAF_VERSION; }
  static int GetMAFMajorVersion() { return MAF_MAJOR_VERSION; }
  static int GetMAFMinorVersion() { return MAF_MINOR_VERSION; }
  static int GetMAFBuildVersion() { return MAF_BUILD_VERSION; }
  static const char *GetMAFSourceVersion() { return MAF_SOURCE_VERSION; }

private:
  mafVersion(const mafVersion&);  // Not implemented.
  void operator=(const mafVersion&);  // Not implemented.
};

#endif 

