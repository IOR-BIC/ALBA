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

	m_Freq_fp = NULL;
	m_FrequencyFileName = "";

	// Advanced Configuration
	m_DensitySelection = USE_MEAN_DENSISTY;
	
	m_ABSMatrixFlag = 1;
	m_EnableBackCalculation = 1;
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

	if (hasMaterials)
	{
		m_Gui->Label(_("FEM Exporter Properties"), true);

		// Frequency
		m_Gui->Label("Output Frequency file: ");

		albaString wildc = "Frequency File (*.*)|*.*";
		m_Gui->FileSave(ID_FREQUENCY_FILE_NAME, "Freq file", &m_FrequencyFileName, wildc.GetCStr());
		m_Gui->Divider();

		m_Gui->Label("Elasticity Gap value");
		m_Gui->Double(ID_GAP_VALUE, "", &m_Egap);
		m_Gui->Divider();

		const wxString choices[] = { "Mean", "Maximum" };
		m_Gui->Label("Grouping Density");
		m_Gui->Combo(ID_DENSITY_SELECTION, "", &m_DensitySelection, 2, choices);

		m_Gui->Label("");
	}

	m_Gui->Divider(2);

	LoadConfigurationTags();

	if (hasMaterials && m_HasConfiguration)
	{
		m_Gui->Label("Back Calculation", true);
		m_Gui->Bool(ID_ENABLE_BACKCALCULATION, "Enable", &m_EnableBackCalculation);
		m_Gui->Label("");

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

		m_GuiRollOutDensityThreeIntervals = m_Gui->RollOut(ID_DENSITY_THREE_INTERVALS_ROLLOUT, _("Three intervals"), m_GuiASDensityThreeIntervals);

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
	m_Gui->Enable(ID_DENSITY_SELECTION, !enable);
			

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
void albaOpExporterFEMCommon::SetDefaultFrequencyFile()
{
	wxStandardPaths std_paths;
	wxString userPath = std_paths.GetUserDataDir();
	m_FrequencyFileName = userPath;
	m_FrequencyFileName += "\\";
	m_FrequencyFileName += m_Input->GetName();
	m_FrequencyFileName += "-Freq.txt";
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
bool albaOpExporterFEMCommon::Accept(albaVME *node)
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
	// COMPUTE MATERIALS & WRITE FREQUENCY FILE
	if (!m_FrequencyFileName.IsEmpty() && (m_Freq_fp = fopen(m_FrequencyFileName.GetCStr(), "w")) == NULL && GetTestMode() == false)
		albaErrorMessage("Frequency file can't be opened");
	
	//Sorting elements
	qsort(elProps, numElements, sizeof(MaterialProp), compareE);

	//Writing freq file intestation
	if (m_Freq_fp != NULL)
		fprintf(m_Freq_fp, "rho \t\t E \t\t NUMBER OF ELEMENTS\n\n");

	//accumulator and stats for first material
	vtkIdType freq = 1;
	vtkIdType numMats = 1;
	double densAccumulator = elProps[0].density;
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
			else if (m_DensitySelection == USE_MEAN_DENSISTY)
				(*materialProperties)[matId].density = densAccumulator / freq;
			//else 
			//  the max density is already contained in the materialProperties struct

			// print statistics
			if (m_Freq_fp != NULL)
				fprintf(m_Freq_fp, "%f \t %f \t %d\n", (*materialProperties)[matId].density, (*materialProperties)[matId].ex, freq);

			//accumulator and stats for next material
			if (id < (numElements - 1))
			{
				freq = 1;
				numMats++;
				densAccumulator = elProps[id].density;
				E = elProps[id].ex;
			}
		}
		else
		{
			densAccumulator += elProps[id].density;
			freq++;
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
void albaOpExporterFEMCommon::LoadConfigurationTags()
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
		m_Configuration.b_RhoLessThanRho1 = GetDoubleTag("b_RhoLessThanRho1");
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
double albaOpExporterFEMCommon::GetDoubleTag(wxString tagName)
{
	if (m_Input->GetTagArray()->IsTagPresent("bmtConf_" + tagName))
	{
		albaTagItem *tagItem = m_Input->GetTagArray()->GetTag("bmtConf_" + tagName);

		return tagItem->GetValueAsDouble();
	}

	return -1;
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
			return pow(((elasticity - m_Configuration.a_RhoBetweenRho1andRho2) / m_Configuration.a_RhoBetweenRho1andRho2), 1.0 / m_Configuration.c_RhoBetweenRho1andRho2);
		else 
			return pow(((elasticity - m_Configuration.a_RhoBiggerThanRho2) / m_Configuration.b_RhoBiggerThanRho2), 1.0 / m_Configuration.c_RhoBiggerThanRho2);
	}
}

