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

#ifndef __albaVMEMesh_h
#define __albaVMEMesh_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEGeneric.h"
#include "albaVMEOutputMesh.h"
#include "vtkIntArray.h"
#include "albaGUIRollOut.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkUnstructuredGrid;
class wxBitmap;
class mmaMaterial;

enum
{
	SINGLE_INTERVAL = 0,
	THREE_INTERVALS = 1,
};

enum
{
	HU_INTEGRATION = 0,
	YOUNG_MODULE_INTEGRATION = 1,
	INTEGRATION_MODALITIES_NUMBER,
};

enum RhoSelection
{
	USE_RHO_QCT,
	USE_RHO_ASH,
	USE_RHO_WET,
};

typedef struct configurationStruct
{
	//---------------------RhoQCTFromHU-----------------
	/*rho = a + b * HU*/
	double rhoIntercept;
	double rhoSlope;

	//---------------------RhoQCTFromHU-----------------
	//rho calibration
	int rhoCalibrationCorrectionIsActive;
	int rhoCalibrationCorrectionType;

	//---------------------RhoASHFromRhoQCT-----------------
	//rho interval
	double rhoQCT1, rhoQCT2;
	//single interval rho calibration
	double a_CalibrationCorrection, b_CalibrationCorrection;
	//three intervals rho calibration
	double a_RhoQCTLessThanRhoQCT1, b_RhoQCTLessThanRhoQCT1;
	double a_RhoQCTBetweenRhoQCT1AndRhoQCT2, b_RhoQCTBetweenRhoQCT1AndRhoQCT2;
	double a_RhoQCTBiggerThanRhoQCT2, b_RhoQCTBiggerThanRhoQCT2;

	//---------------------RhoWetFromRhoAsh-----------------
	int rhoWetConversionIsActive;
	double a_rhoWet;

	//---------------YoungModuleFromRho-----------------
	int densityIntervalsNumber;
	double rho1, rho2;
	//single interval rho calibration
	double a_OneInterval, b_OneInterval, c_OneInterval;
	//three intervals rho calibration
	double a_RhoLessThanRho1, b_RhoLessThanRho1, c_RhoLessThanRho1;
	double a_RhoBetweenRho1andRho2, b_RhoBetweenRho1andRho2, c_RhoBetweenRho1andRho2;
	double a_RhoBiggerThanRho2, b_RhoBiggerThanRho2, c_RhoBiggerThanRho2;

	double elasticityBounds[2];
	

	//-------------Integration Parameters---------------
	int m_IntegrationSteps;
	int m_YoungModuleCalculationModality;

	//---------------Advanced Parameters----------------
	int m_DensityOutput;
	double m_PoissonRatio;

} BonematConfiguration;


/** albaVMEMesh - A vme that can accept vtkUnstructuredGrid 
dataset only. it's used to store finite element analysis
data*/
class ALBA_EXPORT albaVMEMesh : public albaVMEGeneric
{
public:
	enum 
	{
		ID_NONE = MINID,
		ID_DISABLED,
		ID_BONEMAT_CONFIG_ROLLOUT,
		ID_SAVE,
	};

	albaTypeMacro(albaVMEMesh, albaVMEGeneric);

  /** Set data for the give timestamp. 
  This function automatically creates a VMEItem for the data to be stored. 
  The optional parameter specify if data must be copied, referenced, or detached from original source. 
  Default is to copy data which creates a new data set copying the original one.
  Referencing means the data is kept connected to its original source, and
  detaching means disconnect is like referencing but also disconnect the data
  from the source.
  Return ALBA_OK if succeeded, ALBA_ERROR if they kind of data is not accepted by
  this type of VME. Accepted data is vtkUnstructuredGrid */
  virtual int SetData(vtkUnstructuredGrid *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);
  virtual int SetData(vtkDataSet *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);

  /** return the output casted to albaVMEOutputMesh*/
  albaVMEOutputMesh *GetUnstructuredGridOutput() {return albaVMEOutputMesh::SafeDownCast(GetOutput());}

  /** return the Node IDs Array*/
  vtkIntArray *GetNodesIDArray();
	/** return the Node IDs Array*/
	static vtkIntArray *GetNodesIDArray(vtkUnstructuredGrid *inputUGrid);
			
  /** return the Element IDs Array*/
  vtkIntArray *GetElementsIDArray();
	/** return the Element IDs Array*/
	static vtkIntArray *GetElementsIDArray(vtkUnstructuredGrid *inputUGrid);

	/** return the Element Types Array*/
	vtkIntArray *GetElementsTypeArray();
	/** return the Element Types Array*/
	static vtkIntArray *GetElementsTypeArray(vtkUnstructuredGrid *inputUGrid);

	/** return the Element Reals Array*/
	vtkIntArray *GetElementsRealArray();
	/** return the Element Reals Array*/
	static vtkIntArray *GetElementsRealArray(vtkUnstructuredGrid *inputUGrid);

	/** Load configuration tags from a mesh vme and stores it to conf var, returns false if tags are not present */
	static bool LoadConfigurationTags(albaVMEMesh *vme, BonematConfiguration &conf);

	/** Get a double value from a specific tag */
	static double GetDoubleTag(albaVME *vme, wxString tagName);

	/** save the configuration to a file in xml format*/
	static int SaveConfigurationFile(BonematConfiguration configuration, const char *configurationFileName);

	/** return the output */
  virtual albaVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeMesh");};

	/** used to initialize and create the material attribute if not yet present */
	virtual int InternalInitialize();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

	/** Precess events coming from other objects */
	void OnEvent(albaEventBase *alba_event);
	
protected:

	albaVMEMesh();
  virtual ~albaVMEMesh();

	/** Create GUI for the VME */
	virtual albaGUI *CreateGui();


	/** Return the array with the name given. Returns NULL is array not found */
	vtkIntArray *GetIntCellArray(const char *arrayName, const char *arrayName2);

	/** Return the array with the name given. Returns NULL is array not found */
	static vtkIntArray *GetIntCellArray(vtkUnstructuredGrid *inputUGrid, const char *arrayName, const char *arrayName2);

	BonematConfiguration m_Configuration;

	// GUI elements
	albaGUIRollOut *m_GuiRollOutBonematConfig;
	albaGUI *m_GuiBonematConfig;

	albaString  m_NumCells;
	albaString  m_NumNodes;

private:
  albaVMEMesh(const albaVMEMesh&); // Not implemented
  void operator=(const albaVMEMesh&); // Not implemented
  
};
#endif
