/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaParabolicMeshToLinearMeshFilter
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaParabolicMeshToLinearMeshFilter_h
#define __albaParabolicMeshToLinearMeshFilter_h

#include "albaDefines.h"
#include "vtkUnstructuredGridAlgorithm.h"

/**
 albaParabolicMeshToLinearMeshFilter is a filter to linearize a mesh made of parabolic elements.
 Hybrid meshes are not supported ie all elements should be the same type!
 Currently supported cells are 10 nodes tetra and 20 nodes hexa.
 If the input mesh is already linear or made of unsupported type elements the filter is simply bypassed.
*/
class ALBA_EXPORT albaParabolicMeshToLinearMeshFilter : public vtkUnstructuredGridAlgorithm
{

public:
  
  vtkTypeMacro(albaParabolicMeshToLinearMeshFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static albaParabolicMeshToLinearMeshFilter *New();
  
protected:

  albaParabolicMeshToLinearMeshFilter();
  ~albaParabolicMeshToLinearMeshFilter();

	int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

private:

  albaParabolicMeshToLinearMeshFilter(const albaParabolicMeshToLinearMeshFilter&);  // Not implemented.
  void operator=(const albaParabolicMeshToLinearMeshFilter&);  // Not implemented.
};

#endif


