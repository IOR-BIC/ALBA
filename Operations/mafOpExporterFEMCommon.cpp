/*=========================================================================

Program: MAF2
Module: mafOpExporterFEMCommon.cpp
Authors: Nicola Vanella

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

#include "mafOpExporterFEMCommon.h"
#include "mafOpImporterAnsysCommon.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafVMEMesh.h"

// vtk includes
#include "vtkUnstructuredGrid.h"
#include "vtkFieldData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"

#include "wx/stdpaths.h"

#define DEFAULT_POISSON 0.3

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExporterFEMCommon);

//----------------------------------------------------------------------------
mafOpExporterFEMCommon::mafOpExporterFEMCommon(const wxString &label) :
mafOp(label)
{
	m_MaterialFieldData = NULL;
	m_MatIdArray = NULL;

	m_Egap = 50;

	m_Freq_fp = NULL;
	m_FrequencyFileName = "";

	// Advanced Configuration
	m_DensitySelection = USE_MEAN_DENSISTY;
}
//----------------------------------------------------------------------------
mafOpExporterFEMCommon::~mafOpExporterFEMCommon()
{	
	vtkDEL(m_MaterialFieldData);
	delete[] m_MatIdArray;
}

//----------------------------------------------------------------------------
void mafOpExporterFEMCommon::CreateGui()
{
	m_Gui = new mafGUI(this);

	if (HasMaterials())
	{
		m_Gui->Label(_("FEM Exporter Properties"), true);

		// Frequency
		m_Gui->Label("Output Frequency file: ");

		wxStandardPaths std_paths;
		wxString userPath = std_paths.GetUserDataDir();
		mafString wildc = "Frequency File (*.*)|*.*";
		m_FrequencyFileName = userPath;
		m_FrequencyFileName += "\\";
		m_FrequencyFileName += m_Input->GetName();
		m_FrequencyFileName += "-Freq.txt";

		m_Gui->FileSave(ID_FREQUENCY_FILE_NAME, "Freq file", &m_FrequencyFileName, wildc.GetCStr());

		m_Gui->Label("");
		const wxString choices[] = { "Mean", "Maximum" };
		m_Gui->Label("Grouping Density");
		m_Gui->Combo(ID_DENSITY_SELECTION, "", &m_DensitySelection, 2, choices);

		m_Gui->Label("");
		m_Gui->Label("Elasticity Gap value");
		m_Gui->Double(ID_GAP_VALUE, "", &m_Egap);

		m_Gui->Divider();
		m_Gui->Label("");
	}
}

//----------------------------------------------------------------------------
bool mafOpExporterFEMCommon::HasMaterials()
{
	bool hasMaterials = false;

	mafVMEMesh *input = mafVMEMesh::SafeDownCast(m_Input);
	assert(input);

	vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();

	if (inputUGrid != NULL)
	{
		hasMaterials = inputUGrid->GetCellData()->GetArray("EX") != NULL;
	}

	return hasMaterials;
}

//----------------------------------------------------------------------------
vtkIdType * mafOpExporterFEMCommon::GetMatIdArray()
{
	if (m_MatIdArray == NULL)
		GetMaterialData();

	return m_MatIdArray;
}

//----------------------------------------------------------------------------
void mafOpExporterFEMCommon::GenerateArraysAndMaterialsData(vtkIdType numElements, MaterialProp *elProps, vtkUnstructuredGrid *inputUG)
{
	if (!m_FrequencyFileName.IsEmpty() &&(m_Freq_fp = fopen(m_FrequencyFileName.GetCStr(), "w")) == NULL)
	{
		if (GetTestMode() == false)
		{
			wxMessageBox("Frequency file can't be opened");
		}
	}

	std::vector <MaterialProp> materialProperties;
	
	CreateBins(numElements, elProps, &materialProperties);

	// FieldData materials
	m_MaterialFieldData = CreateMaterialsData(materialProperties);
	
	materialProperties.clear();
}

//----------------------------------------------------------------------------
void mafOpExporterFEMCommon::CreateBins(int numElements, MaterialProp *elProps, std::vector<MaterialProp> *materialProperties)
{
	typedef std::vector<int> idVectorType;
	idVectorType idVector;
	double densAccumulator, nuxyAccumulator;

	m_MatIdArray = new vtkIdType[numElements];

	// COMPUTE MATERIALS & WRITE FREQUENCY FILE
	qsort(elProps, numElements, sizeof(MaterialProp), compareE);

	mafLogMessage("-- Writing frequency file\n");

	if (m_Freq_fp != NULL)
		fprintf(m_Freq_fp, "rho \t\t E \t\t NUMBER OF ELEMENTS\n\n");

	vtkIdType freq = 1;
	vtkIdType numMats = 1;
	materialProperties->push_back(elProps[0]);

	double E = elProps[0].ex;
	double dens = densAccumulator = elProps[0].density;
	double nuxy = nuxyAccumulator = elProps[0].nuxy;

	// grouping materials according to E value
	for (int id = 1; id < numElements; id++)
	{
		if (E - elProps[id].ex > m_Egap) // generate statistics for old group and create a new group
		{
			if (m_DensitySelection == USE_MEAN_DENSISTY)
			{
				dens = (*materialProperties)[numMats - 1].density = densAccumulator / freq;
				nuxy = (*materialProperties)[numMats - 1].nuxy = nuxyAccumulator / freq;
			}

			// print statistics
			if (m_Freq_fp != NULL)
				fprintf(m_Freq_fp, "%f \t %f \t %d\n", dens, E, freq);

			dens = densAccumulator = elProps[id].density;
			nuxy = nuxyAccumulator = elProps[id].nuxy;

			materialProperties->push_back(elProps[id]);

			E = elProps[id].ex;
			numMats++;
			freq = 1;
		}
		else
		{
			densAccumulator += elProps[id].density;
			nuxyAccumulator += elProps[id].nuxy;
			freq++;
		}
		m_MatIdArray[elProps[id].elementID] = numMats;
	}

	if (m_DensitySelection == USE_MEAN_DENSISTY)
	{
		dens = (*materialProperties)[numMats - 1].density = densAccumulator / freq;
		nuxy = (*materialProperties)[numMats - 1].nuxy = nuxyAccumulator / freq;
	}

	// Print statistics
	if (m_Freq_fp != NULL)
	{
		fprintf(m_Freq_fp, "%f \t %f \t %d\n", dens, E, freq);
		fclose(m_Freq_fp);
	}
}

//----------------------------------------------------------------------------
int mafOpExporterFEMCommon::compareE(const void *p1, const void *p2)
//----------------------------------------------------------------------------
{
	double result;
	// decreasing order 
	result = ((MaterialProp *)p2)->ex - ((MaterialProp *)p1)->ex;
	if (result < 0)
		return -1;
	if (result > 0)
		return 1;
	return 0;
}

//----------------------------------------------------------------------------
vtkFieldData* mafOpExporterFEMCommon::CreateMaterialsData(std::vector <MaterialProp>materialProperties)
{
	vtkIdType numMats = materialProperties.size();
	char *vectorNames[3] = { "EX","NUXY","DENS" };

	vtkFieldData * fdata = vtkFieldData::New();

	vtkDoubleArray *iarr = vtkDoubleArray::New();
	iarr->SetName("material_id");
	iarr->SetNumberOfValues(numMats);

	for (int j = 0; j < numMats; j++)
		iarr->InsertValue(j, j + 1);

	// Add the i-th data array to the field data
	fdata->AddArray(iarr);
	//Clean up
	iarr->Delete();

	// Create field data data array
	for (int i = 0; i < 3; i++)
	{
		// Create the i-th data array
		vtkDoubleArray *darr = vtkDoubleArray::New();
		darr->SetName(vectorNames[i]);
		darr->SetNumberOfValues(numMats);

		for (int j = 0; j < numMats; j++)
		{
			// fill i-th data array with j-th value 
			// cycle on materials
			if (i == EX)
				darr->InsertValue(j, materialProperties[j].ex);
			else if (i == NUXY)
				darr->InsertValue(j, materialProperties[j].nuxy);
			else if (i == DENS)
				darr->InsertValue(j, materialProperties[j].density);
		}

		// Add the i-th data array to the field data
		fdata->AddArray(darr);

		// Clean up
		darr->Delete();
	}

	return fdata;
}

//---------------------------------------------------------------------------
vtkFieldData *mafOpExporterFEMCommon::GetMaterialData()
{
	mafVMEMesh *input = mafVMEMesh::SafeDownCast(m_Input);
	assert(input);

	vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();

	if (m_MaterialFieldData == NULL)
	{
		int numElements = inputUGrid->GetNumberOfCells();

		vtkDataArray *arrayE = inputUGrid->GetCellData()->GetArray("EX");
		if (arrayE == NULL) numElements = 1;

		MaterialProp *elProps;
		elProps = new MaterialProp[numElements];

		vtkDataArray *arrayMaterial = inputUGrid->GetCellData()->GetArray("Material");
		vtkDataArray *arrayPoisson = inputUGrid->GetCellData()->GetArray("NUXY");
		vtkDataArray *arrayDens = inputUGrid->GetCellData()->GetArray("DENS");


		for (int i=0; i<numElements; i++)
		{
			elProps[i].elementID = i;
			elProps[i].density = arrayDens ? arrayDens->GetTuple(i)[0] : 0;
			elProps[i].nuxy = arrayPoisson ? arrayPoisson->GetTuple(i)[0] : DEFAULT_POISSON;
			elProps[i].ex = arrayE ? arrayE->GetTuple(i)[0] : 0;
		}

		GenerateArraysAndMaterialsData(numElements, elProps, inputUGrid);

		delete[] elProps;
	}

	return m_MaterialFieldData;
}


