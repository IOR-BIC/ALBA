/*=========================================================================
Program:   Bonemat
Module:    albaOpMeshScarlarsImporter.h
Language:  C++
Date:      $Date: 2010-11-23 16:50:26 $
Version:   $Revision: 1.1.1.1.2.3 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpMeshScarlarsImporter_H__
#define __albaOpMeshScarlarsImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaTextFileReaderHelper.h"
#include "vtkSystemIncludes.h"

class vtkFloatArray;


//----------------------------------------------------------------------------
// albaOpMeshScarlarsImporter :
//----------------------------------------------------------------------------
/** 
Reads Scalars from Lis files
*/
class ALBA_EXPORT albaOpMeshScarlarsImporter : public albaTextFileReaderHelper, public albaOp
{
public:

	albaOpMeshScarlarsImporter(const wxString &label = "Load Scalars from Lis files");
	~albaOpMeshScarlarsImporter(); 

	albaTypeMacro(albaOpMeshScarlarsImporter, albaOp);

	enum ScalarScopes {
		NODE_SCALARS,
		ELEMENT_SCALARS,
	};

	enum CheckIDResults {
		PERFECT_MATCH_IDS,
		SUBGROUP_IDS,
		WRONG_IDS,
	};

	/** Builds operation's interface. */
	void OpRun();

	albaOp* Copy();

	/** imports Scalars from files */
	int ImportFile(void);
	
	/** Get the Scalar file name*/
	albaString GetLisScalarFileName() const { return m_LisScalarFile; }

	/** Set the Scalar file name*/
	void SetLisScalarFileName(albaString val) { m_LisScalarFile = val; }

	/** Answer to the messages coming from interface. */
	virtual void OnEvent(albaEventBase* alba_event);

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);

	/** Create the dialog interface for the importer. */
	void CreateGui();

	albaString m_LisScalarFile;

	ScalarScopes m_ScalarScope;

	std::vector<albaString> m_ScalarNames;
	std::vector<vtkFloatArray*> m_Arrays;
	std::vector<vtkIdType> m_Ids;
	float* m_DefaultValues;

	CheckIDResults IDsToIndexes();

	vtkIdType m_MeshScalarNum;

	void DeleteArrays();
	int ReadHeader();
	int ReadLine();
private:
	void CreateNewScalars();
};
#endif
