/*=========================================================================

Program: ALBA
Module: albaOpExporterFEMCommon.cpp
Authors: Nicola Vanella, Gianluigi Crimi

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

#include "albaOpExporterFEMCommon.h"
#include "albaOpImporterAnsysCommon.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaVMEMesh.h"

// vtk includes
#include "vtkUnstructuredGrid.h"
#include "vtkFieldData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"

#include "wx/stdpaths.h"
#include "albaVME.h"
#include "albaTagArray.h"
#include "albaGUIRollOut.h"

#define DEFAULT_POISSON 0.3

//----------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaOpExporterFEMCommon);

//----------------------------------------------------------------------------
albaOpExporterFEMCommon::albaOpExporterFEMCommon(const wxString &label) :
albaOp(label)
{
	m_OpType = OPTYPE_EXPORTER;
	m_Canundo = true;

	m_MaterialData = NULL;
	m_MatIdArray = NULL;
	m_Egap = 50;

	m_ABSMatrixFlag = 1;
	m_EnableBackCalculation = 1;

	memset(&m_Configuration, 0, sizeof(BonematConfiguration));
	
	m_Configuration.rho2 = 1; 
	m_Configuration.b_OneInterval = m_Configuration.b_RhoLessThanRho1 = m_Configuration.b_RhoBetweenRho1andRho2 = m_Configuration.b_RhoBiggerThanRho2 = 1;
	m_Configuration.c_OneInterval = m_Configuration.c_RhoLessThanRho1 = m_Configuration.c_RhoBetweenRho1andRho2 = m_Configuration.c_RhoBiggerThanRho2 = 1;
}
//----------------------------------------------------------------------------
albaOpExporterFEMCommon::~albaOpExporterFEMCommon()
{	
	vtkDEL(m_MaterialData);
	delete[] m_MatIdArray;
}

//----------------------------------------------------------------------------
void albaOpExporterFEMCommon::CreateGui()
{
	m_Gui = new albaGUI(this);

	m_Gui->Label("Absolute matrix", true);
	m_Gui->Bool(ID_ABS_MATRIX, "Apply", &m_ABSMatrixFlag, 0);
	m_Gui->Divider(1);

	bool hasMaterials = HasMaterials();

	m_Gui->Label(_("FEM Exporter Properties"), true);

	if (hasMaterials)
	{
		m_Gui->Label("Elasticity Gap value");
		m_Gui->Double(ID_GAP_VALUE, "", &m_Egap);
		m_Gui->Divider();
		m_Gui->Label("");
	}

	m_Gui->Divider(2);

	m_HasConfiguration=albaVMEMesh::LoadConfigurationTags((albaVMEMesh *)m_Input, m_Configuration);

	if (hasMaterials)
	{
		m_Gui->Label("Density Back Calculation", true);
		m_Gui->Bool(ID_ENABLE_BACKCALCULATION, "Enable", &m_EnableBackCalculation);
		m_Gui->Divider();
		m_Gui->Divider();

		m_BackPropGui = new albaGUI(this);

		//////////////////////////////////////////////////////////////////////////
		// Density - Elasticity
		m_BackPropGui->Divider();
		m_BackPropGui->Divider();
		m_BackPropGui->Divider();
		m_BackPropGui->Label("Density-elasticity relationship", true);
		m_BackPropGui->Label("E = a + b * Rho^c", false);
		m_BackPropGui->Divider();
		m_BackPropGui->Divider();

		const wxString densityChoices[] = { "Single interval", "Three intervals" };
		m_BackPropGui->Combo(ID_RHO_DENSITY_INTERVALS_NUMBER, "", &m_Configuration.densityIntervalsNumber, 2, densityChoices);

		m_GuiASDensityOneInterval = new albaGUI(this);

		m_GuiASDensityOneInterval->Double(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_0, "a", &m_Configuration.a_OneInterval);
		m_GuiASDensityOneInterval->Double(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_1, "b", &m_Configuration.b_OneInterval);
		m_GuiASDensityOneInterval->Double(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_2, "c", &m_Configuration.c_OneInterval);

		m_GuiRollOutDensityOneInterval = m_BackPropGui->RollOut(ID_DENSITY_ONE_INTERVAL_ROLLOUT, _("Single interval"), m_GuiASDensityOneInterval);

		m_BackPropGui->Divider();

		m_GuiASDensityThreeIntervals = new albaGUI(this);

		m_GuiASDensityThreeIntervals->Double(ID_DENSITY_INTERVAL_0, "Rho1", &m_Configuration.rho1);
		m_GuiASDensityThreeIntervals->Double(ID_DENSITY_INTERVAL_1, "Rho2", &m_Configuration.rho2);

		m_GuiASDensityThreeIntervals->Label("Rho < Rho1");
		m_GuiASDensityThreeIntervals->Double(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, "a", &m_Configuration.a_RhoLessThanRho1);
		m_GuiASDensityThreeIntervals->Double(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, "b", &m_Configuration.b_RhoLessThanRho1);
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

		m_GuiRollOutDensityThreeIntervals = m_BackPropGui->RollOut(ID_DENSITY_THREE_INTERVALS_ROLLOUT, _("Three intervals"), m_GuiASDensityThreeIntervals);

		m_BackPropRollOut = m_Gui->RollOut(ID_BACK_PROPAGATION_ROLLOUT, _("Back Calculation Settings"), m_BackPropGui);

		if (m_HasConfiguration)
			m_BackPropRollOut->RollOut(false);
		UpdateGui();
	}

	AddSpecificGui();
	
	m_Gui->Label("");
	m_Gui->OkCancel();
	m_Gui->Divider();

	ShowGui();

}

//----------------------------------------------------------------------------
void albaOpExporterFEMCommon::UpdateGui()
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
			

	m_GuiRollOutDensityOneInterval->RollOut(m_Configuration.densityIntervalsNumber == SINGLE_INTERVAL && enable);
	m_Gui->Enable(ID_DENSITY_ONE_INTERVAL_ROLLOUT, m_Configuration.densityIntervalsNumber == SINGLE_INTERVAL && enable);

	m_GuiRollOutDensityThreeIntervals->RollOut(m_Configuration.densityIntervalsNumber == THREE_INTERVALS && enable);
	m_Gui->Enable(ID_DENSITY_THREE_INTERVALS_ROLLOUT, m_Configuration.densityIntervalsNumber == THREE_INTERVALS && enable);

	m_GuiASDensityOneInterval->Update();
	m_GuiASDensityThreeIntervals->Update();

	m_Gui->FitGui();
	m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaOpExporterFEMCommon::OpRun()
{
	Init();
	CreateGui();
}
//---------------------------------------------------------------------------
void albaOpExporterFEMCommon::Init()
{
	albaVMEMesh *input = albaVMEMesh::SafeDownCast(m_Input);
	assert(input);

	vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();


	if (inputUGrid != NULL)
	{
		// Calculate Num of Elements
		m_TotalElements = inputUGrid->GetNumberOfPoints(); // Points

		m_TotalElements += inputUGrid->GetNumberOfCells(); // Elements

		vtkDataArray *materialsIDArray = NULL;
		materialsIDArray = inputUGrid->GetCellData()->GetArray("EX");

		if (materialsIDArray != NULL)
		{
			m_TotalElements += materialsIDArray->GetNumberOfTuples(); // Materials
		}
	}
}
//----------------------------------------------------------------------------
void albaOpExporterFEMCommon::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
			case wxOK:
			{
				OnOK();
				OpStop(OP_RUN_OK);
			}
			break;
			case wxCANCEL:
			{
				OpStop(OP_RUN_CANCEL);
			}
			break;
			case ID_ENABLE_BACKCALCULATION:
			case ID_RHO_DENSITY_INTERVALS_NUMBER:
			{
				UpdateGui();
			}
			case ID_DENSITY_ONE_INTERVAL_ROLLOUT:
			case ID_DENSITY_THREE_INTERVALS_ROLLOUT:
				m_Gui->FitGui();
			break;
			default:
				albaEventMacro(*e);
			break;
		}
	}
}

//----------------------------------------------------------------------------
bool albaOpExporterFEMCommon::HasMaterials()
{
	bool hasMaterials = false;

	albaVMEMesh *input = albaVMEMesh::SafeDownCast(m_Input);
	assert(input);

	vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();

	if (inputUGrid != NULL)
	{
		hasMaterials = inputUGrid->GetCellData()->GetArray("EX") != NULL;
	}

	return hasMaterials;
}

//----------------------------------------------------------------------------
bool albaOpExporterFEMCommon::InternalAccept(albaVME *node)
{
	return (node->IsA("albaVMEMesh"));
}

//----------------------------------------------------------------------------
vtkIdType * albaOpExporterFEMCommon::GetMatIdArray()
{
	if (m_MatIdArray == NULL)
		GetMaterialData();

	return m_MatIdArray;
}

//----------------------------------------------------------------------------
void albaOpExporterFEMCommon::CreateBins(int numElements, MaterialProp *elProps, std::vector<MaterialProp> *materialProperties)
{
	//Sorting elements
	qsort(elProps, numElements, sizeof(MaterialProp), compareE);

	//accumulator and stats for first material
	vtkIdType numMats = 1;
	double E = elProps[0].ex;
	
	m_MatIdArray = new vtkIdType[numElements];
	if (numElements > 0)
		m_MatIdArray[elProps[0].elementID] = numMats;

	// grouping materials according to E value
	for (int id = 1; id < numElements; id++)
	{
		// generate statistics for the group
		if (id == (numElements - 1) || E - elProps[id].ex > m_Egap)
		{
			materialProperties->push_back(elProps[id-1]);
			int matId = numMats - 1;

			if (m_EnableBackCalculation)
				(*materialProperties)[matId].density = DensityBackCalculation(elProps[id - 1].ex);
			//else 
			//  the density is already contained in the materialProperties struct

			//update E value and increase matNum
			if (id < (numElements - 1))
			{
				numMats++;
				E = elProps[id].ex;
			}
		}
		m_MatIdArray[elProps[id].elementID] = numMats;
	}
}

//----------------------------------------------------------------------------
int albaOpExporterFEMCommon::compareE(const void *p1, const void *p2)
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
vtkFieldData* albaOpExporterFEMCommon::CreateMaterialsData(std::vector <MaterialProp>materialProperties)
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
			else 
				continue;
		}

		// Add the i-th data array to the field data
		fdata->AddArray(darr);

		// Clean up
		darr->Delete();
	}

	return fdata;
}

//---------------------------------------------------------------------------
vtkFieldData *albaOpExporterFEMCommon::GetMaterialData()
{
	albaVMEMesh *input = albaVMEMesh::SafeDownCast(m_Input);
	assert(input);

	vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();

	if (m_MaterialData == NULL)
	{
		int numElements = inputUGrid->GetNumberOfCells();

		vtkDataArray *arrayE = inputUGrid->GetCellData()->GetArray("EX");
		//if (arrayE == NULL) numElements = 1;

		MaterialProp *elProps;
		elProps = new MaterialProp[numElements];

		vtkDataArray *arrayMaterial = inputUGrid->GetCellData()->GetArray("Material");
		vtkDataArray *arrayPoisson = inputUGrid->GetCellData()->GetArray("NUXY");
		vtkDataArray *arrayDens = inputUGrid->GetCellData()->GetArray("DENS");

		for (int i = 0; i < numElements; i++)
		{
			elProps[i].elementID = i;
			elProps[i].density = arrayDens ? arrayDens->GetTuple(i)[0] : 0;
			elProps[i].nuxy = arrayPoisson ? arrayPoisson->GetTuple(i)[0] : DEFAULT_POISSON;
			elProps[i].ex = arrayE ? arrayE->GetTuple(i)[0] : 0;
		}

		std::vector <MaterialProp> materialProperties;

		CreateBins(numElements, elProps, &materialProperties);

		// Create MaterialsData
		m_MaterialData = CreateMaterialsData(materialProperties);

		materialProperties.clear();

		delete[] elProps;
	}

	return m_MaterialData;
}

//----------------------------------------------------------------------------
double albaOpExporterFEMCommon::DensityBackCalculation(double elasticity)
{
	if (m_Configuration.densityIntervalsNumber == SINGLE_INTERVAL)
	{
		return pow(((elasticity - m_Configuration.a_OneInterval) / m_Configuration.b_OneInterval), 1.0 / m_Configuration.c_OneInterval);
	}
	else
	{
		double e1 = m_Configuration.a_RhoLessThanRho1 + m_Configuration.b_RhoLessThanRho1 * pow(m_Configuration.rho1, m_Configuration.c_RhoLessThanRho1);
		double e2 = m_Configuration.a_RhoBetweenRho1andRho2 + m_Configuration.b_RhoBetweenRho1andRho2 * pow(m_Configuration.rho2, m_Configuration.c_RhoBetweenRho1andRho2);

		if (elasticity < e1)
			return pow(((elasticity - m_Configuration.a_RhoLessThanRho1) / m_Configuration.b_RhoLessThanRho1), 1.0 / m_Configuration.c_RhoLessThanRho1);
		else if (elasticity < e2)
			return pow(((elasticity - m_Configuration.a_RhoBetweenRho1andRho2) / m_Configuration.b_RhoBetweenRho1andRho2), 1.0 / m_Configuration.c_RhoBetweenRho1andRho2);
		else 
			return pow(((elasticity - m_Configuration.a_RhoBiggerThanRho2) / m_Configuration.b_RhoBiggerThanRho2), 1.0 / m_Configuration.c_RhoBiggerThanRho2);
	}
}

