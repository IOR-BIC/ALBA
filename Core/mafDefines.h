/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: mafDefines.h,v $
  Language:  C++
  Date:      $Date: 2004-08-24 12:46:49 $
  Version:   $Revision: 1.1 $

=========================================================================*/
// .NAME standard macros and constants for the MFL libray
// .SECTION Description
// This file include all the standard macros and constants used inside the MFL
// library.
// .SECTION ToDO
// - Make a test for the VME object factory 
#ifndef __mafDefines_h
#define __mafDefines_h

#include "Core/Configure.h"

typedef double mafTimeStamp;

#define MAF_OK                 0
#define MAF_ERROR              1

// Delete a VTK object
#define vtkDEL(a) if (a) { a->Delete(); a = NULL; }

// Allocate a new VTK object: don't worry, New is a static member!
#define vtkNEW(a) a=a->New()

// this is the type used for IDs inside the MAF
typedef unsigned long mafID;

#endif

