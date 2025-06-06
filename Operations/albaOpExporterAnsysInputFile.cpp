/*=========================================================================

Program: ALBA
Module: albaOpExporterAnsysInputFile.cpp
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

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

#include "albaOpExporterAnsysInputFile.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaSmartPointer.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEMesh.h"
#include "albaVMEMeshAnsysTextExporter.h"
#include "albaAbsMatrixPipe.h"
#include "albaProgressBarHelper.h"

#include <iostream>
#include <fstream>

// vtk includes
#include "vtkALBASmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkFieldData.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

// vcl includes

#include <vcl_map.h>
#include <vcl_vector.h>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterAnsysInputFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpExporterAnsysInputFile::albaOpExporterAnsysInputFile(const wxString &label) :
albaOpExporterAnsysCommon(label)
{
	m_CreateComponents = false;
}
//----------------------------------------------------------------------------
albaOpExporterAnsysInputFile::~albaOpExporterAnsysInputFile()
{

}

//----------------------------------------------------------------------------
albaOp* albaOpExporterAnsysInputFile::Copy()   
{
  albaOpExporterAnsysInputFile *cp = new albaOpExporterAnsysInputFile(m_Label);
  return cp;
}

//----------------------------------------------------------------------------
albaString albaOpExporterAnsysInputFile::GetWildcard()
{
  return "inp files (*.inp)|*.inp|All Files (*.*)|*.*";
}

//----------------------------------------------------------------------------
void albaOpExporterAnsysInputFile::AddSpecificGui()
{
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->Label("Components", true);
	m_Gui->Bool(ID_CREATE_COMPONENTS, "Create", &m_CreateComponents, 0);
}

//---------------------------------------------------------------------------
int albaOpExporterAnsysInputFile::Write()
{
  FILE *outFile;
  outFile = albaTryOpenFile(m_AnsysOutputFileNameFullPath.ToAscii(), "w");

  albaVMEMesh *input = albaVMEMesh::SafeDownCast(m_Input);

  input->Update();
  input->GetUnstructuredGridOutput()->Update();
  input->GetUnstructuredGridOutput()->GetVTKData()->Update();

	m_ProgressHelper = new albaProgressBarHelper(m_Listener);
	m_ProgressHelper->SetTextMode(m_TestMode);
  m_ProgressHelper->InitProgressBar("Please wait exporting file...");

  // File header
  WriteHeaderFile(outFile);

  // Nodes
  // N,210623,             -125.054497,             178.790497,             -297.887695
    
  WriteNodesFile(outFile);

  fprintf(outFile,"\n");

  // Materials
  // MP,EX,1,             26630.9
  // MP,NUXY,1,             0.3
  // MP,DENS,1,             1.73281
 
  WriteMaterialsFile(outFile);

  // Elements
  // TYPE, 3 $ MAT, 268 $ REAL, 1
  // EN,             2949927,             1815163,             1727014,             1822649,             1820606,             2096026,             2096028,             2247569,             2247567
  // EMORE,             2096027,             2291473
  // CM, TYPE3-REAL1-MAT268, ELEM

  fprintf(outFile,"\n\n");

  WriteElementsFile(outFile);

  // End file
  fprintf(outFile,"\nESEL, ALL\n\nFINISH\n");

  fclose(outFile);

  cppDEL(m_ProgressHelper);

  return ALBA_OK;
}

//---------------------------------------------------------------------------
int albaOpExporterAnsysInputFile::WriteHeaderFile(FILE *file )
{
  time_t rawtime;
  struct tm * timeinfo;  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  fprintf(file,"/TITLE,\n/COM, Generated by albaOpExporterAnsysInputFile %s/PREP7\n\n", asctime (timeinfo));

  return ALBA_OK;
}
//---------------------------------------------------------------------------
int albaOpExporterAnsysInputFile::WriteNodesFile(FILE *file )
{
  albaVMEMesh *input = albaVMEMesh::SafeDownCast(m_Input);
  assert(input);

  vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();

  vtkIntArray *nodesIDArray = albaVMEMesh::GetNodesIDArray(inputUGrid);
		
  vtkIntArray *syntheticNodesIDArray = NULL;

  if (nodesIDArray == NULL)
  {
    albaLogMessage("nodesID informations not found in vtk unstructured grid!\
                  Temporary nodes id array will be created in order to export the data.");

    int numPoints = inputUGrid->GetNumberOfPoints();
    syntheticNodesIDArray = vtkIntArray::New();

    int offset = 1 ;
    for (int i = 0; i < numPoints; i++) 
    {
      syntheticNodesIDArray->InsertNextValue(i + offset);
    }

    nodesIDArray = syntheticNodesIDArray;
  }

  assert(nodesIDArray != NULL);

  // get the pointsToBeExported

  vtkPoints *pointsToBeExported = NULL;

  vtkTransform *transform = NULL;
  vtkTransformFilter *transformFilter = NULL;
  vtkUnstructuredGrid *inUGDeepCopy = NULL;

  if (m_ABSMatrixFlag)
  {
    albaVMEMesh *inMesh = albaVMEMesh::SafeDownCast(m_Input);
    assert(inMesh);

    vtkMatrix4x4 *matrix = inMesh->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix();

    // apply abs matrix to geometry
    assert(matrix);

    transform = vtkTransform::New();
    transform->SetMatrix(matrix);

    transformFilter = vtkTransformFilter::New();
    inUGDeepCopy = vtkUnstructuredGrid::New();
    inUGDeepCopy->DeepCopy(inputUGrid);

    transformFilter->SetInput(inUGDeepCopy);
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    pointsToBeExported = transformFilter->GetOutput()->GetPoints();
  } 
  else
  {
    // do not transform geometry
    pointsToBeExported = inputUGrid->GetPoints();
  }

  // read all the pointsToBeExported in memory (vnl_matrix)

  double pointCoordinates[3] = {-9999, -9999, -9999};

  int rowsNumber = inputUGrid->GetNumberOfPoints();

  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    float nodProgress = rowID / m_TotalElements;
    m_ProgressHelper->UpdateProgressBar(nodProgress * 100);

    pointsToBeExported->GetPoint(rowID, pointCoordinates);

    fprintf(file,"N,%d,             %f,             %f,             %f\n", nodesIDArray->GetValue(rowID), pointCoordinates[0], pointCoordinates[1], pointCoordinates[2]);
  }

  m_CurrentProgress = rowsNumber;

  // clean up
  vtkDEL(inUGDeepCopy);
  vtkDEL(transform);
  vtkDEL(transformFilter);
  vtkDEL(syntheticNodesIDArray);

  nodesIDArray = NULL;

  return ALBA_OK;
}
//---------------------------------------------------------------------------
int albaOpExporterAnsysInputFile::WriteMaterialsFile(FILE *file)
{
	vtkFieldData *materialData = GetMaterialData();

  vtkDataArray *materialsIDArray = NULL;

  // try field data
  materialsIDArray = materialData->GetArray("material_id");
	  
  if (materialsIDArray == NULL)
  {
    albaLogMessage("material informations not found in vtk unstructured grid!\
                  A fake temporary material with ID = 1 will be created in order to export the data.");

    int fakeMaterialID = 1;
    double fakeMaterialTemperature = 0.0000; 
    wxString fakeMaterialPropertyName = "FAKEMATERIALUSEDFOREXPORTTOWARDANSYS";
    int fakeMaterialPropertyValue = 1;

    fprintf(file,"MP,%s,%d,             %f\n",fakeMaterialPropertyName, fakeMaterialID, fakeMaterialPropertyValue);
  }
  else
  {
    // get the number of materials
    int numberOfMaterials = materialsIDArray->GetNumberOfTuples();

    // get the number of materials properties
    int numberOfMaterialProperties = materialData->GetNumberOfArrays() - 1; // 1 is the materialsIDArray

    // gather material properties array names
    vcl_vector<wxString> materialProperties;
    for (int arrayID = 0; arrayID < materialData->GetNumberOfArrays(); arrayID++)
    {
      wxString arrayName = materialData->GetArray(arrayID)->GetName();
      if (arrayName != "material_id")
      {
        materialProperties.push_back(arrayName);
      }
    }

    // for each material
    for (int i = 0; i < numberOfMaterials; i++)
    {
      float matProgress = (m_CurrentProgress + i) / m_TotalElements;
      m_ProgressHelper->UpdateProgressBar(matProgress * 100);

      int materialID = materialsIDArray->GetTuple(i)[0];
      double materialTemperature = 0.0000;  // not supported for the moment  

      // for each property
      for (int j = 0; j < numberOfMaterialProperties; j++)
      {
        wxString arrayName = materialProperties[j];
        vtkDataArray *array = materialData->GetArray(arrayName.ToAscii());

        fprintf(file,"MP,%s,%d,             %.8lf\n",arrayName.ToAscii(), materialID, array->GetTuple(i)[0]);
      }

      fprintf(file,"\n");
    }  

    m_CurrentProgress += numberOfMaterials;
  }  

  return ALBA_OK;
}
//---------------------------------------------------------------------------
int albaOpExporterAnsysInputFile::WriteElementsFile(FILE *file)
{
  albaVMEMesh *input = albaVMEMesh::SafeDownCast(m_Input);
  assert(input);

  vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();

  // create elements matrix 
  int rowsNumber = inputUGrid->GetNumberOfCells();

  ExportElement *exportVector = CreateExportElements(input, rowsNumber, inputUGrid, file);

  fprintf(file,"\n");
  
  int currentMatID = -1;
  int currentType = -1;

  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    float elemProgress = (m_CurrentProgress + (float)rowID) / m_TotalElements;
    m_ProgressHelper->UpdateProgressBar(elemProgress * 100);

    if(exportVector[rowID].matID != currentMatID || exportVector[rowID].elementType != currentType)
    {
      fprintf(file, "TYPE, %d $ MAT, %d $ REAL, %d\n", exportVector[rowID].elementType, exportVector[rowID].matID, exportVector[rowID].elementReal); 
      currentMatID = exportVector[rowID].matID;
      currentType = exportVector[rowID].elementType;
    }

    fprintf(file, "EN,             %d", exportVector[rowID].elementID);

    vtkCell *currentCell = inputUGrid->GetCell(exportVector[rowID].cellID);
    vtkIdList *idList = currentCell->GetPointIds();
    int cellNpoints=currentCell->GetNumberOfPoints();
    for (int currentID = 0; currentID < cellNpoints;currentID++)
    {
      if(currentID == 8)  
        fprintf(file, "\nEMORE"); 

      fprintf(file, ",             %d",   input->GetNodesIDArray()->GetValue(idList->GetId(currentID)));
    }
		
		if((rowID == rowsNumber - 1) || (currentMatID != exportVector[rowID+1].matID || currentType != exportVector[rowID + 1].elementType))
    {
			if (m_CreateComponents)
				fprintf(file, "\nCM, TYPE%d-REAL%d-MAT%d, ELEM\n", exportVector[rowID].elementType, exportVector[rowID].elementReal, exportVector[rowID].matID);
			else
				fprintf(file, "\n");
    }

    fprintf(file,"\n");
  }

  delete [] exportVector;

  return ALBA_OK;
}


