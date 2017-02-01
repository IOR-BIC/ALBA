/*=========================================================================

 Program: MAF2
 Module: mafPipeMesh
 Authors: Daniele Giunchi , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeMesh_H__
#define __mafPipeMesh_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipeGenericPolydata.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkGeometryFilter;
class mafParabolicMeshToLinearMeshFilter;

//----------------------------------------------------------------------------
// mafPipeMesh :
//----------------------------------------------------------------------------
class MAF_EXPORT mafPipeMesh : public mafPipeGenericPolydata
{
public:
	mafTypeMacro(mafPipeMesh,mafPipeGenericPolydata);

	mafPipeMesh();
	virtual     ~mafPipeMesh();

protected:

	vtkPolyData *GetInputAsPolyData();

	vtkGeometryFilter                  *m_GeometryFilter;
  mafParabolicMeshToLinearMeshFilter *m_LinearizationFilter;
};  
#endif // __mafPipeMesh_H__
