/*=========================================================================

 Program: MAF2
 Module: mafParabolicMeshToLinearMeshFilter
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafParabolicMeshToLinearMeshFilter_h
#define __mafParabolicMeshToLinearMeshFilter_h

#include "mafDefines.h"
#include "vtkUnstructuredGridToUnstructuredGridFilter.h"

/**
 mafParabolicMeshToLinearMeshFilter is a filter to linearize a mesh made of parabolic elements.
 Hybrid meshes are not supported ie all elements should be the same type!
 Currently supported cells are 10 nodes tetra and 20 nodes hexa.
 If the input mesh is already linear or made of unsupported type elements the filter is simply bypassed.
*/
class MAF_EXPORT mafParabolicMeshToLinearMeshFilter : public vtkUnstructuredGridToUnstructuredGridFilter
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


