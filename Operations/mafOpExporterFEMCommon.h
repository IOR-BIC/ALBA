/*=========================================================================

Program: MAF2
Module: mafOpExporterFEMCommon.h
Authors: Nicola Vanella

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterFEMCommon_H__
#define __mafOpExporterFEMCommon_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

#include "vtkUnstructuredGrid.h"
#include "vtkIntArray.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

// Element Properties: element ID, density, Young Module
typedef struct {
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
// mafOpExporterFEMCommon :
//----------------------------------------------------------------------------
class MAF_EXPORT mafOpExporterFEMCommon : public mafOp
{
public:
		
	mafOpExporterFEMCommon(const wxString &label = "mafOpExporterFEMCommon");
	~mafOpExporterFEMCommon(); 

	mafTypeMacro(mafOpExporterFEMCommon, mafOp);

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

protected:

	/** Create the dialog interface for the importer. */
	virtual void CreateGui();

	bool  HasMaterials();
		
	
	void GenerateArraysAndMaterialsData(vtkIdType numElements, MaterialProp *elProps, vtkUnstructuredGrid *inputUG);
	void CreateBins(int numElements, MaterialProp *elProps, std::vector<MaterialProp> *materialProperties);
	vtkFieldData* CreateMaterialsData(std::vector <MaterialProp>materialProperties);

	/** Static function witch compares two ElementProps used for qsort in decreasing order */
	static int compareE(const void *p1, const void *p2);
	
private:

	vtkFieldData *m_MaterialFieldData;
	vtkIdType *m_MatIdArray;

	double m_Egap;

	// Advanced Configuration
	int m_DensitySelection;

	mafString m_FrequencyFileName;
	FILE *m_Freq_fp;
};
#endif
