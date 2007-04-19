/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafParabolicMeshToLinearMeshFilter.h,v $
Language:  C++
Date:      $Date: 2007-04-19 14:41:51 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafParabolicMeshToLinearMeshFilter_h
#define __mafParabolicMeshToLinearMeshFilter_h

#include "vtkUnstructuredGridToUnstructuredGridFilter.h"

/**
 mafParabolicMeshToLinearMeshFilter is a filter to linearize a mesh made of parabolic elements.
 Hybrid meshes are not supported ie all elements should be the same type!
 Currently supported cells are 10 nodes tetra and 20 nodes hexa.
 If the input mesh is already linear or made of unsupported type elements the filter is simply bypassed.
*/
class VTK_FILTERING_EXPORT mafParabolicMeshToLinearMeshFilter : public vtkUnstructuredGridToUnstructuredGridFilter
{

public:
  
  vtkTypeRevisionMacro(mafParabolicMeshToLinearMeshFilter,vtkUnstructuredGridToUnstructuredGridFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  static mafParabolicMeshToLinearMeshFilter *New();
  
protected:

  mafParabolicMeshToLinearMeshFilter();
  ~mafParabolicMeshToLinearMeshFilter();

  void Execute();

private:

  mafParabolicMeshToLinearMeshFilter(const mafParabolicMeshToLinearMeshFilter&);  // Not implemented.
  void operator=(const mafParabolicMeshToLinearMeshFilter&);  // Not implemented.
};

#endif


