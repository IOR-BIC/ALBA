/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMesh
 Authors: Daniele Giunchi , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeMesh_H__
#define __albaPipeMesh_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipeGenericPolydata.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkGeometryFilter;
class albaParabolicMeshToLinearMeshFilter;

//----------------------------------------------------------------------------
// albaPipeMesh :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeMesh : public albaPipeGenericPolydata
{
public:
	albaTypeMacro(albaPipeMesh,albaPipeGenericPolydata);

	albaPipeMesh();
	virtual     ~albaPipeMesh();

protected:

	virtual vtkAlgorithmOutput* GetPolyDataOutputPort();

	vtkGeometryFilter                  *m_GeometryFilter;
  albaParabolicMeshToLinearMeshFilter *m_LinearizationFilter;
};  
#endif // __albaPipeMesh_H__
