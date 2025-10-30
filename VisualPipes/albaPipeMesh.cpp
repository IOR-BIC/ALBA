/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMesh
 Authors: Daniele Giunchi , Stefano Perticoni, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaPipeMesh.h"
#include "albaVMEOutputMesh.h"
#include "albaVMEMesh.h"
#include "albaParabolicMeshToLinearMeshFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeMesh);
//----------------------------------------------------------------------------
//#include "vtkMemDbg.h"

//----------------------------------------------------------------------------
albaPipeMesh::albaPipeMesh()
:albaPipeGenericPolydata()
//----------------------------------------------------------------------------
{
  m_BorderElementsWiredActor = 1;
	m_LinearizationFilter = NULL;
	m_GeometryFilter = NULL;
}

//----------------------------------------------------------------------------
albaPipeMesh::~albaPipeMesh()
//----------------------------------------------------------------------------
{
  vtkDEL(m_LinearizationFilter);
	vtkDEL(m_GeometryFilter);
}

//----------------------------------------------------------------------------
vtkAlgorithmOutput* albaPipeMesh::GetPolyDataOutputPort()
{
	if (!m_PolydataConnection)
	{
		assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputMesh));
		albaVMEOutputMesh* mesh_output = albaVMEOutputMesh::SafeDownCast(m_Vme->GetOutput());
		assert(mesh_output);
		mesh_output->Update();
		vtkUnstructuredGrid* data = vtkUnstructuredGrid::SafeDownCast(mesh_output->GetVTKData());
		assert(data);

		// create the linearization filter
		vtkNEW(m_LinearizationFilter);
		m_LinearizationFilter->SetInputData(data);
		m_LinearizationFilter->Update();

		vtkNEW(m_GeometryFilter);
		m_GeometryFilter->SetInputConnection(m_LinearizationFilter->GetOutputPort());
		m_GeometryFilter->Update();

		m_PolydataConnection = m_GeometryFilter->GetOutputPort();
	}

	return m_PolydataConnection;
}

