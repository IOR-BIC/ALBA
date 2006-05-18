/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafASCIIImporterUtility.h,v $
Language:  C++
Date:      $Date: 2006-05-18 10:29:00 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafASCIIImporterUtility_h
#define __mafASCIIImporterUtility_h

#include <vnl\vnl_matrix.h>

//----------------------------------------------------------------------------
class mafASCIIImporterUtility
//----------------------------------------------------------------------------
{
public:
  mafASCIIImporterUtility(){};
  ~mafASCIIImporterUtility() {};
  
  // Description:
  // Read the matrix from a filename by means of the VNL library
  int ReadMatrix(vnl_matrix<double> &M, const char *fname);
};
#endif
