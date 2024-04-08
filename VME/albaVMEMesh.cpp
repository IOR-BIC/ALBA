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
#include "albaGUI.h"
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
#include "albaTagArray.h"

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include "albaXMLString.h"
#include "albaEvent.h"
#include "albaPics.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEMesh)

//-------------------------------------------------------------------------
albaVMEMesh::albaVMEMesh()
{
	m_NumNodes = m_NumCells = "0";
}
//-------------------------------------------------------------------------
albaVMEMesh::~albaVMEMesh()
{
}
//----------------------------------------------------------------------------
albaGUI * albaVMEMesh::CreateGui()
{
	Superclass::CreateGui();

	if (this->GetUnstructuredGridOutput() && this->GetUnstructuredGridOutput()->GetVTKData())
	{
		this->Update();
		m_NumCells = this->GetUnstructuredGridOutput()->GetVTKData()->GetNumberOfCells();
		m_NumNodes = this->GetUnstructuredGridOutput()->GetVTKData()->GetNumberOfPoints();
	}

	m_Gui->Label(_("Elem: "), &m_NumCells, true);
	m_Gui->Label(_("Nodes: "), &m_NumNodes, true);
	
	bool hasConfiguration = LoadConfigurationTags(this, m_Configuration);
	
	if (hasConfiguration)
	{
		m_Gui->Divider(1);
		//m_Gui->Label("Bonemat configuration parameters");

		m_GuiBonematConfig = new albaGUI(this);
		//////////////////////////////////////////////////////////////////////////
		// Save Config
 		//wxBitmap bitmap = albaPictureFactory::GetPictureFactory()->GetBmp("FILE_SAVE");
		//m_GuiBonematConfig->ImageButton(ID_SAVE, "Save Configuration", bitmap, "Save Configuration");
		m_GuiBonematConfig->Button(ID_SAVE, "Save Configuration");
		m_GuiBonematConfig->Divider(2);

		//////////////////////////////////////////////////////////////////////////
		m_GuiBonematConfig->Label("CT densitometric calibration", true);
		m_GuiBonematConfig->Label("RhoQCT = a + b * HU", false);

		m_GuiBonematConfig->Double(ID_DISABLED, "a", &m_Configuration.rhoIntercept);
		m_GuiBonematConfig->Double(ID_DISABLED, "b", &m_Configuration.rhoSlope);
		m_GuiBonematConfig->Divider(2);

		//////////////////////////////////////////////////////////////////////////
		// Calibration
		if (m_Configuration.rhoCalibrationCorrectionIsActive)
		{
			m_GuiBonematConfig->Label("Correction of the calibration", true);
			m_GuiBonematConfig->Label("RhoAsh = a + b * RhoQCT", false);

			m_GuiBonematConfig->Divider();

			if (m_Configuration.rhoCalibrationCorrectionType == 0) // SINGLE_INTERVAL = 0,	THREE_INTERVALS = 1
			{
				m_GuiBonematConfig->Label("Single interval");

				m_GuiBonematConfig->Double(ID_DISABLED, "a", &m_Configuration.a_CalibrationCorrection);
				m_GuiBonematConfig->Double(ID_DISABLED, "b", &m_Configuration.b_CalibrationCorrection);
			}
			else
			{
				m_GuiBonematConfig->Label("Three intervals");

				m_GuiBonematConfig->Double(ID_DISABLED, "RhoQCT1", &m_Configuration.rhoQCT1);
				m_GuiBonematConfig->Double(ID_DISABLED, "RhoQCT2", &m_Configuration.rhoQCT2);

				m_GuiBonematConfig->Divider();

				m_GuiBonematConfig->Label("RhoQCT < RhoQCT1");
				m_GuiBonematConfig->Double(ID_DISABLED, "a", &m_Configuration.a_RhoQCTLessThanRhoQCT1);
				m_GuiBonematConfig->Double(ID_DISABLED, "b", &m_Configuration.b_RhoQCTLessThanRhoQCT1);

				m_GuiBonematConfig->Label("RhoQCT1 <= RhoQCT <= RhoQCT2");
				m_GuiBonematConfig->Double(ID_DISABLED, "a", &m_Configuration.a_RhoQCTBetweenRhoQCT1AndRhoQCT2);
				m_GuiBonematConfig->Double(ID_DISABLED, "b", &m_Configuration.b_RhoQCTBetweenRhoQCT1AndRhoQCT2);

				m_GuiBonematConfig->Label("RhoQCT > RhoQCT2");
				m_GuiBonematConfig->Double(ID_DISABLED, "a", &m_Configuration.a_RhoQCTBiggerThanRhoQCT2);
				m_GuiBonematConfig->Double(ID_DISABLED, "b", &m_Configuration.b_RhoQCTBiggerThanRhoQCT2);
			}

			m_GuiBonematConfig->Divider(2);
		}

		//////////////////////////////////////////////////////////////////////////
		// RhoAsh - RhoWet
		if (m_Configuration.rhoWetConversionIsActive)
		{
			m_GuiBonematConfig->Label("RhoAsh -> RhoWet Conversion", true);
			m_GuiBonematConfig->Label("RhoAsh = a * RhoWet", false);
			m_GuiBonematConfig->Double(ID_DISABLED, "a", &m_Configuration.a_rhoWet);
			m_GuiBonematConfig->Divider(2);
		}

		//////////////////////////////////////////////////////////////////////////
		// Density - Elasticity
		m_GuiBonematConfig->Label("Density-elasticity relationship", true);
		m_GuiBonematConfig->Label("E = a + b * Rho^c", false);

		//m_GuiBonematConfig->Label("Minimum Elasticity Modulus:", false);
		m_GuiBonematConfig->Double(ID_DISABLED, "Min Elasticity Modulus:", &m_Configuration.elasticityBounds[0], MINDOUBLE, MAXDOUBLE, -1, "", false, 0.60);
		m_GuiBonematConfig->Double(ID_DISABLED, "Max Elasticity Modulus:", &m_Configuration.elasticityBounds[1], MINDOUBLE, MAXDOUBLE, -1, "", false, 0.60);
		m_GuiBonematConfig->Divider();

		if (m_Configuration.densityIntervalsNumber == 0) // SINGLE_INTERVAL = 0,	THREE_INTERVALS = 1
		{
			m_GuiBonematConfig->Label("Single interval");
			m_GuiBonematConfig->Double(ID_DISABLED, "a", &m_Configuration.a_OneInterval);
			m_GuiBonematConfig->Double(ID_DISABLED, "b", &m_Configuration.b_OneInterval);
			m_GuiBonematConfig->Double(ID_DISABLED, "c", &m_Configuration.c_OneInterval);
		}
		else
		{
			m_GuiBonematConfig->Label("Three intervals");

			m_GuiBonematConfig->Double(ID_DISABLED, "Rho1", &m_Configuration.rho1);
			m_GuiBonematConfig->Double(ID_DISABLED, "Rho2", &m_Configuration.rho2);

			m_GuiBonematConfig->Label("Rho < Rho1");
			m_GuiBonematConfig->Double(ID_DISABLED, "a", &m_Configuration.a_RhoLessThanRho1);
			m_GuiBonematConfig->Double(ID_DISABLED, "b", &m_Configuration.b_RhoLessThanRho1);
			m_GuiBonematConfig->Double(ID_DISABLED, "c", &m_Configuration.c_RhoLessThanRho1);

			m_GuiBonematConfig->Label("Rho1 <= Rho <= Rho2");
			m_GuiBonematConfig->Double(ID_DISABLED, "a", &m_Configuration.a_RhoBetweenRho1andRho2);
			m_GuiBonematConfig->Double(ID_DISABLED, "b", &m_Configuration.b_RhoBetweenRho1andRho2);
			m_GuiBonematConfig->Double(ID_DISABLED, "c", &m_Configuration.c_RhoBetweenRho1andRho2);

			m_GuiBonematConfig->Label("Rho > Rho2");
			m_GuiBonematConfig->Double(ID_DISABLED, "a", &m_Configuration.a_RhoBiggerThanRho2);
			m_GuiBonematConfig->Double(ID_DISABLED, "b", &m_Configuration.b_RhoBiggerThanRho2);
			m_GuiBonematConfig->Double(ID_DISABLED, "c", &m_Configuration.c_RhoBiggerThanRho2);
		}

		m_GuiBonematConfig->Divider(2);

		//////////////////////////////////////////////////////////////////////////
		// Young's modulus
		m_GuiBonematConfig->Divider();
		const wxString choices[] = { "HU integration", "E integration" };
		m_GuiBonematConfig->Label("Bonemat integration parameter", "", TRUE);
		m_GuiBonematConfig->Combo(ID_DISABLED, "", &m_Configuration.m_YoungModuleCalculationModality, 2, choices);
		//m_GuiBonematConfig->Label("Integration steps:");
		m_GuiBonematConfig->Integer(ID_DISABLED, "Integration steps:", &m_Configuration.m_IntegrationSteps, MININT, MAXINT, "", false, 0.50);

		m_GuiBonematConfig->Divider(2);

		//////////////////////////////////////////////////////////////////////////
		// Advanced 
		m_GuiBonematConfig->Label("Advanced Configuration", TRUE);
		const wxString choices3[] = { "rhoQCT", "rhoAsh", "rhoWet" };

		m_GuiBonematConfig->Bool(ID_DISABLED, "Apply E bounds on Integration:", &m_Configuration.m_ElasticityBoundsOnInteg, 1);
	
		//m_GuiBonematConfig->Label("Density Output:");
		m_GuiBonematConfig->Combo(ID_DISABLED, "Density Output:", &m_Configuration.m_DensityOutput, 3, choices3, "", 0.45);
		//m_GuiBonematConfig->Label("Poisson's Ratio:");
		m_GuiBonematConfig->Double(ID_DISABLED, "Poisson's Ratio:", &m_Configuration.m_PoissonRatio, MINDOUBLE, MAXDOUBLE, -1, "", false, 0.45);

		//////////////////////////////////////////////////////////////////////////

		m_GuiBonematConfig->Enable(ID_DISABLED, false);
		m_GuiRollOutBonematConfig = m_Gui->RollOut(ID_BONEMAT_CONFIG_ROLLOUT, _("Bonemat config parameters"), m_GuiBonematConfig);
		m_GuiRollOutBonematConfig->RollOut(false);
	}

	m_Gui->FitGui();
	m_Gui->Update();

	return m_Gui;
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

	if (unstructuredGrid)
	{
		m_NumCells = unstructuredGrid->GetNumberOfCells();
		m_NumNodes = unstructuredGrid->GetNumberOfPoints();
		return Superclass::SetData(unstructuredGrid, t, mode);
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
		m_NumCells = unstructuredGrid->GetNumberOfCells();
		m_NumNodes = unstructuredGrid->GetNumberOfPoints();
    return Superclass::SetData(data,t,mode);
  }
  else
  {
    albaErrorMacro("Trying to set the wrong type of data inside a VME Mesh :"<< (data?data->GetClassName():"NULL"));
		m_NumNodes = m_NumCells = "0";
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

//----------------------------------------------------------------------------
void albaVMEMesh::OnEvent(albaEventBase *alba_event)
{
	albaEvent *e = albaEvent::SafeDownCast(alba_event);

	if (e && e->GetSender() == m_Gui)
	{
		if (e->GetId() == ID_SAVE)
		{
			albaString initialFileName;
			initialFileName = albaGetDocumentsDirectory();
			initialFileName.Append("\\newConfigurationFile.xml");

			albaString wildc = "configuration xml file (*.xml)|*.xml";
			albaString newFileName = albaGetSaveFile(initialFileName.GetCStr(), wildc);

			if (newFileName != "")
				SaveConfigurationFile(m_Configuration, newFileName);
			return;
		}
		else if (e->GetId() == ID_BONEMAT_CONFIG_ROLLOUT)
		{
			// Call Update and Fit Gui for VMEPanel in SideBar
			GetLogicManager()->VmeModified(this);
			return;
		}
	}
	
	Superclass::OnEvent(alba_event);	
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

//----------------------------------------------------------------------------
bool albaVMEMesh::LoadConfigurationTags(albaVMEMesh *vme, BonematConfiguration &conf)
{
	if (vme && vme->GetTagArray()->IsTagPresent("BMT_CONFIG_TAG"))
	{
		//---------------------RhoQCTFromHU-----------------
		/*rho = a + b * HU*/
		conf.rhoIntercept = GetDoubleTag(vme, "rhoIntercept");
		conf.rhoSlope = GetDoubleTag(vme, "rhoSlope");

		//three intervals rho calibration
		conf.a_RhoLessThanRho1 = GetDoubleTag(vme, "a_RhoLessThanRho1");
		conf.b_RhoLessThanRho1 = GetDoubleTag(vme, "b_RhoLessThanRho1");
		conf.c_RhoLessThanRho1 = GetDoubleTag(vme, "c_RhoLessThanRho1");

		conf.a_RhoBetweenRho1andRho2 = GetDoubleTag(vme, "a_RhoBetweenRho1andRho2");
		conf.b_RhoBetweenRho1andRho2 = GetDoubleTag(vme, "b_RhoBetweenRho1andRho2");
		conf.c_RhoBetweenRho1andRho2 = GetDoubleTag(vme, "c_RhoBetweenRho1andRho2");

		conf.a_RhoBiggerThanRho2 = GetDoubleTag(vme, "a_RhoBiggerThanRho2");
		conf.b_RhoBiggerThanRho2 = GetDoubleTag(vme, "b_RhoBiggerThanRho2");
		conf.c_RhoBiggerThanRho2 = GetDoubleTag(vme, "c_RhoBiggerThanRho2");

		conf.m_IntegrationSteps = GetDoubleTag(vme, "m_IntegrationSteps");
		conf.rho1 = GetDoubleTag(vme, "rho1");
		conf.rho2 = GetDoubleTag(vme, "rho2");

		conf.densityIntervalsNumber = GetDoubleTag(vme, "densityIntervalsNumber"); //appOpBonematCommon::SINGLE_INTERVAL;

		conf.a_OneInterval = GetDoubleTag(vme, "a_OneInterval");
		conf.b_OneInterval = GetDoubleTag(vme, "b_OneInterval");
		conf.c_OneInterval = GetDoubleTag(vme, "c_OneInterval");

		conf.m_YoungModuleCalculationModality = GetDoubleTag(vme, "m_YoungModuleCalculationModality"); //appOpBonematCommon::HU_INTEGRATION;

																																																	 //Rho Calibration Flag
		conf.rhoCalibrationCorrectionIsActive = GetDoubleTag(vme, "rhoCalibrationCorrectionIsActive");
		conf.rhoCalibrationCorrectionType = GetDoubleTag(vme, "rhoCalibrationCorrectionType"); //equals to single interval

		conf.rhoQCT1 = GetDoubleTag(vme, "rhoQCT1");
		conf.rhoQCT2 = GetDoubleTag(vme, "rhoQCT2");

		//single interval rho calibration
		conf.a_CalibrationCorrection = GetDoubleTag(vme, "a_CalibrationCorrection");
		conf.b_CalibrationCorrection = GetDoubleTag(vme, "b_CalibrationCorrection");

		//three intervals rho calibration
		conf.a_RhoQCTLessThanRhoQCT1 = GetDoubleTag(vme, "a_RhoQCTLessThanRhoQCT1");
		conf.b_RhoQCTLessThanRhoQCT1 = GetDoubleTag(vme, "b_RhoQCTLessThanRhoQCT1");

		conf.a_RhoQCTBetweenRhoQCT1AndRhoQCT2 = GetDoubleTag(vme, "a_RhoQCTBetweenRhoQCT1AndRhoQCT2");
		conf.b_RhoQCTBetweenRhoQCT1AndRhoQCT2 = GetDoubleTag(vme, "b_RhoQCTBetweenRhoQCT1AndRhoQCT2");

		conf.a_RhoQCTBiggerThanRhoQCT2 = GetDoubleTag(vme, "a_RhoQCTBiggerThanRhoQCT2");
		conf.b_RhoQCTBiggerThanRhoQCT2 = GetDoubleTag(vme, "b_RhoQCTBiggerThanRhoQCT2");

		conf.rhoWetConversionIsActive = GetDoubleTag(vme, "rhoWetConversionIsActive");
		conf.a_rhoWet = GetDoubleTag(vme, "a_rhoWet");

		// Advanced Configuration
		conf.m_DensityOutput = GetDoubleTag(vme, "m_DensityOutput"); //appOpBonematCommon::RhoSelection::USE_RHO_QCT;
		conf.m_PoissonRatio = GetDoubleTag(vme, "m_PoissonRatio");
		conf.m_ElasticityBoundsOnInteg = GetDoubleTag(vme, "m_ElasticityBoundsOnInteg");
		conf.elasticityBounds[0] = GetDoubleTag(vme, "minElasticity");
		conf.elasticityBounds[1] = GetDoubleTag(vme, "maxElasticity");

		return true;
	}
	else
		return false;
}
//----------------------------------------------------------------------------
double albaVMEMesh::GetDoubleTag(albaVME *vme, wxString tagName)
{
	if (vme->GetTagArray()->IsTagPresent("bmtConf_" + tagName))
	{
		albaTagItem *tagItem = vme->GetTagArray()->GetTag("bmtConf_" + tagName);

		return tagItem->GetValueAsDouble();
	}

	return -1;
}

//---------------------------------------------------------------------------
int albaVMEMesh::SaveConfigurationFile(BonematConfiguration configuration, const char *configurationFileName)
{
	//Open the file xml
	try
	{
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
	{
		// Do your failure processing here
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
	XMLCh tempStr[100];
	XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("LS", tempStr, 99);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *impl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(tempStr);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* theSerializer = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)impl)->createDOMWriter();
	theSerializer->setNewLine(albaXMLString("\r"));

	if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true))
		theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);

	doc = impl->createDocument(NULL, albaXMLString("CONFIGURATION"), NULL);

	doc->setEncoding(albaXMLString("UTF-8"));
	doc->setStandalone(true);
	doc->setVersion(albaXMLString("1.0"));

	// extract root element and wrap it with an albaXMLElement object
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
	assert(root);

	// attach version attribute to the root node
	root->setAttribute(albaXMLString("Version"), albaXMLString(albaString(2)));

	// CT_DENSITOMETRIC_CALIBRATION
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *densitometricCalibrationNode = doc->createElement(albaXMLString("CT_DENSITOMETRIC_CALIBRATION"));
	densitometricCalibrationNode->setAttribute(albaXMLString("ROIntercept"), albaXMLString(albaString(configuration.rhoIntercept)));
	densitometricCalibrationNode->setAttribute(albaXMLString("ROSlope"), albaXMLString(albaString(configuration.rhoSlope)));
	if (configuration.rhoCalibrationCorrectionIsActive)
	{
		densitometricCalibrationNode->setAttribute(albaXMLString("ROCalibrationCorrectionIsActive"), albaXMLString("true"));
	}
	else
	{
		densitometricCalibrationNode->setAttribute(albaXMLString("ROCalibrationCorrectionIsActive"), albaXMLString("false"));
	}
	root->appendChild(densitometricCalibrationNode);

	// CORRECTION_OF_CALIBRATION
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *correctionCalibrationNode = doc->createElement(albaXMLString("CORRECTION_OF_CALIBRATION"));

	if (configuration.rhoCalibrationCorrectionType == SINGLE_INTERVAL)
	{
		correctionCalibrationNode->setAttribute(albaXMLString("IntervalsType"), albaXMLString("SINGLE"));
	}
	else
	{
		correctionCalibrationNode->setAttribute(albaXMLString("IntervalsType"), albaXMLString("THREE"));
	}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *calibrationSingleIntervalNode = doc->createElement(albaXMLString("CALIBRATION_SINGLE_INTERVAL"));
	calibrationSingleIntervalNode->setAttribute(albaXMLString("a"), albaXMLString(albaString(configuration.a_CalibrationCorrection)));
	calibrationSingleIntervalNode->setAttribute(albaXMLString("b"), albaXMLString(albaString(configuration.b_CalibrationCorrection)));
	correctionCalibrationNode->appendChild(calibrationSingleIntervalNode);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *calibrationLimitsNode = doc->createElement(albaXMLString("CALIBRATION_LIMITS"));
	calibrationLimitsNode->setAttribute(albaXMLString("RhoQCT1"), albaXMLString(albaString(configuration.rhoQCT1)));
	calibrationLimitsNode->setAttribute(albaXMLString("RhoQCT2"), albaXMLString(albaString(configuration.rhoQCT2)));
	correctionCalibrationNode->appendChild(calibrationLimitsNode);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *calibrationInterval1Node = doc->createElement(albaXMLString("CALIBRATION_INTERVAL_1"));
	calibrationInterval1Node->setAttribute(albaXMLString("a"), albaXMLString(albaString(configuration.a_RhoQCTLessThanRhoQCT1)));
	calibrationInterval1Node->setAttribute(albaXMLString("b"), albaXMLString(albaString(configuration.b_RhoQCTLessThanRhoQCT1)));
	correctionCalibrationNode->appendChild(calibrationInterval1Node);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *calibrationInterval2Node = doc->createElement(albaXMLString("CALIBRATION_INTERVAL_2"));
	calibrationInterval2Node->setAttribute(albaXMLString("a"), albaXMLString(albaString(configuration.a_RhoQCTBetweenRhoQCT1AndRhoQCT2)));
	calibrationInterval2Node->setAttribute(albaXMLString("b"), albaXMLString(albaString(configuration.b_RhoQCTBetweenRhoQCT1AndRhoQCT2)));
	correctionCalibrationNode->appendChild(calibrationInterval2Node);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *calibrationInterval3Node = doc->createElement(albaXMLString("CALIBRATION_INTERVAL_3"));
	calibrationInterval3Node->setAttribute(albaXMLString("a"), albaXMLString(albaString(configuration.a_RhoQCTBiggerThanRhoQCT2)));
	calibrationInterval3Node->setAttribute(albaXMLString("b"), albaXMLString(albaString(configuration.b_RhoQCTBiggerThanRhoQCT2)));
	correctionCalibrationNode->appendChild(calibrationInterval3Node);

	root->appendChild(correctionCalibrationNode);

	// CT_DENSITOMETRIC_CALIBRATION
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *rhoWetConverionNode = doc->createElement(albaXMLString("RO_WET_CONVERSION"));
	rhoWetConverionNode->setAttribute(albaXMLString("a"), albaXMLString(albaString(configuration.a_rhoWet)));
	if (configuration.rhoWetConversionIsActive)
	{
		rhoWetConverionNode->setAttribute(albaXMLString("ROWetConversionIsActive"), albaXMLString("true"));
	}
	else
	{
		rhoWetConverionNode->setAttribute(albaXMLString("ROWetConversionIsActive"), albaXMLString("false"));
	}
	root->appendChild(rhoWetConverionNode);


	// DENSITY_ELASTICITY_RELATIONSHIP
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *densityRelationshipNode = doc->createElement(albaXMLString("DENSITY_ELASTICITY_RELATIONSHIP"));

	if (configuration.densityIntervalsNumber == SINGLE_INTERVAL)
	{
		densityRelationshipNode->setAttribute(albaXMLString("IntervalsType"), albaXMLString("SINGLE"));
	}
	else
	{
		densityRelationshipNode->setAttribute(albaXMLString("IntervalsType"), albaXMLString("THREE"));
	}

	densityRelationshipNode->setAttribute(albaXMLString("MinElasticity"), albaXMLString(albaString(configuration.elasticityBounds[0])));
	densityRelationshipNode->setAttribute(albaXMLString("MaxElasticity"), albaXMLString(albaString(configuration.elasticityBounds[1])));

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *densitySingleIntervalNode = doc->createElement(albaXMLString("DENSITY_SINGLE_INTERVAL"));
	densitySingleIntervalNode->setAttribute(albaXMLString("a"), albaXMLString(albaString(configuration.a_OneInterval)));
	densitySingleIntervalNode->setAttribute(albaXMLString("b"), albaXMLString(albaString(configuration.b_OneInterval)));
	densitySingleIntervalNode->setAttribute(albaXMLString("c"), albaXMLString(albaString(configuration.c_OneInterval)));
	densityRelationshipNode->appendChild(densitySingleIntervalNode);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *densityLimitsNode = doc->createElement(albaXMLString("DENSITY_LIMITS"));
	densityLimitsNode->setAttribute(albaXMLString("Rho1"), albaXMLString(albaString(configuration.rho1)));
	densityLimitsNode->setAttribute(albaXMLString("Rho2"), albaXMLString(albaString(configuration.rho2)));
	densityRelationshipNode->appendChild(densityLimitsNode);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *densityInterval1Node = doc->createElement(albaXMLString("DENSITY_INTERVAL_1"));
	densityInterval1Node->setAttribute(albaXMLString("a"), albaXMLString(albaString(configuration.a_RhoLessThanRho1)));
	densityInterval1Node->setAttribute(albaXMLString("b"), albaXMLString(albaString(configuration.b_RhoLessThanRho1)));
	densityInterval1Node->setAttribute(albaXMLString("c"), albaXMLString(albaString(configuration.c_RhoLessThanRho1)));
	densityRelationshipNode->appendChild(densityInterval1Node);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *densityInterval2Node = doc->createElement(albaXMLString("DENSITY_INTERVAL_2"));
	densityInterval2Node->setAttribute(albaXMLString("a"), albaXMLString(albaString(configuration.a_RhoBetweenRho1andRho2)));
	densityInterval2Node->setAttribute(albaXMLString("b"), albaXMLString(albaString(configuration.b_RhoBetweenRho1andRho2)));
	densityInterval2Node->setAttribute(albaXMLString("c"), albaXMLString(albaString(configuration.c_RhoBetweenRho1andRho2)));
	densityRelationshipNode->appendChild(densityInterval2Node);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *densityInterval3Node = doc->createElement(albaXMLString("DENSITY_INTERVAL_3"));
	densityInterval3Node->setAttribute(albaXMLString("a"), albaXMLString(albaString(configuration.a_RhoBiggerThanRho2)));
	densityInterval3Node->setAttribute(albaXMLString("b"), albaXMLString(albaString(configuration.b_RhoBiggerThanRho2)));
	densityInterval3Node->setAttribute(albaXMLString("c"), albaXMLString(albaString(configuration.c_RhoBiggerThanRho2)));
	densityRelationshipNode->appendChild(densityInterval3Node);

	root->appendChild(densityRelationshipNode);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *youngModuleNode = doc->createElement(albaXMLString("YOUNGMODULE"));
	if (configuration.m_YoungModuleCalculationModality == HU_INTEGRATION)
		youngModuleNode->setAttribute(albaXMLString("CalculationModality"), albaXMLString("HU"));
	else
		youngModuleNode->setAttribute(albaXMLString("CalculationModality"), albaXMLString("E"));

	youngModuleNode->setAttribute(albaXMLString("StepsNumber"), albaXMLString(albaString(configuration.m_IntegrationSteps)));
	root->appendChild(youngModuleNode);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *advancedConfig = doc->createElement(albaXMLString("ADVANCED"));

	if (configuration.m_DensityOutput == USE_RHO_QCT)
	{
		advancedConfig->setAttribute(albaXMLString("RhoUsage"), albaXMLString("rhoQCT"));
	}
	else if (configuration.m_DensityOutput == USE_RHO_ASH)
	{
		advancedConfig->setAttribute(albaXMLString("RhoUsage"), albaXMLString("rhoAsh"));
	}
	else if (configuration.m_DensityOutput == USE_RHO_WET)
	{
		advancedConfig->setAttribute(albaXMLString("RhoUsage"), albaXMLString("rhoWet"));
	}

	advancedConfig->setAttribute(albaXMLString("PoissonRatio"), albaXMLString(albaString(configuration.m_PoissonRatio)));

	if (configuration.m_ElasticityBoundsOnInteg)
	{
		advancedConfig->setAttribute(albaXMLString("ElasticityBoundsOnIntegration"), albaXMLString("true"));
	}
	else
	{
		advancedConfig->setAttribute(albaXMLString("ElasticityBoundsOnIntegration"), albaXMLString("false"));
	}

	root->appendChild(advancedConfig);
	// 

	XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget *XMLTarget;
	albaString fileName = configurationFileName;

	XMLTarget = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget(fileName);

	try
	{
		// do the serialization through DOMWriter::writeNode();
		theSerializer->writeNode(XMLTarget, *doc);
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER  XMLException& toCatch)
	{
		char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.getMessage());
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
		return ALBA_ERROR;
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch)
	{
		char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.msg);
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
		return ALBA_ERROR;
	}
	catch (...) {
		return ALBA_ERROR;
	}

	theSerializer->release();
	cppDEL(XMLTarget);
	doc->release();

	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	albaLogMessage(albaString::Format("New configuration file has been written %s", fileName.GetCStr()));

	return ALBA_OK;
}