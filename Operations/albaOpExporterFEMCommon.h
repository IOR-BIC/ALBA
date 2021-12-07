/*=========================================================================

Program: ALBA
Module: albaOpExporterFEMCommon.h
Authors: Nicola Vanella, Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterFEMCommon_H__
#define __albaOpExporterFEMCommon_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaVMEMesh.h"

#include "vtkUnstructuredGrid.h"
#include "vtkIntArray.h"
#include "albaGUI.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaEvent;
class albaProgressBarHelper;

// Element Properties: element ID, density, Young Module
typedef struct
{
	vtkIdType elementID;
	double ex;
	double nuxy;
	double density;
} MaterialProp;

enum MyEnum
{
	EX,
	NUXY,
	DENS
};

//----------------------------------------------------------------------------
// albaOpExporterFEMCommon :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpExporterFEMCommon : public albaOp
{
public:

	albaAbstractTypeMacro(albaOpExporterFEMCommon, albaOp);

	enum EXPORTER_FEM_ID
	{
		ID_FIRST = MINID,
		ID_GAP_VALUE,
		ID_POISSON_RATIO,
		ID_ABS_MATRIX,
		ID_ENABLE_BACKCALCULATION,
		ID_MIN_ELASTICITY,
		ID_RHO_DENSITY_INTERVALS_NUMBER,
		ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_0,
		ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_1,
		ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_2,
		ID_DENSITY_ONE_INTERVAL_ROLLOUT,
		ID_DENSITY_INTERVAL_0,
		ID_DENSITY_INTERVAL_1,
		ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0,
		ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1,
		ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2,
		ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0,
		ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1,
		ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2,
		ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0,
		ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1,
		ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2,
		ID_DENSITY_THREE_INTERVALS_ROLLOUT,
		ID_BACK_PROPAGATION_ROLLOUT,
		MINID
	};
			
	albaOpExporterFEMCommon(const wxString &label = "albaOpExporterFEMCommon");
	~albaOpExporterFEMCommon();

	/** Apply vme abs matrix to data geometry */
	void ApplyABSMatrixOn() { m_ABSMatrixFlag = 1; };
	void ApplyABSMatrixOff() { m_ABSMatrixFlag = 0; };
	void SetApplyABSMatrix(int apply_matrix) { m_ABSMatrixFlag = apply_matrix; };
	
	vtkIdType * GetMatIdArray();

	vtkFieldData *GetMaterialData();

	//----------------------------------------------------------------------------
	virtual void OnEvent(albaEventBase *alba_event);
	
	/** Returns EnableBackCalculation */
	int GetEnableBackCalculation() const { return m_EnableBackCalculation; }

	/** Sets EnableBackCalculation */
	void SetEnableBackCalculation(int enableBackCalculation) { m_EnableBackCalculation = enableBackCalculation; }
	
	/** Returns Configuration */
	BonematConfiguration GetConfiguration() const { return m_Configuration; }

	/** Sets Configuration */
	void SetConfiguration(BonematConfiguration configuration) { m_Configuration = configuration; }

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	virtual void OnOK() = 0;
	
	/** Create the dialog interface for the exporter. */
	virtual void CreateGui();

	void UpdateGui();

	bool  HasMaterials();

	/** Creates material Bins By grouping element props, internally sorts elProps vector */
	void CreateBins(int numElements, MaterialProp *elProps, std::vector<MaterialProp> *materialProperties);

	/** Generates Arrays And MaterialsData from element properties vector */
	vtkFieldData* CreateMaterialsData(std::vector <MaterialProp>materialProperties);

	/** Static function witch compares two ElementProps used for qsort in decreasing order */
	static int compareE(const void *p1, const void *p2);

	/** Calculates the density from the value of elasticity */
	double DensityBackCalculation(double elasticity);
	
	/** extend this function to add exporter specific GUI */
	virtual void AddSpecificGui() {};

	int m_EnableBackCalculation;
	bool m_HasConfiguration;

	albaGUIRollOut *m_BackPropRollOut;
	albaGUIRollOut *m_GuiRollOutDensityOneInterval;
	albaGUIRollOut *m_GuiRollOutDensityThreeIntervals;

	albaGUI *m_BackPropGui;
	albaGUI *m_GuiASDensityOneInterval;
	albaGUI *m_GuiASDensityThreeIntervals;

	BonematConfiguration m_Configuration;

	albaProgressBarHelper *m_ProgressHelper;
	float m_TotalElements;
	long m_CurrentProgress;

	vtkFieldData *m_MaterialData;
	vtkIdType *m_MatIdArray;

	double m_Egap;

	int m_ABSMatrixFlag;
};
#endif
