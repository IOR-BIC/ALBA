/*=========================================================================
  Program:   Bonemat
  Module:    albaOpExporterAbaqusFile.cpp
  Language:  C++
  Date:      $Date: 2010-12-03 14:58:16 $
  Version:   $Revision: 1.1.1.1.2.3 $
  Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"

#include "albaOpExporterAbaqusFile.h"

#include "wx/stdpaths.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaSmartPointer.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEMesh.h"
#include "albaVMEMeshAnsysTextExporter.h"
#include "albaAbsMatrixPipe.h"
#include "albaGUIRollOut.h"
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
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_map.h>
#include <vcl_vector.h>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterAbaqusFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpExporterAbaqusFile::albaOpExporterAbaqusFile(const wxString &label) :
	albaOpExporterFEMCommon(label)
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_ImportedVmeMesh = NULL;

  m_AbaqusOutputFileNameFullPath		= "";
  wxStandardPaths std_paths;

  m_Pid = -1;
  m_ABSMatrixFlag = 1;
	m_EnableBackCalculation = 1;
}
//----------------------------------------------------------------------------
albaOpExporterAbaqusFile::~albaOpExporterAbaqusFile()
{
  for(int i=0; i< m_Elsets.size();i++)
  {
    for (int j=0; j<m_Elsets[i].elementsIDVect.size(); j++)
    {
      m_Elsets[i].elementsIDVect.clear();
    }
  }

  m_Elsets.clear();
  m_MatIDMap.clear();

  albaDEL(m_ImportedVmeMesh);
}

//----------------------------------------------------------------------------
albaOp* albaOpExporterAbaqusFile::Copy()   
{
  albaOpExporterAbaqusFile *cp = new albaOpExporterAbaqusFile(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
bool albaOpExporterAbaqusFile::Accept(albaVME *node)
{
  return (node->IsA("albaVMEMesh"));
}
//----------------------------------------------------------------------------
void albaOpExporterAbaqusFile::OpRun()   
{
	SetDefaultFrequencyFile();
  CreateGui();
}
//----------------------------------------------------------------------------
albaString albaOpExporterAbaqusFile::GetWildcard()
{
  return "inp files (*.inp)|*.inp|All Files (*.*)|*.*";
}

//----------------------------------------------------------------------------
void albaOpExporterAbaqusFile::OnEvent(albaEventBase *alba_event) 
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      {
        OnOK();
        this->OpStop(OP_RUN_OK);
      }
      break;
    case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
      }
      break;
		case ID_ENABLE_BACKCALCULATION:
		case ID_RHO_DENSITY_INTERVALS_NUMBER:
		{
			UpdateGui();
		}
    default:
      albaEventMacro(*e);
      break;
    }	
  }
}

//----------------------------------------------------------------------------
void albaOpExporterAbaqusFile::OnOK()
{
  albaString wildcard = GetWildcard();

  m_AbaqusOutputFileNameFullPath = "";

  wxString f;
  f = albaGetSaveFile("",wildcard).c_str(); 
  if(!f.IsEmpty())
  {
    m_AbaqusOutputFileNameFullPath = f;
    Write();
  }
}
//----------------------------------------------------------------------------
void albaOpExporterAbaqusFile::OpStop(int result)
{
  HideGui();
  albaEventMacro(albaEvent(this,result));        
}

//----------------------------------------------------------------------------
void albaOpExporterAbaqusFile::CreateGui()
{
	Superclass::CreateGui();

	m_Gui->Divider(2);

  m_Gui->Label("Absolute matrix",true);
  m_Gui->Bool(ID_ABS_MATRIX_TO_STL,"Apply",&m_ABSMatrixFlag,0);
	m_Gui->Divider(1);

	LoadConfigurationTags();

	if (m_HasConfiguration)
	{
		m_Gui->Label("Back Calculation", true);
		m_Gui->Bool(ID_ENABLE_BACKCALCULATION, "Enable", &m_EnableBackCalculation);

		//////////////////////////////////////////////////////////////////////////
		// Density - Elasticity
		m_Gui->Label("Density-elasticity relationship", true);
		m_Gui->Label("E = a + b * Rho^c", false);

		m_Gui->Label("Minimum Elasticity Modulus", false);
		m_Gui->Double(ID_MIN_ELASTICITY, "", &m_Configuration.minElasticity);
		m_Gui->Divider();
		m_Gui->Divider();
		m_Gui->Divider();

		const wxString densityChoices[] = { "Single interval", "Three intervals" };
		m_Gui->Combo(ID_RHO_DENSITY_INTERVALS_NUMBER, "", &m_Configuration.densityIntervalsNumber, 2, densityChoices);

		m_GuiASDensityOneInterval = new albaGUI(this);

		m_GuiASDensityOneInterval->Double(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_0, "a", &m_Configuration.a_OneInterval);
		m_GuiASDensityOneInterval->Double(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_1, "b", &m_Configuration.b_OneInterval);
		m_GuiASDensityOneInterval->Double(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_2, "c", &m_Configuration.c_OneInterval);

		m_GuiRollOutDensityOneInterval = m_Gui->RollOut(ID_DENSITY_ONE_INTERVAL_ROLLOUT, _("Single interval"), m_GuiASDensityOneInterval);

		m_Gui->Divider();

		m_GuiASDensityThreeIntervals = new albaGUI(this);

		m_GuiASDensityThreeIntervals->Double(ID_DENSITY_INTERVAL_0, "Rho1", &m_Configuration.rho1);
		m_GuiASDensityThreeIntervals->Double(ID_DENSITY_INTERVAL_1, "Rho2", &m_Configuration.rho2);

		m_GuiASDensityThreeIntervals->Label("Rho < Rho1");
		m_GuiASDensityThreeIntervals->Double(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, "a", &m_Configuration.a_RhoLessThanRho1);
		m_GuiASDensityThreeIntervals->Double(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, "b", &m_Configuration.b_RhoLessThanAsh1);
		m_GuiASDensityThreeIntervals->Double(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, "c", &m_Configuration.c_RhoLessThanRho1);

		m_GuiASDensityThreeIntervals->Label("Rho1 <= Rho <= Rho2");
		m_GuiASDensityThreeIntervals->Double(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, "a", &m_Configuration.a_RhoBetweenRho1andRho2);
		m_GuiASDensityThreeIntervals->Double(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, "b", &m_Configuration.b_RhoBetweenRho1andRho2);
		m_GuiASDensityThreeIntervals->Double(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, "c", &m_Configuration.c_RhoBetweenRho1andRho2);

		m_GuiASDensityThreeIntervals->Label("Rho > Rho2");
		m_GuiASDensityThreeIntervals->Double(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, "a", &m_Configuration.a_RhoBiggerThanRho2);
		m_GuiASDensityThreeIntervals->Double(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, "b", &m_Configuration.b_RhoBiggerThanRho2);
		m_GuiASDensityThreeIntervals->Double(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, "c", &m_Configuration.c_RhoBiggerThanRho2);

		m_GuiASDensityThreeIntervals->Divider(2);

		//   EnableTwoIntervals(true);

		m_GuiASDensityThreeIntervals->Enable(ID_DENSITY_INTERVAL_0, true);
		m_GuiASDensityThreeIntervals->Enable(ID_DENSITY_INTERVAL_1, true);

		m_GuiASDensityThreeIntervals->Enable(ID_RHO_DENSITY_INTERVALS_NUMBER, true);

		m_GuiRollOutDensityThreeIntervals = m_Gui->RollOut(ID_DENSITY_THREE_INTERVALS_ROLLOUT, _("Three intervals"), m_GuiASDensityThreeIntervals);

		UpdateGui();
	}
	//////////////////////////////////////////////////////////////////////////
	
  m_Gui->OkCancel();  
  m_Gui->Divider();

  ShowGui();
}

//----------------------------------------------------------------------------
void albaOpExporterAbaqusFile::UpdateGui()
{
	bool enable = m_EnableBackCalculation == 1;

	m_Gui->Enable(ID_MIN_ELASTICITY, enable);
	m_Gui->Enable(ID_RHO_DENSITY_INTERVALS_NUMBER, enable);
	m_Gui->Enable(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_0, enable);
	m_Gui->Enable(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_1, enable);
	m_Gui->Enable(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_2, enable);
	m_Gui->Enable(ID_DENSITY_ONE_INTERVAL_ROLLOUT, enable);
	m_Gui->Enable(ID_DENSITY_INTERVAL_0, enable);
	m_Gui->Enable(ID_DENSITY_INTERVAL_1, enable);
	m_Gui->Enable(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, enable);
	m_Gui->Enable(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, enable);
	m_Gui->Enable(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, enable);
	m_Gui->Enable(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, enable);
	m_Gui->Enable(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, enable);
	m_Gui->Enable(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, enable);
	m_Gui->Enable(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, enable);
	m_Gui->Enable(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, enable);
	m_Gui->Enable(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, enable);
	m_Gui->Enable(ID_DENSITY_THREE_INTERVALS_ROLLOUT, enable);

	m_GuiRollOutDensityOneInterval->RollOut(m_Configuration.densityIntervalsNumber == 0 && enable ? true : false);
	m_Gui->Enable(ID_DENSITY_ONE_INTERVAL_ROLLOUT, m_Configuration.densityIntervalsNumber == 0 && enable ? true : false);

	m_GuiRollOutDensityThreeIntervals->RollOut(m_Configuration.densityIntervalsNumber == 1 && enable ? true : false);
	m_Gui->Enable(ID_DENSITY_THREE_INTERVALS_ROLLOUT, m_Configuration.densityIntervalsNumber == 1 && enable ? true : false);

	m_GuiASDensityOneInterval->Update();
	m_GuiASDensityThreeIntervals->Update();

	m_Gui->FitGui();
	m_Gui->Update();
}

//----------------------------------------------------------------------------
int albaOpExporterAbaqusFile::compareElem(const void *p1, const void *p2) 
{
  ExportElement *a, *b;
  a = (ExportElement *)p1;
  b = (ExportElement *)p2;

  double result;

  result = a->elementType - b->elementType;  
  if (result < 0)
    return -1;
  else if (result > 0)
    return 1;
  else
  {
    result = a->matID - b->matID;  
    if (result < 0)
      return -1;
    else if (result > 0)
      return 1;
    else
      return 0;
  }
}

//----------------------------------------------------------------------------
long albaOpExporterAbaqusFile::GetPid()   
{
  return m_Pid;
}

//---------------------------------------------------------------------------
int albaOpExporterAbaqusFile::Write()
{
  FILE *outFile;
  outFile = fopen(m_AbaqusOutputFileNameFullPath.c_str(), "w");

  albaVMEMesh *input = albaVMEMesh::SafeDownCast(m_Input);

  input->Update();
  input->GetUnstructuredGridOutput()->Update();
  input->GetUnstructuredGridOutput()->GetVTKData()->Update();

  // Init ProgressBar
  m_ProgressHelper = new albaProgressBarHelper(m_Listener);
  m_ProgressHelper->SetTextMode(m_TestMode);
  m_ProgressHelper->InitProgressBar("Please wait exporting file...");


  // *Heading
  //  ** Job name: test Model name: test
  //  ** Generated by: py_bonemat_abaqus
  // *Preprint, echo=NO, model=NO, history=NO, contact=NO

  WriteHeaderFile(outFile);

  fprintf(outFile,"**\n");

  // *Node
  //    1,         -1.0,          1.0,         -1.0    
  WriteNodesFile(outFile);


  //*Element, type=C3D10
  //    1,      1,      2,      3,      4,      5,      6,      7,      8,      9,     10
  WriteElementsFile(outFile);
  
  fprintf(outFile,"**\n");

  // Materials
  
  //*Material, name=Mat_1
  //*Elastic
  //6.759396380901535, 0.35
  
  WriteMaterialsFile(outFile);
	
  fclose(outFile);

  cppDEL(m_ProgressHelper);

  return ALBA_OK;
}

//---------------------------------------------------------------------------
int albaOpExporterAbaqusFile::WriteHeaderFile(FILE *file)
{
  wxString jobName="";
  wxString modelName="";

  wxString header = "*Heading\n** Job name: " + jobName + " Model name: " + modelName + "\n";
  header += "** Generated by: Bonemat\n";
  header += "*Preprint, echo=NO, model=NO, history=NO, contact=NO\n";

  fprintf(file, header); 

  return ALBA_OK;
}
//---------------------------------------------------------------------------
int albaOpExporterAbaqusFile::WriteNodesFile(FILE *file)
{
  albaVMEMesh *input = albaVMEMesh::SafeDownCast(m_Input);
  assert(input);

  vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();

  vtkIntArray *nodesIDArray = input->GetNodesIDArray();

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
  int columnsNumber;
  int rowsNumber = inputUGrid->GetNumberOfPoints();
  columnsNumber = 4; // point ID + point coordinates

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

  int pointIDColumn = 0;
  double pointCoordinates[3] = {-9999, -9999, -9999};
  int maxNodeId = nodesIDArray->GetRange()[1];

  char printStr[100];
  m_IntCharSize = sprintf(printStr,"%d",maxNodeId);

  fprintf(file,"*Node");
   
  sprintf(printStr,"\n\t%%%dd,\t %%21.13E,\t %%21.13E,\t %%21.13E",m_IntCharSize);

  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    float nodProgress = rowID / m_TotalElements;
    m_ProgressHelper->UpdateProgressBar(nodProgress * 100);

    pointsToBeExported->GetPoint(rowID, pointCoordinates);
  
    fprintf(file,printStr, nodesIDArray->GetValue(rowID), pointCoordinates[0], pointCoordinates[1], pointCoordinates[2]);
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
int albaOpExporterAbaqusFile::WriteElementsFile(FILE *file)
{
  albaVMEMesh *input = albaVMEMesh::SafeDownCast(m_Input);
  assert(input);

  vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();

  // create elements matrix 
  int rowsNumber = inputUGrid->GetNumberOfCells();

  // read all the elements with their attribute data in memory (vnl_matrix)

  // get the ELEMENT_ID array
  vtkIntArray *elementIdArray = input->GetElementsIDArray();

  // get the Nodes Id array
  vtkIntArray *nodesIDArray = input->GetNodesIDArray();

  // get the TYPE array
  vtkIntArray *typeArray = input->GetElementsTypeArray();

  // get the REAL array
  vtkIntArray *realArray = input->GetElementsRealArray();

  ExportElement *exportVector = new ExportElement[rowsNumber];

  
  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    exportVector[rowID].elementID = elementIdArray ? elementIdArray->GetValue(rowID) : rowID+1;
    exportVector[rowID].matID = GetMatIdArray() ? GetMatIdArray()[rowID] : 1;
    exportVector[rowID].elementType = typeArray ? typeArray->GetValue(rowID) : 1;
    exportVector[rowID].elementReal = realArray ? realArray->GetValue(rowID) : 1;
    exportVector[rowID].cellID=rowID;

    // Create Elset
    if(m_MatIDMap.find(exportVector[rowID].matID) == m_MatIDMap.end())
    {
      ExportElset elset;
      elset.matID = exportVector[rowID].matID;
      elset.elementsIDVect.push_back(exportVector[rowID].elementID);
      m_Elsets.push_back(elset);

      m_MatIDMap[elset.matID]= m_Elsets.size()-1;
    }
    else
    {
      int elsetIndex = m_MatIDMap.find(exportVector[rowID].matID)->second;

      m_Elsets[elsetIndex].elementsIDVect.push_back(exportVector[rowID].elementID);
      m_Elsets[elsetIndex].matID = exportVector[rowID].matID;
    }
  }

  //qsort(exportVector, rowsNumber, sizeof(ExportElement), compareElem);
	int currCellNpoints = -1;


  // Write Elements
  char printStr[100];
  sprintf(printStr," %%%dd %%%dd %%%dd %%%dd %%%dd %%%dd %%%dd %%%dd %%%dd %%%dd %%%dd",m_IntCharSize,m_IntCharSize,m_IntCharSize,m_IntCharSize,m_IntCharSize,m_IntCharSize,m_IntCharSize,m_IntCharSize,m_IntCharSize,m_IntCharSize,m_IntCharSize);

  char printStr2[10];
  sprintf(printStr2,"\t%%%dd",m_IntCharSize);

  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    float elemProgress = (m_CurrentProgress + (float)rowID) / m_TotalElements;
    m_ProgressHelper->UpdateProgressBar(elemProgress * 100);

    vtkCell *currentCell = inputUGrid->GetCell(exportVector[rowID].cellID);
    vtkIdList *idList = currentCell->GetPointIds();
    int cellNpoints=currentCell->GetNumberOfPoints();

		if (currCellNpoints != cellNpoints)
		{
			fprintf(file, "\n*Element, type=C3D%d\n", cellNpoints);
			currCellNpoints = cellNpoints;
		}
		    
    fprintf(file,"\t%d, ", exportVector[rowID].elementID);
    
    for (int currentID = 0; currentID < cellNpoints; currentID++)
    {
      fprintf(file, printStr2, nodesIDArray->GetValue(idList->GetId(currentID)));

      if(currentID < cellNpoints-1)
        fprintf(file, ","); 
    }

    fprintf(file, "\n"); 
  }

  // Write Elsets

  for(int i=0; i< m_Elsets.size();i++)
  {
    fprintf(file,"*Elset, elset=Set_%d\n",(i+1));
    
    for (int j=0; j<m_Elsets[i].elementsIDVect.size(); j++)
    {
      fprintf(file,"%d", m_Elsets[i].elementsIDVect[j]);

     if(j<m_Elsets[i].elementsIDVect.size()-1)
        fprintf(file,", ", m_Elsets[i].elementsIDVect[j]);

	 if ((j != 0) && ((j+1) % 16) == 0)
		 fprintf(file, "\n");
    }

    fprintf(file,"\n*Solid Section, elset=Set_%d, material=Mat_%d\n",(i+1), m_Elsets[i].matID);
  }

  delete [] exportVector;

  return ALBA_OK;
}
//---------------------------------------------------------------------------
int albaOpExporterAbaqusFile::WriteMaterialsFile(FILE *file)
{
  albaVMEMesh *input = albaVMEMesh::SafeDownCast(m_Input);
  assert(input);

  vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();

	vtkFieldData *materialData = GetMaterialData();

  vtkDataArray *materialsIDArray = NULL;

  // try field data
  materialsIDArray = materialData->GetArray("material_id");

  if (materialsIDArray != NULL)
  {
		albaLogMessage("Found material array in field data");

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

    // For each material
    for (int i = 0; i < numberOfMaterials; i++)
    {
      float matProgress = (m_CurrentProgress + i) / m_TotalElements;
      m_ProgressHelper->UpdateProgressBar(matProgress * 100);

      int materialID = materialsIDArray->GetTuple(i)[0];
      double EX, NUXY;
			bool exFound = false, nuxyFound = false;

      // For each property
      for (int j = 0; j < numberOfMaterialProperties; j++)
      {
        wxString arrayName = materialProperties[j];
        vtkDataArray *array = materialData->GetArray(arrayName.c_str());

				if (arrayName.compare("EX") == 0)
				{
					EX = array->GetTuple(i)[0];
					exFound = true;
				}

				if (arrayName.compare("NUXY") == 0)
				{
					NUXY = array->GetTuple(i)[0];
					nuxyFound = true;
				}
      }

			if (exFound && nuxyFound)
			{
				fprintf(file, "*Material, name=Mat_%d\n", materialID);
				fprintf(file, "*Elastic\n");
				fprintf(file, "%.8lf, %.8lf", EX, NUXY);
				fprintf(file, "\n");
			}
    }  

    m_CurrentProgress += numberOfMaterials;
  }  

  return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaOpExporterAbaqusFile::LoadConfigurationTags()
{
	m_HasConfiguration = false;

	if (m_Input && m_Input->GetTagArray()->IsTagPresent("BMT_CONFIG_TAG"))
	{
		//---------------------RhoQCTFromHU-----------------
		/*rho = a + b * HU*/
		m_Configuration.rhoIntercept = GetDoubleTag("rhoIntercept");
		m_Configuration.rhoSlope = GetDoubleTag("rhoSlope");

		//three intervals rho calibration
		m_Configuration.a_RhoLessThanRho1 = GetDoubleTag("a_RhoLessThanRho1");
		m_Configuration.b_RhoLessThanAsh1 = GetDoubleTag("b_RhoLessThanAsh1");
		m_Configuration.c_RhoLessThanRho1 = GetDoubleTag("c_RhoLessThanRho1");

		m_Configuration.a_RhoBetweenRho1andRho2 = GetDoubleTag("a_RhoBetweenRho1andRho2");
		m_Configuration.b_RhoBetweenRho1andRho2 = GetDoubleTag("b_RhoBetweenRho1andRho2");
		m_Configuration.c_RhoBetweenRho1andRho2 = GetDoubleTag("c_RhoBetweenRho1andRho2");

		m_Configuration.a_RhoBiggerThanRho2 = GetDoubleTag("a_RhoBiggerThanRho2");
		m_Configuration.b_RhoBiggerThanRho2 = GetDoubleTag("b_RhoBiggerThanRho2");
		m_Configuration.c_RhoBiggerThanRho2 = GetDoubleTag("b_RhoBiggerThanRho2");

		m_Configuration.m_IntegrationSteps = GetDoubleTag("m_IntegrationSteps");
		m_Configuration.rho1 = m_Configuration.rho2 = GetDoubleTag("rho2");

		m_Configuration.densityIntervalsNumber = GetDoubleTag("densityIntervalsNumber"); //appOpBonematCommon::SINGLE_INTERVAL;

		m_Configuration.a_OneInterval = GetDoubleTag("a_OneInterval");
		m_Configuration.b_OneInterval = GetDoubleTag("b_OneInterval");
		m_Configuration.c_OneInterval = GetDoubleTag("c_OneInterval");

		m_Configuration.m_YoungModuleCalculationModality = GetDoubleTag("m_YoungModuleCalculationModality"); //appOpBonematCommon::HU_INTEGRATION;

																																																				 //Rho Calibration Flag
		m_Configuration.rhoCalibrationCorrectionIsActive = GetDoubleTag("rhoCalibrationCorrectionIsActive");
		m_Configuration.rhoCalibrationCorrectionType = GetDoubleTag("rhoCalibrationCorrectionType"); //equals to single interval

		m_Configuration.rhoQCT1 = GetDoubleTag("rhoQCT1");
		m_Configuration.rhoQCT2 = GetDoubleTag("rhoQCT2");

		//single interval rho calibration
		m_Configuration.a_CalibrationCorrection = GetDoubleTag("a_CalibrationCorrection");
		m_Configuration.b_CalibrationCorrection = GetDoubleTag("b_CalibrationCorrection");

		//three intervals rho calibration
		m_Configuration.a_RhoQCTLessThanRhoQCT1 = GetDoubleTag("a_RhoQCTLessThanRhoQCT1");
		m_Configuration.b_RhoQCTLessThanRhoQCT1 = GetDoubleTag("b_RhoQCTLessThanRhoQCT1");

		m_Configuration.a_RhoQCTBetweenRhoQCT1AndRhoQCT2 = GetDoubleTag("a_RhoQCTBetweenRhoQCT1AndRhoQCT2");
		m_Configuration.b_RhoQCTBetweenRhoQCT1AndRhoQCT2 = GetDoubleTag("b_RhoQCTBetweenRhoQCT1AndRhoQCT2");

		m_Configuration.a_RhoQCTBiggerThanRhoQCT2 = GetDoubleTag("a_RhoQCTBiggerThanRhoQCT2");
		m_Configuration.b_RhoQCTBiggerThanRhoQCT2 = GetDoubleTag("b_RhoQCTBiggerThanRhoQCT2");

		m_Configuration.rhoWetConversionIsActive = GetDoubleTag("rhoWetConversionIsActive");
		m_Configuration.a_rhoWet = GetDoubleTag("a_rhoWet");

		// Advanced Configuration
		m_Configuration.m_DensityOutput = GetDoubleTag("m_DensityOutput"); //appOpBonematCommon::RhoSelection::USE_RHO_QCT;
		m_Configuration.m_PoissonRatio = GetDoubleTag("m_PoissonRatio");
		m_Configuration.minElasticity = GetDoubleTag("minElasticity"); // 1e-6;

		m_HasConfiguration = true;
	}
}
//----------------------------------------------------------------------------
double albaOpExporterAbaqusFile::GetDoubleTag(wxString tagName)
{
	if (m_Input->GetTagArray()->IsTagPresent("bmtConf_" + tagName))
	{
		albaTagItem *tagItem = m_Input->GetTagArray()->GetTag("bmtConf_" + tagName);

		return tagItem->GetValueAsDouble();
	}

	return -1;
}
