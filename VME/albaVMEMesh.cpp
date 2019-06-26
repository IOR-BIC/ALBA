/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMesh
 Authors: Stefano Perticoni
 
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



#include "albaVMEMesh.h"
#include "albaVME.h"
#include "albaMatrixInterpolator.h"
#include "albaDataVector.h"
#include "albaDataPipeInterpolatorVTK.h"
#include "albaVMEItemVTK.h"
#include "albaAbsMatrixPipe.h"
#include "mmaMaterial.h"

#include "vtkDataSet.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEMesh)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEMesh::albaVMEMesh()
//-------------------------------------------------------------------------
{

}

//-------------------------------------------------------------------------
albaVMEMesh::~albaVMEMesh()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in albaVME
  // data vector destroyed in albaVMEGeneric

  //cppDEL(m_MaterialButton);
}

//-------------------------------------------------------------------------
albaVMEOutput *albaVMEMesh::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputMesh::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
int albaVMEMesh::InternalInitialize()
//-------------------------------------------------------------------------
{
	if (Superclass::InternalInitialize()==ALBA_OK)
	{
		// force material allocation
		GetMaterial();
		GetMaterial()->m_MaterialType = mmaMaterial::USE_VTK_PROPERTY;
		return ALBA_OK;
	}
	return ALBA_ERROR;
}
//-------------------------------------------------------------------------
int albaVMEMesh::SetData(vtkUnstructuredGrid *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkUnstructuredGrid *unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(data);

  if(unstructuredGrid)
  {
    unstructuredGrid->Update();
    return Superclass::SetData(unstructuredGrid,t,mode);
  }
  else
  {
    albaErrorMacro("Trying to set the wrong type of data inside a VME Mesh :"<< (data?data->GetClassName():"NULL"));
    return ALBA_ERROR;
  }
}
//-------------------------------------------------------------------------
int albaVMEMesh::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkUnstructuredGrid *unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(data);

  if (unstructuredGrid) 
  {
    unstructuredGrid->Update();
    return Superclass::SetData(data,t,mode);
  }
  else
  {
    albaErrorMacro("Trying to set the wrong type of data inside a VME Mesh :"<< (data?data->GetClassName():"NULL"));
    return ALBA_ERROR;
  }
}

//-------------------------------------------------------------------------
char** albaVMEMesh::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "albaVMEFem.xpm"
  return albaVMEFem_xpm;
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMEMesh::GetMaterial()
//-------------------------------------------------------------------------
{
	mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute("MaterialAttributes", material);
		if (m_Output)
		{
			((albaVMEOutputMesh *)m_Output)->SetMaterial(material);
		}
	}
	return material;
}

//------------------------------------------------------------------------
vtkIntArray *albaVMEMesh::GetIntCellArray(vtkUnstructuredGrid *inputUGrid, const char *arrayName, const char *arrayName2)
{
  vtkIntArray *myArray = NULL;

  // get the ELEMENT_ID array
  myArray = vtkIntArray::SafeDownCast(inputUGrid->GetCellData()->GetArray(arrayName));
  
  if(myArray==NULL)
    myArray = vtkIntArray::SafeDownCast(inputUGrid->GetCellData()->GetArray(arrayName2));  

  return myArray;
}

//------------------------------------------------------------------------
vtkIntArray *albaVMEMesh::GetIntCellArray( const char *arrayName, const char *arrayName2)
{
	vtkUnstructuredGrid *inputUGrid = GetUnstructuredGridOutput()->GetUnstructuredGridData();

	return GetIntCellArray(inputUGrid,arrayName,arrayName2);	
}


//------------------------------------------------------------------------
vtkIntArray *albaVMEMesh::GetNodesIDArray()
{
  vtkUnstructuredGrid *inputUGrid = GetUnstructuredGridOutput()->GetUnstructuredGridData();

  // get the Nodes Id array
  vtkIntArray *myArray = vtkIntArray::SafeDownCast(inputUGrid->GetPointData()->GetArray("Id"));  

  if(myArray==NULL)
    myArray = vtkIntArray::SafeDownCast(inputUGrid->GetPointData()->GetArray("id"));  

  return myArray;
}

//----------------------------------------------------------------------------
vtkIntArray * albaVMEMesh::GetNodesIDArray(vtkUnstructuredGrid *inputUGrid)
{
	// get the Nodes Id array
	vtkIntArray *myArray = vtkIntArray::SafeDownCast(inputUGrid->GetPointData()->GetArray("Id"));  

	if(myArray==NULL)
		myArray = vtkIntArray::SafeDownCast(inputUGrid->GetPointData()->GetArray("id"));  

	return myArray;
}

//------------------------------------------------------------------------
vtkIntArray *albaVMEMesh::GetElementsIDArray()
{
  return GetIntCellArray("Id", "ANSYS_ELEMENT_ID");
}

//----------------------------------------------------------------------------
vtkIntArray * albaVMEMesh::GetElementsIDArray(vtkUnstructuredGrid *inputUGrid)
{
	 return GetIntCellArray(inputUGrid,"Id", "ANSYS_ELEMENT_ID");
}

//------------------------------------------------------------------------
vtkIntArray *albaVMEMesh::GetElementsTypeArray()
{
  return GetIntCellArray("Type", "ANSYS_ELEMENT_TYPE");
}

//----------------------------------------------------------------------------
vtkIntArray * albaVMEMesh::GetElementsTypeArray(vtkUnstructuredGrid *inputUGrid)
{
	return GetIntCellArray(inputUGrid,"Type", "ANSYS_ELEMENT_TYPE");
}

//------------------------------------------------------------------------
vtkIntArray *albaVMEMesh::GetElementsRealArray()
{
  return GetIntCellArray("Real", "ANSYS_ELEMENT_REAL");
}

//----------------------------------------------------------------------------
vtkIntArray * albaVMEMesh::GetElementsRealArray(vtkUnstructuredGrid *inputUGrid)
{
	return GetIntCellArray(inputUGrid,"Real", "ANSYS_ELEMENT_REAL");
}
