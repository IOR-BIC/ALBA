/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMeshSlice
 Authors: Daniele Giunchi
 
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

#include "albaPipeMeshSlice.h"
#include "albaTransformBase.h"
#include "albaVMEOutputMesh.h"
#include "albaVMEMesh.h"
#include "vtkALBAMeshCutter.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPlane.h"
#include "vtkALBAPolyDataNormals.h"
#include "vtkALBAToLinearTransform.h"

#include <vector>

// local debug facility
const bool DEBUG_MODE = true;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeMeshSlice);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
albaPipeMeshSlice::albaPipeMeshSlice()
:albaPipeGenericPolydata()
//----------------------------------------------------------------------------
{
	m_BorderElementsWiredActor = 1;
  m_Plane           = NULL;
  m_Cutter          = NULL;
}

//----------------------------------------------------------------------------
albaPipeMeshSlice::~albaPipeMeshSlice()
{
	vtkDEL(m_Plane);
	vtkDEL(m_Cutter);
}

//----------------------------------------------------------------------------
vtkPolyData* albaPipeMeshSlice::GetInputAsPolyData()
{
	if (!m_InputAsPolydata)
	{
		assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputMesh));
		albaVMEOutputMesh *mesh_output = albaVMEOutputMesh::SafeDownCast(m_Vme->GetOutput());
		assert(mesh_output);
		mesh_output->Update();
		vtkUnstructuredGrid *data = vtkUnstructuredGrid::SafeDownCast(mesh_output->GetVTKData());
		assert(data);
		data->Update();

		m_Plane = vtkPlane::New();
		m_Cutter = vtkALBAMeshCutter::New();

		m_Plane->SetOrigin(m_Origin);
		m_Plane->SetNormal(m_Normal);

		vtkALBAToLinearTransform* m_VTKTransform = vtkALBAToLinearTransform::New();
		m_VTKTransform->SetInputMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrixPointer());
		m_Plane->SetTransform(m_VTKTransform);

		m_Cutter->SetInput(data);
		m_Cutter->SetCutFunction(m_Plane);
		m_Cutter->GetOutput()->Update();
		m_Cutter->Update();

		m_InputAsPolydata = m_Cutter->GetOutput();
	}

	return m_InputAsPolydata;
}

//----------------------------------------------------------------------------
/*virtual*/ void albaPipeMeshSlice::SetSlice(double *Origin, double *Normal)
{
  if (Origin != NULL)
  {
    m_Origin[0] = Origin[0];
    m_Origin[1] = Origin[1];
    m_Origin[2] = Origin[2];
  }

  if (Normal != NULL)
  {
    m_Normal[0] = Normal[0];
    m_Normal[1] = Normal[1];
    m_Normal[2] = Normal[2];
  }
	
	if(m_Plane && m_Cutter)
	{
    m_Plane->SetNormal(m_Normal);
		m_Plane->SetOrigin(m_Origin);
		m_Cutter->SetCutFunction(m_Plane);
		m_Cutter->Update();
    
		if(m_NormalsFilter)
			m_NormalsFilter->Update();
	}
	
}