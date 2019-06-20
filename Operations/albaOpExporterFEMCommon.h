/*=========================================================================

Program: ALBA
Module: albaOpExporterFEMCommon.h
Authors: Nicola Vanella

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

#include "vtkUnstructuredGrid.h"
#include "vtkIntArray.h"
#include "albaGUI.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

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

enum DensitySelection
{
	USE_MEAN_DENSISTY,
	USE_MAXIMUM_DENSITY,
};

//----------------------------------------------------------------------------
// albaOpExporterFEMCommon :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpExporterFEMCommon : public albaOp
{
public:
		
	albaOpExporterFEMCommon(const wxString &label = "albaOpExporterFEMCommon");
	~albaOpExporterFEMCommon(); 

	albaTypeMacro(albaOpExporterFEMCommon, albaOp);

	enum EXPORTER_FEM_ID
	{
		ID_FIRST = MINID,
		ID_GAP_VALUE,
		ID_DENSITY_SELECTION,
		ID_POISSON_RATIO,
		ID_FREQUENCY_FILE_NAME,
		MINID,
	};

	vtkIdType * GetMatIdArray();

	vtkFieldData *GetMaterialData();

	void SetDefaultFrequencyFile();

protected:

	/** Create the dialog interface for the exporter. */
	virtual void CreateGui();

	bool  HasMaterials();

	/** Creates material Bins By grouping element props, internally sorts elProps vector  and save frequency file */
	void CreateBins(int numElements, MaterialProp *elProps, std::vector<MaterialProp> *materialProperties);

	/** Generates Arrays And MaterialsData from element properties vector */
	vtkFieldData* CreateMaterialsData(std::vector <MaterialProp>materialProperties);

	/** Static function witch compares two ElementProps used for qsort in decreasing order */
	static int compareE(const void *p1, const void *p2);
	
private:

	vtkFieldData *m_MaterialData;
	vtkIdType *m_MatIdArray;

	double m_Egap;

	// Advanced Configuration
	int m_DensitySelection;

	albaString m_FrequencyFileName;
	FILE *m_Freq_fp;
};
#endif
