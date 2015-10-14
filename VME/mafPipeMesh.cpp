/*=========================================================================

 Program: MAF2
 Module: mafPipeMesh
 Authors: Daniele Giunchi , Stefano Perticoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipeMesh.h"
#include "mafVMEOutputMesh.h"
#include "mafVMEMesh.h"
#include "mafParabolicMeshToLinearMeshFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeMesh);
//----------------------------------------------------------------------------
//#include "vtkMemDbg.h"

//----------------------------------------------------------------------------
mafPipeMesh::mafPipeMesh()
:mafPipeGenericPolydata()
//----------------------------------------------------------------------------
{
  m_BorderElementsWiredActor = 1;
}

//----------------------------------------------------------------------------
mafPipeMesh::~mafPipeMesh()
//----------------------------------------------------------------------------
{
  vtkDEL(m_LinearizationFilter);
	vtkDEL(m_GeometryFilter);
}

//----------------------------------------------------------------------------
vtkPolyData * mafPipeMesh::GetInputAsPolyData()
{
	assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputMesh));
	mafVMEOutputMesh *mesh_output = mafVMEOutputMesh::SafeDownCast(m_Vme->GetOutput());
	assert(mesh_output);
	mesh_output->Update();
	vtkUnstructuredGrid *data = vtkUnstructuredGrid::SafeDownCast(mesh_output->GetVTKData());
	assert(data);
	data->Update();

	// create the linearization filter
	vtkNEW(m_LinearizationFilter);
	m_LinearizationFilter->SetInput(data);
	m_LinearizationFilter->Update();

	vtkNEW(m_GeometryFilter);
	m_GeometryFilter->SetInput(m_LinearizationFilter->GetOutput());
	m_GeometryFilter->Update();

	return m_GeometryFilter->GetOutput();
}

